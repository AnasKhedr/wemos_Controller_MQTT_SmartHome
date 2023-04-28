//! \file Application.cpp
//! \authors Anas Khedr
//! \brief
//! \version 0.1
//! \date 29-May-2021
//! \copyright This work is licensed under a Creative Commons Attribution 4.0 International License. Copyright (c) 2021
//!

//---------------------------------------------------------------------------
// Includes
//---------------------------------------------------------------------------
#include "Application.hpp"
#include <DNSServer.h>
#include <EEPROM.h>
#include <ESP8266mDNS.h>
#include <optional>

//---------------------------------------------------------------------------
// Functions
//---------------------------------------------------------------------------

namespace app
{
Application::Application() : m_lastSentTime(g_persistantData.sensorsReadingsUpdateInterval), m_resolver(m_udp, DEVICEIP)
{
    /// TODO: this may cause a problem because serial monitor uses TX and RX pins that are already used for relay pins
    Serial.begin(115200);
    // initling EEPROM
    EEPROM.begin(EEPROMSIZE);

    // starting up the serial
    // _PRINTLN("start of Application");

    // m_ads.setGain(GAIN_TWOTHIRDS);  // 2/3x gain +/- 6.144V  1 bit = 3mV      0.1875mV (default)
    // m_wifiManager.resetSettings();
    // m_wifiManager.setAPStaticIPConfig(IPAddress(192,168,1,110), IPAddress(192,168,1,1), IPAddress(255,255,255,0));
}

void Application::addBroker(const std::string& brokerHostname)
{
    _PRINTF("adding broker with hostname: %s to list.\n", brokerHostname.c_str());

    std::string expectedHostname = brokerHostname + ".local";
    IPAddress ip = hostnameToIP(expectedHostname);
    std::string ipString = ip.toString().c_str();

    // if IP was not resolved successfully then insert ip as empty to attempt to resolve it at a later time
    if (!ip)
    {
        _PRINTF("failed to resolve ip of hostname: %s | ip %s\n", brokerHostname.c_str(), ip.toString().c_str());
        m_resolveFailedHostnames.push_back(expectedHostname);

        // add the hostname to a task that will attempt to resolve it at a later time
        /// TODO: this may cause a problem because the resolver takes too much time
        m_timerTasks.every(
            10000UL,
            [](void* app) -> bool {
                auto application_ptr = static_cast<Application*>(app);

                for (std::vector<std::string>::iterator oneHostnameIt = application_ptr->m_resolveFailedHostnames.begin();
                     oneHostnameIt != application_ptr->m_resolveFailedHostnames.end(); ++oneHostnameIt)
                {
                    _PRINTF("[task]attempting to resolve hostname: %s\n", (*oneHostnameIt).c_str());
                    if (IPAddress ip = application_ptr->hostnameToIP(*oneHostnameIt); ip)
                    {
                        application_ptr->m_brokerHostnameIPMap.emplace(*oneHostnameIt, ip.toString().c_str());
                        application_ptr->m_resolveFailedHostnames.erase(oneHostnameIt);

                        // setup the broker from the resolved ip address
                        application_ptr->setupBroker(*oneHostnameIt, ip.toString().c_str());
                    }
                }

                // true to repeat, false to stop
                return (application_ptr->m_resolveFailedHostnames.size() != 0);
            },
            this);
    }
    else
    {
        _PRINTF("ip of hostname: %s was resolved successfully to: %s\n", brokerHostname.c_str(), ipString.c_str());
        m_brokerHostnameIPMap.emplace(brokerHostname, ipString);
    }
}

IPAddress Application::hostnameToIP(const std::string& hostname)
{
    IPAddress ip = m_resolver.search(hostname.c_str());
    _PRINTF("IP address of host: %s is : %s\n", hostname.c_str(), ip.toString().c_str());
    yield(); // the lookup of hostname ip sometime takes a bit of time so yield just to be sure

    return ip;
}

void Application::init()
{
    initWifi();

    readFromEEPROM(PESISTANTEEPROMIDX, g_persistantData);

    _PRINTF("g_persistantData data in EEPROM - mq2 threshold: %f, mq4 threshold: %f, MotionEnable: %d,"
            " sensorsReadingsUpdateInterval: %lu, motionSensorLightActiveTime: %lu\n",
            g_persistantData.MQ2AnalogThrethhold, g_persistantData.MQ4AnalogThrethhold, g_persistantData.MotionEnable,
            g_persistantData.sensorsReadingsUpdateInterval, g_persistantData.motionSensorLightActiveTime);

    initGpioDevices();
}

void Application::initWifi()
{
    m_wifiManager.setSTAStaticIPConfig(DEVICEIP, IPAddress(192, 168, 1, 1), IPAddress(255, 255, 255, 0)); // optional DNS 4th argument
    m_wifiManager.setWiFiAutoReconnect(true);
    m_wifiManager.setShowInfoErase(true);

    _PRINTLN("connecting to WIFI network. Blocking execution until connected!");
    // m_wifiManager.autoConnect(m_wifiManager.getDefaultAPName().c_str(), "1234567809");
    _PRINTLN("connected to WIFI network.");
    m_wifiManager.setConfigPortalTimeout(60);
    m_wifiManager.setConfigPortalBlocking(false);
    m_wifiManager.setWiFiAutoReconnect(true);
    m_wifiManager.autoConnect("Office-ESP");
    m_wifiManager.setHostname("Office-ESP");
    m_wifiManager.setConnectTimeout(300); // how much time to wait in AP mode before returning to try connecting to saved wifi
    m_wifiManager.setSaveConnectTimeout(300); // I think it's how much time to keep trying to search for and connect to saved wifis

    initRemoteDebug();
}

void Application::initGpioDevices()
{
    _PRINT("starting creating GPIOs.\n");
    // factory
    /// TODO: something is fishy about the creation of those objects, check is
    /// the issue with this code is that we're using serial pins(TX, RX) which is messing up the serial port but the code is working
    m_ControlGPIOsList.emplace(
        "mainLight", std::make_unique<office::officeGPIO>(MAINLIGHTPIN, office::GPIOtype::activeLow, "mainLight", MAINLIGHTBUTTONPIN));
    m_ControlGPIOsList.emplace(
        "neerLight", std::make_unique<office::officeGPIO>(NEERLIGHTPIN, office::GPIOtype::activeLow, "nearLight", NEERLIGHTBUTTONPIN));
    m_ControlGPIOsList.emplace("farLight",
                               std::make_unique<office::officeGPIO>(FARLIGHTPIN, office::GPIOtype::activeLow, "farLight", FARBUTTONPIN));
    m_ControlGPIOsList.emplace(
        "ableekLight", std::make_unique<office::officeGPIO>(ABLEEKLIGHTPIN, office::GPIOtype::activeLow, "ableekLight", ABLEEKBUTTONPIN));
    m_ControlGPIOsList.emplace("doorLight",
                               std::make_unique<office::officeGPIO>(DOORLIGHTPIN, office::GPIOtype::activeLow, "doorLight", DOORBUTTONPIN));
    _PRINT("done creating GPIOs.\n");

    _PRINTLN("initializing DHT11 and ADS1115 Sensors");
    m_dhtSensor.setup(DHT11PIN, DHTesp::DHT11);
    _PRINTLN("done initializing dht11 and ADS1115.");
}

void Application::initRemoteDebug()
{
    if (MDNS.begin(HOST_NAME))
    {
        // Serial.printf("* MDNS responder started. Hostname -> %s\n", HOST_NAME);
    }

    // enable the telnet debug logs
    MDNS.addService("telnet", "tcp", 23); // Telnet server RemoteDebug
    Debug.begin(HOST_NAME); // Initiaze the telnet server
    Debug.setResetCmdEnabled(true); // Enable the reset command
    Debug.showProfiler(true); // Profiler (Good to measure times, to optimize codes)
    Debug.showColors(true); // Colors
    Debug.handle();
}

void Application::setupBrokers()
{
    // creating clients to all the different brokerIps
    for (auto& [oneHostname, oneIp] : m_brokerHostnameIPMap)
    {
        setupBroker(oneHostname, oneIp);
    }
}

void Application::setupBroker(const std::string& hostname, const std::string& hostIP)
{
    _PRINTF("creating a new client to broker: %s with ip: %s\n", hostname.c_str(), hostIP.c_str());

    /// NOTE: toString doesn't return an std::string
    m_brokerHostnameIPMap[hostname] = hostIP.c_str();

    // subscribe to the commands that we should act on only
    m_mqttClients.emplace_back(std::make_unique<mqtt::mqttClient>(hostIP, std::string {officeInputCommands} + "#"));

    m_mqttClients.back()->addObserver(*this);
    // loop on all our brokers to connect our ESP to them
    m_brokerStatus &= connectToBroker(hostname);
}

bool Application::connectToBroker(const std::string& mqttBrokerHostname)
{
    bool brokerStatus = false;

    for (auto& mqttClient : m_mqttClients)
    {
        if (mqttClient->getBrokerIp() == m_brokerHostnameIPMap[mqttBrokerHostname])
        {
            brokerStatus = mqttClient->init();
            _PRINTF("connection attempt to mqtt broker with ip: %s result: %d\n", mqttClient->getBrokerIp().c_str(), brokerStatus);

            // if failed to connect to at least one broker
            if (brokerStatus)
            {
                _PRINTLN("Succeeded in connecting to one of the mqtt clients, Will try to connect to unconnected(if any) brokers during "
                         "runtime.");
            }
        }
    }

    return brokerStatus;
}

void Application::onMqttMessage(const std::string& topic, const std::string& message)
{
    std::string room;
    std::string device;
    splitRoomAndSensor(topic, room, device);
    _PRINTF("room: %s, device: %s, and message: %s\n", room.c_str(), device.c_str(), message.c_str());

    // if the received topic is for office
    if (room.compare(officeInputCommands) == 0)
    {
        if (m_ControlGPIOsList.find(device) == m_ControlGPIOsList.end())
        {
            checkForConfigUpdate(device, message);
            _PRINTF("Device: %s does not exist!\n", device.c_str());
            return;
        }
        else
        {
            m_ControlGPIOsList[device]->act(static_cast<helper::actions>(std::stoi(message)));
        }
    }
    else
    {
        _PRINTLN("Message is not a office control message, ignoring message.");
        Debug.println("Message is not a office control message, ignoring message.");
    }
}

void Application::onMqttDisconnected(const std::string& brokerIP) {}

void Application::run()
{
    // _PRINTLN("run instance");
    // _PRINTF("bla bla bla: %d %s", 10, "str\n"); // OR
    // Debug.printf("bla bla bla: %d %s", 20, "str.c_str()"); // Note: if type is String need c_str() // OR
    // Debug.println("bla bla bla 2 ln");
    m_timerTasks.tick();
    m_wifiManager.process();

    /// TODO: enable this and fix if any problems are found
    // updateSensorsReadings();

    // checking messages form all brokers
    for (auto&& oneClient : m_mqttClients)
    {
        // this function will attempt to reconnect to mqtt broker if connection was lost
        m_brokerStatus |= oneClient->loop();
        // _PRINTLN("publishing.");
        // oneClient->publish("Test",std::to_string(x).c_str());
    }

    /// TODO: refactor this --> task
    // checking if any GPIO button was pushed.
    for (auto it = m_ControlGPIOsList.begin(); it != m_ControlGPIOsList.end(); it++)
    {
        it->second->checkButton(m_mqttClients);
    }

    Debug.handle();
}

void Application::splitRoomAndSensor(const std::string fullTopic, std::string& room, std::string& device)
{
    // this is the example from find_last_of api documentation:
    // https://www.cplusplus.com/reference/string/string/find_last_of/
    std::size_t found = fullTopic.find_last_of("/");
    /// VIOLATION: magic number, justification: it can't be avoided!
    room = fullTopic.substr(0, found + 1); // from 0 to the last(including) / found
    device = fullTopic.substr(found + 1); // from just after the last / found to the end
}

void Application::updateSensorsReadings()
{
    /// TODO: refactor this --> task
    // send temp and humi every 10 seconds
    if ((millis() - m_lastSentTime) > g_persistantData.sensorsReadingsUpdateInterval)
    {
        // if(!m_brokerStatus)
        // {
        //     _PRINTF("(sensorsReadingsUpdateInterval) %dms have passed, but Working in offline mode, skipping updateing sensors
        //     State.\n",
        //                 g_persistantData.sensorsReadingsUpdateInterval);
        //     return;
        // }

        debugV("* This is a message of debug level VERBOSE");
        _PRINTF("(sensorsReadingsUpdateInterval) %lu ms have passed, sending readings.\n", g_persistantData.sensorsReadingsUpdateInterval);
        // updateing the time.
        m_lastSentTime = millis();

        uint8_t counter = 0;

        do
        {
            counter++;
            m_temperature = m_dhtSensor.getTemperature();
            // _PRINTF("Reading humidity\n");
            delay(1);
        } while ((m_temperature == NAN) && (counter < 10));

        m_humidity = m_dhtSensor.getHumidity();
        _PRINTF("Interval have passed or #retries: %d, updateing Temperature: %f C and Humidity: %f%%\n", counter, m_temperature,
                m_humidity);
        // _PRINTF("mq4Analog: %f, mq2Analog: %f\n", m_MQ4Sensor.readAnalogValue(), m_MQ2Sensor.readAnalogValue());

        if (m_temperature == NAN)
        {
            _PRINTLN("Failed to get DHT11 reading, skipping updateing.");
        }
        else
        {
            m_temperature = helper::converTempToNymea(m_temperature);
        }

        // _PRINTLN("y");
        for (auto&& oneClient : m_mqttClients)
        {
            // _PRINTLN("x");
            if ((m_temperature != NAN))
            {
                _PRINTLN("Sending readings to Nymea.");

                // DHT11 readings
                oneClient->publish(std::string(officeInfoData) + "temperature", std::to_string(m_temperature));
                oneClient->publish(std::string(officeInfoData) + "humidity", std::to_string(m_humidity));

                // MQ4 readings
                // oneClient->publish(std::string(officeInfoData)+"mq4Analog", std::to_string(m_MQ4Sensor.readAnalogValue()));
                // // oneClient->publish(std::string(officeInfoData)+"mq4Digital", std::to_string(m_MQ4Sensor.readDigitalValue()));

                // MQ2 readings
                // oneClient->publish(std::string(officeInfoData)+"mq2Analog", std::to_string(m_MQ2Sensor.readAnalogValue()));
                // // oneClient->publish(std::string(officeInfoData)+"mq2Digital", std::to_string(mq2SensorValueDigital));
            }
            else
            {
                _PRINTLN("Failed to get DHT11 readings after 250 retries!");
            }
        }
    }
}

// void Application::checkHazeredSensors()
// {
//     static unsigned long lastReadTimer = millis();

//     // check reading every 1 second
//     if((millis() - lastReadTimer) > ONESECOND)
//     {
//         lastReadTimer = millis();
//         //refactor this to tell-don't-ask
//         helper::state buzzerState = m_ControlGPIOsList["buzzer"]->whatIsDeviceState();

//         ///TODO: add a way to check for analog reading and use it to turn on\off
//         ///TODO: turn on\off buzzer every 0.5S to make a warning sound insteat of alway on
//         // activate the buzzer(alarm sound) only if one of the 2 sensors detects a high consentration.
//         if(( buzzerState == helper::OFF ) &&
//             (m_MQ4Sensor.readAnalogValue() >= g_persistantData.MQ4AnalogThrethhold) ||
//             (m_MQ2Sensor.readAnalogValue() >= g_persistantData.MQ2AnalogThrethhold))
//         {
//             // if the buzzer is off and there is a gas leakage
//             _PRINTLN("Detected Dangerous gas levels, starting the buzzer.");
//             m_ControlGPIOsList["buzzer"]->switchOn();

//             // update Nymea(broker) if connected
//             if(m_brokerStatus)
//             {
//                 for(auto&& oneClient : m_mqttClients)
//                 {
//                     oneClient->publish(std::string(officeInfoData)+"hazeredGasState", std::string("1"));
//                     oneClient->publish(std::string(officeInfoData)+"mq4Analog", std::to_string(m_MQ4Sensor.readAnalogValue()));
//                     // oneClient->publish(std::string(officeInfoData)+"mq4Digital", std::to_string(mq4SensorValueDigital));

//                     oneClient->publish(std::string(officeInfoData)+"mq2Analog", std::to_string(m_MQ2Sensor.readAnalogValue()));
//                     // oneClient->publish(std::string(officeInfoData)+"mq2Digital", std::to_string(mq2SensorValueDigital));
//                 }
//             }
//         }
//         else if(buzzerState == helper::ON)
//         {
//             // buzzer is on but there is no gas leakage
//             _PRINTLN("gas level is normal, stopping the buzzer.");
//             m_ControlGPIOsList["buzzer"]->switchOff();

//             // update Nymea(broker) if connected
//             if(m_brokerStatus)
//             {
//                 for(auto&& oneClient : m_mqttClients)
//                 {
//                     oneClient->publish(std::string(officeInfoData)+"hazeredGasState", std::string("0"));
//                     oneClient->publish(std::string(officeInfoData)+"mq4Analog", std::to_string(m_MQ4Sensor.readAnalogValue()));
//                     // oneClient->publish(std::string(officeInfoData)+"mq4Digital", std::to_string(mq4SensorValueDigital));

//                     oneClient->publish(std::string(officeInfoData)+"mq2Analog", std::to_string(m_MQ2Sensor.readAnalogValue()));
//                     // oneClient->publish(std::string(officeInfoData)+"mq2Digital", std::to_string(mq2SensorValueDigital));
//                 }
//             }
//         }
//     }
//     else
//     {
//         return;
//     }
// }

void Application::checkForConfigUpdate(const std::string& command, const std::string& message)
{
    if (command == "reset")
    {
        _PRINTLN("Received WIFI reset command, device will be disconnected from WIFI. connected to AP to enter a new password.");
        m_wifiManager.resetSettings();
    }
    else if (command == "MQ2ThretholdUpdate")
    {
        _PRINTF("update MQ2 Threthold command with data: %s\n", message.c_str());
        g_persistantData.MQ2AnalogThrethhold = std::stof(message);
        writeToEEPROM(PESISTANTEEPROMIDX, g_persistantData);
    }
    else if (command == "MQ4ThretholdUpdate")
    {
        _PRINTF("update MQ4 Threthold command with data: %s\n", message.c_str());
        g_persistantData.MQ4AnalogThrethhold = std::stof(message);
        writeToEEPROM(PESISTANTEEPROMIDX, g_persistantData);
    }
    else if (command == "MothionSensorEnable")
    {
        bool enable = std::stoi(message);
        _PRINTF("update motion sensor enable command with data: %s\n", message.c_str());
        g_persistantData.MotionEnable = enable;
        writeToEEPROM(PESISTANTEEPROMIDX, g_persistantData);
    }
    else if (command == "sensorsReadingsUpdateInterval")
    {
        int sensorsInterval = std::stoi(message);
        _PRINTF("update the send interval for sensors readings command received with data: %s\n", message.c_str());
        g_persistantData.sensorsReadingsUpdateInterval = sensorsInterval * 1000UL; // 1000 to convert from seconds to ms
        writeToEEPROM(PESISTANTEEPROMIDX, g_persistantData);
    }
    else if (command == "mothionSensorLightActiveTime")
    {
        int motionInterval = std::stoi(message);
        g_persistantData.motionSensorLightActiveTime = motionInterval * 1000UL; // 1000 to convert from seconds to ms
        _PRINTF("update the motion sensor turn on time command received with data: %lu\n", g_persistantData.motionSensorLightActiveTime);

        writeToEEPROM(PESISTANTEEPROMIDX, g_persistantData);
    }
}

void Application::checkMothion() {}

template<typename T>
void Application::writeToEEPROM(int address, T xdata)
{
    EEPROM.put(address, xdata);
    EEPROM.commit();

    _PRINTF("Write to Persistance storage - mq2 threshold: %f, mq4 threshold: %f, MotionEnable: %d,"
            " sensorsReadingsUpdateInterval: %lu, motionSensorLightActiveTime: %lu\n",
            xdata.MQ2AnalogThrethhold, xdata.MQ4AnalogThrethhold, xdata.MotionEnable, xdata.sensorsReadingsUpdateInterval,
            xdata.motionSensorLightActiveTime);
}

template<typename T>
void Application::readFromEEPROM(int address, T& xdata)
{
    EEPROM.get(address, xdata);

    _PRINTF("Read from Persistance storage - mq2 threshold: %f, mq4 threshold: %f, MotionEnable: %d,"
            " sensorsReadingsUpdateInterval: %lu, motionSensorLightActiveTime: %lu\n",
            xdata.MQ2AnalogThrethhold, xdata.MQ4AnalogThrethhold, xdata.MotionEnable, xdata.sensorsReadingsUpdateInterval,
            xdata.motionSensorLightActiveTime);
}

} // namespace app