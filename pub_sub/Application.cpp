//---------------------------------------------------------------------------
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
#include <optional>
#include <EEPROM.h>

//---------------------------------------------------------------------------
// Functions
//---------------------------------------------------------------------------

namespace app
{

Application::Application() :
    m_ads(0x48),
    m_lastSentTime(g_persistantData.sensorsReadingsUpdateInterval),
    m_MQ4Sensor(std::nullopt, m_ads, ADSA1, INPUT),
    m_MQ2Sensor(std::nullopt, m_ads, ADSA2, INPUT),
    m_RCWLSensor(RCWLPIN)
{
    //initling EEPROM
    EEPROM.begin(EEPROMSIZE);

    //starting up the serial
    Serial.begin(115200);
    delay(10);
    Serial.println("start of Application");

    m_ads.setGain(GAIN_TWOTHIRDS);  // 2/3x gain +/- 6.144V  1 bit = 3mV      0.1875mV (default)
    // m_wifiManager.resetSettings();
    // m_wifiManager.setAPStaticIPConfig(IPAddress(192,168,1,110), IPAddress(192,168,1,1), IPAddress(255,255,255,0));

    //factory
    ///TODO: something is fishy about the creation of those objects, check is
    m_ControlGPIOsList["mainLight"] = std::make_shared<bathRoom::bathRoomGPIO>(MAINLIGHTPIN, bathRoom::GPIOtype::activeLow,"mainLight",MAINLIGHTBUTTONPIN);
    m_ControlGPIOsList["washbasineLight"] = std::make_shared<bathRoom::bathRoomGPIO>(WASHBASINELIGHTPIN, bathRoom::GPIOtype::activeLow,"washbasineLight",WASHBASINELIGHTBUTTONPIN);
    m_ControlGPIOsList["ventilator"] = std::make_shared<bathRoom::bathRoomGPIO>(VENTILATORPIN, bathRoom::GPIOtype::activeLow,"ventilator",VENTILATORBUTTONPIN);
    m_ControlGPIOsList["buzzer"] = std::make_shared<bathRoom::bathRoomGPIO>(BUZZERCONTROLPIN, bathRoom::GPIOtype::activeHigh,"buzzer");

    m_RCWLSensor.setRelayControllerObject(m_ControlGPIOsList["mainLight"]);

}

void Application::addClient(std::string brokerIp)
{
    Serial.printf("adding broker ip: %s to list.\n", brokerIp.c_str());
    m_brokerIps.push_back(brokerIp);
}

void Application::init()
{
    readFromEEPROM(PESISTANTEEPROMIDX, g_persistantData);
    m_RCWLSensor.changeLightEnable(g_persistantData.MotionEnable);


    Serial.println("connecting to WIFI network. Blocking execution until connected!");
    // m_wifiManager.autoConnect(m_wifiManager.getDefaultAPName().c_str(), "1234567809");
    m_wifiManager.autoConnect();
    Serial.println("connected to WIFI network.");

    Serial.println("initializing DHT11 and ADS1115 Sensors");
    m_dhtSensor.setup(DHT11PIN, DHTesp::DHT11);
    m_ads.begin();
    Serial.println("done initializing dht11 and ADS1115.");

    // creating clients to all the different brokerIps
    for(auto& oneIp : m_brokerIps)
    {
        Serial.printf("creating a new client to broker: %s\n", oneIp.c_str());
        // subscribe to the commands that we should act on only
        m_mqttClients.push_back(std::make_shared<mqtt::mqttClient>(oneIp, std::string{bathRoomInputCommands}+"#",
            [this](char *topic, uint8_t *payload, unsigned int length) -> void
            {
                std::string message(reinterpret_cast<char*>(payload), length);
                helper::printMqttMessage(topic, message);
                onMqttMessage(topic, message);
            }));
    }

    // init-ing all the broker connections that were created.
    // for(auto&& oneClient : m_mqttClients)
    bool status = false;
    // at least connect to one broker
    while(!status)
    {
        for(auto& oneClient : m_mqttClients)
        {
            Serial.println("start initializing over m_mqttClients");
            status |= oneClient->init();
        }

        if(!status)
        {
            Serial.print("Failed to connect to all brokers, retrying in 2 seconds.\n");
            yield();
            delay(1000);
            yield();
        }
    }

    // for(auto& oneClient : m_mqttClients)
    // {
    //     Serial.println("start initializing over m_mqttClients");
    //     oneClient->init();
    //     Serial.println("done initializing over m_mqttClients");
    // }
}

void Application::onMqttMessage(const std::string& topic, const std::string& message)
{
    std::string room;
    std::string device;
    splitRoomAndSensor(topic, room, device);
    Serial.printf("room: %s, device: %s, and message: %s\n", room.c_str(), device.c_str(), message.c_str());

    //if the received topic is for bathroom
    if(room.compare(bathRoomInputCommands) == 0)
    {
        if(m_ControlGPIOsList.find(device) == m_ControlGPIOsList.end())
        {
            checkForConfigUpdate(device, message);
            // Serial.printf("Device: %s does not exist!\n", device.c_str());
            return;
        }
        else
        {
            m_ControlGPIOsList[device]->act(static_cast<helper::actions>(std::stoi(message)));
        }
    }
    else
    {
        Serial.println("Message is not a bathroom control message, ignoring message.");
    }

}


void Application::run()
{
    // Serial.println("run instance");
    m_timerTasks.tick();

    // checking messages form all brokers
    for(auto&& oneClient : m_mqttClients)
    {
        oneClient->loop();
        // Serial.println("publishing.");
        // oneClient->publish("Test",std::to_string(x).c_str());
    }

    ///TODO: refactor this --> task
    // checking if any GPIO button was pushed.
    for(auto it = m_ControlGPIOsList.begin(); it != m_ControlGPIOsList.end(); it++)
    {
        it->second->checkButton(m_mqttClients);
    }

    checkHazeredSensors();
    updateSensorsReadings();
    checkMothion();
    checkMothion();
}

void Application::splitRoomAndSensor(const std::string fullTopic, std::string& room, std::string& device)
{
    // this is the example from find_last_of api documentation:
    // https://www.cplusplus.com/reference/string/string/find_last_of/
    std::size_t found = fullTopic.find_last_of("/");
    ///VIOLATION: magic number, justification: it can't be avoided!
    room = fullTopic.substr(0,found+1);       // from 0 to the last(including) / found
    device = fullTopic.substr(found+1);     // from just after the last / found to the end
}

void Application::updateSensorsReadings()
{
    ///TODO: refactor this --> task
    // send temp and humi every 10 seconds
    if((millis() - m_lastSentTime) > g_persistantData.sensorsReadingsUpdateInterval)
    {
        debugV("* This is a message of debug level VERBOSE");
        Serial.printf("(sensorsReadingsUpdateInterval) %dms have passed, sending readings.\n",
                        g_persistantData.sensorsReadingsUpdateInterval);
        Debug.printf("(sensorsReadingsUpdateInterval) %dms have passed, sending readings.\n",
                        g_persistantData.sensorsReadingsUpdateInterval);
        // updateing the time.
        m_lastSentTime = millis();

        uint8_t counter = 0;

        do
        {
            counter++;
            m_temperature = m_dhtSensor.getTemperature();
            // Serial.printf("Reading humidity\n");
            delay(1);
        }
        while((m_temperature == NAN) && (counter < 250));

        m_humidity = m_dhtSensor.getHumidity();
        Serial.printf("Interval have passed or #retries: %d, updateing Temperature: %f C and Humidity: %f%%\n",
                            counter, m_temperature, m_humidity);
        Debug.printf("Interval have passed or #retries: %d, updateing Temperature: %f C and Humidity: %f%%\n",
                            counter, m_temperature, m_humidity);
        debugA("Sending reading to broker\n");

        if(m_temperature == NAN)
        {
            Serial.println("Failed to get DHT11 reading, skipping updateing.");
        }
        else
        {
        }

        for(auto&& oneClient : m_mqttClients)
        {
            if(m_temperature != NAN)
            {
                //DHT11 readings
                oneClient->publish(std::string(bathRoomInfoData)+"temp", std::to_string(m_temperature));
                oneClient->publish(std::string(bathRoomInfoData)+"humi", std::to_string(m_humidity));

                //MQ4 readings
                oneClient->publish(std::string(bathRoomInfoData)+"mq4Analog", std::to_string(m_MQ4Sensor.readAnalogValue()));
                // oneClient->publish(std::string(bathRoomInfoData)+"mq4Digital", std::to_string(m_MQ4Sensor.readDigitalValue()));

                //MQ2 readings
                oneClient->publish(std::string(bathRoomInfoData)+"mq2Analog", std::to_string(m_MQ2Sensor.readAnalogValue()));
                // oneClient->publish(std::string(bathRoomInfoData)+"mq2Digital", std::to_string(mq2SensorValueDigital));
            }
            else
            {
                Serial.println("Failed to get DHT11 readings after 250 retries!");
            }
        }
        // Convert the analog reading (which goes from 0 - 1023) to a voltage (0 - 3.2V):
        // float voltage = sensorValue * (3.2 / 1023.0);
        // print out the value you read:
        // Serial.printf("voltage of M4 sensor is: %f, digital: %d\n",sensorValue, sensorValueDigital);
    }


}

void Application::checkHazeredSensors()
{
    static unsigned long lastReadTimer = millis();

    // check reading every 1 second
    if((millis() - lastReadTimer) > ONESECOND)
    {
        lastReadTimer = millis();
        //refactor this to tell-don't-ask
        helper::state buzzerState = m_ControlGPIOsList["buzzer"]->whatIsDeviceState();

        ///TODO: add a way to check for analog reading and use it to turn on\off
        ///TODO: turn on\off buzzer every 0.5S to make a warning sound insteat of alway on
        // activate the buzzer(alarm sound) only if one of the 2 sensors detects a high consentration.
        if(( buzzerState == helper::OFF ) &&
            (m_MQ4Sensor.readAnalogValue() >= g_persistantData.MQ4AnalogThrethhold) ||
            (m_MQ2Sensor.readAnalogValue() >= g_persistantData.MQ2AnalogThrethhold))
        {
            // if the buzzer is off and there is a gas leakage
            Serial.println("Detected Dangerous gas levels, starting the buzzer.");
            m_ControlGPIOsList["buzzer"]->switchOn();

            // update Nymea(broker)
            for(auto&& oneClient : m_mqttClients)
            {
                oneClient->publish(std::string(bathRoomInfoData)+"hazeredGasState", std::string("1"));
                oneClient->publish(std::string(bathRoomInfoData)+"mq4Analog", std::to_string(m_MQ4Sensor.readAnalogValue()));
                // oneClient->publish(std::string(bathRoomInfoData)+"mq4Digital", std::to_string(mq4SensorValueDigital));

                oneClient->publish(std::string(bathRoomInfoData)+"mq2Analog", std::to_string(m_MQ2Sensor.readAnalogValue()));
                // oneClient->publish(std::string(bathRoomInfoData)+"mq2Digital", std::to_string(mq2SensorValueDigital));
            }
        }
        else if(buzzerState == helper::ON)
        {
            for(auto&& oneClient : m_mqttClients)
            {
                oneClient->publish(std::string(bathRoomInfoData)+"hazeredGasState", std::string("0"));
                oneClient->publish(std::string(bathRoomInfoData)+"mq4Analog", std::to_string(m_MQ4Sensor.readAnalogValue()));
                // oneClient->publish(std::string(bathRoomInfoData)+"mq4Digital", std::to_string(mq4SensorValueDigital));

                oneClient->publish(std::string(bathRoomInfoData)+"mq2Analog", std::to_string(m_MQ2Sensor.readAnalogValue()));
                // oneClient->publish(std::string(bathRoomInfoData)+"mq2Digital", std::to_string(mq2SensorValueDigital));
            }
            // buzzer is on but there is no gas leakage
            Serial.println("gas level is normal, stopping the buzzer.");
            m_ControlGPIOsList["buzzer"]->switchOff();
        }
    }
    else
    {
        return;
    }
}

void Application::checkForConfigUpdate(const std::string& command, const std::string& message)
{
    if(command == "reset")
    {
        Serial.println("Received reset command.");
        m_wifiManager.resetSettings();
    }
    else if(command == "MQ2ThretholdUpdate")
    {
        Serial.printf("update MQ2 Threthold command with data: %s\n", message.c_str());
        g_persistantData.MQ2AnalogThrethhold = std::stof(message);
        writeToEEPROM(PESISTANTEEPROMIDX, g_persistantData);
    }
    else if(command == "MQ4ThretholdUpdate")
    {
        Serial.printf("update MQ4 Threthold command with data: %s\n", message.c_str());
        g_persistantData.MQ4AnalogThrethhold = std::stof(message);
        writeToEEPROM(PESISTANTEEPROMIDX, g_persistantData);
    }
    else if(command == "MothionSensorEnable")
    {
        bool enable = std::stoi(message);
        Serial.printf("update motion sensor enable command with data: %s\n", message.c_str());
        m_RCWLSensor.changeLightEnable(enable);
        g_persistantData.MotionEnable = enable;
        writeToEEPROM(PESISTANTEEPROMIDX, g_persistantData);
    }
    else if(command == "sensorsReadingsUpdateInterval")
    {
        int sensorsInterval = std::stoi(message);
        Serial.printf("update the send interval for sensors readings command received with data: %s\n", message.c_str());
        g_persistantData.sensorsReadingsUpdateInterval = sensorsInterval * 1000UL;  // 1000 to convert from seconds to ms
        writeToEEPROM(PESISTANTEEPROMIDX, g_persistantData);
    }
    else if(command == "motionSensorLightActiveTime")
    {
        int motionInterval = std::stoi(message);
        Serial.printf("update the motion sensor turn on time command received with data: %s\n", message.c_str());
        g_persistantData.motionSensorLightActiveTime = motionInterval * 1000UL;     // 1000 to convert from seconds to ms
        writeToEEPROM(PESISTANTEEPROMIDX, g_persistantData);
    }

}

void Application::checkMothion()
{
    m_RCWLSensor.controlLight();
}

template<typename T>
void Application::writeToEEPROM(int address, T xdata)
{
    EEPROM.put(address, xdata);
    EEPROM.commit();

    Serial.printf("Write to Persistance storage - mq2 threshold: %f, mq4 threshold: %f, MotionEnable: %d,"
                    " sensorsReadingsUpdateInterval: %u, motionSensorLightActiveTime: %u",
                    xdata.MQ2AnalogThrethhold, xdata.MQ4AnalogThrethhold, xdata.MotionEnable,
                    xdata.sensorsReadingsUpdateInterval, xdata.motionSensorLightActiveTime);
}

template<typename T>
void Application::readFromEEPROM(int address, T& xdata)
{
    EEPROM.get(address, xdata);

    Serial.printf("Read from Persistance storage - mq2 threshold: %f, mq4 threshold: %f, MotionEnable: %d\n",
                    xdata.MQ2AnalogThrethhold, xdata.MQ4AnalogThrethhold, xdata.MotionEnable);
}

}       //namespace app