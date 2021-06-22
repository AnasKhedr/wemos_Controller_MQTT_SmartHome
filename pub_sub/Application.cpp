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



//---------------------------------------------------------------------------
// Functions
//---------------------------------------------------------------------------

namespace app
{

Application::Application() :
    m_ads(0x48),
    m_lastSentTime(INTERVAL),
    m_MQ4Sensor(MQ4DIGITALPIN, m_ads, ADSA0, INPUT)
{
    //starting up the serial
    Serial.begin(115200);
    Serial.println("start of Application");

    m_ads.setGain(GAIN_TWOTHIRDS);  // 2/3x gain +/- 6.144V  1 bit = 3mV      0.1875mV (default)
    // m_wifiManager.resetSettings();
    // m_wifiManager.setAPStaticIPConfig(IPAddress(192,168,1,110), IPAddress(192,168,1,1), IPAddress(255,255,255,0));

    //factory
    m_ControlGPIOsList["mainLight"] = std::make_shared<bathRoom::bathRoomGPIO>(MAINLIGHTBUTTONPIN, bathRoom::GPIOtype::activeLow,"mainLight",MAINLIGHTPIN);
    m_ControlGPIOsList["washbasineLight"] = std::make_shared<bathRoom::bathRoomGPIO>(WASHBASINELIGHTBUTTONPIN, bathRoom::GPIOtype::activeLow,"washbasineLight",WASHBASINELIGHTPIN);
    m_ControlGPIOsList["ventilator"] = std::make_shared<bathRoom::bathRoomGPIO>(VENTILATORBUTTONPIN, bathRoom::GPIOtype::activeLow,"ventilator",VENTILATORPIN);
}

void Application::addClient(std::string brokerIp)
{
    Serial.printf("adding broker ip: %s to list.\n", brokerIp.c_str());
    m_brokerIps.push_back(brokerIp);
}

void Application::init()
{

    Serial.println("connected to WIFI network. Blocking execution until connected!");
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
        m_mqttClients.push_back(std::make_shared<mqtt::mqttClient>(oneIp, "#",
            [this](char *topic, uint8_t *payload, unsigned int length) -> void
            {
                std::string message(reinterpret_cast<char*>(payload), length);
                helper::printMqttMessage(topic, message);
                onMqttMessage(topic, message);
            }));
    }

    // init-ing all the broker connections that were created.
    // for(auto&& oneClient : m_mqttClients)
    for(auto& oneClient : m_mqttClients)
    {
        Serial.println("start initializing over m_mqttClients");
        oneClient->init();
        Serial.println("done initializing over m_mqttClients");
    }
}

void Application::onMqttMessage(const std::string& topic, const std::string& message)
{
    std::string room;
    std::string device;
    splitRoomAndSensor(topic, room, device);
    Serial.printf("room: %s, device: %s, and message: %s\n", room.c_str(), device.c_str(), message.c_str());

    if(m_ControlGPIOsList.find(device) == m_ControlGPIOsList.end())
    {
        if(device == "clear")
        {
            Serial.printf("Reseting\\earasing all saved wifi network.\n");
            m_wifiManager.resetSettings();
        }
        // Serial.printf("Device: %s does not exist!\n", device.c_str());
        return;
    }
    else
    {
        m_ControlGPIOsList[device]->act(static_cast<helper::actions>(std::stoi(message)));
    }

    // // if bathroom topic was found in the received topic [ == will also do the jop]
    // if(topic.find(bedRoomGeneralTopic) != std::string::npos)
    // {
    //     // std::string key = topic.substr(idx+ std::string(bathRoomGeneralTopic).length());

    // }
    // if(topic.find(bedRoomGeneralTopic) != std::string::npos)
    // {
    //     Serial.println("bedRoomGeneralTopic topic");

    // }
    // if(topic.find(livingRoomGeneralTopic) != std::string::npos)
    // {
    //     Serial.println("livingRoomGeneralTopic topic");

    // }
    // if(topic.find(receptionGeneralTopic) != std::string::npos)
    // {
    //     Serial.println("receptionGeneralTopic topic");

    // }
    // if(topic.find(kitchenGeneralTopic) != std::string::npos)
    // {
    //     Serial.println("kitchenGeneralTopic topic");

    // }

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

    updateHazeredSensors();
    updateTemperatureAndHumidityValues();
}

void Application::splitRoomAndSensor(const std::string fullTopic, std::string& room, std::string& device)
{
    // this is the example from find_last_of api documentation:
    // https://www.cplusplus.com/reference/string/string/find_last_of/
    std::size_t found = fullTopic.find_last_of("/");
    ///VIOLATION: magic number, justification: it can't be avoided!
    room = fullTopic.substr(0,found);       // from 0 to the last / found
    device = fullTopic.substr(found+1);     // from just after the last / found to the end
}

void Application::updateTemperatureAndHumidityValues()
{
    ///TODO: refactor this --> task
    // send temp and humi every 10 seconds
    if((millis() - m_lastSentTime) > INTERVAL)
    {
        // updateing the time.
        m_lastSentTime = millis();

        uint8_t counter = 0;
        float sensorValue = m_MQ4Sensor.readAnalogValue();
        bool sensorValueDigital = !m_MQ4Sensor.readDigitalValue();

        do
        {
            counter++;
            m_temperature = m_dhtSensor.getTemperature();
            // Serial.printf("Reading humidity\n");
            delay(1);
        }
        while((m_temperature == NAN) || (counter < 250));

        m_humidity = m_dhtSensor.getHumidity();
        Serial.printf("5 seconds have passed, updateing Temperature: %f C and Humidity: %f%%\n",
                            m_temperature, m_humidity);

        if(m_temperature == NAN)
        {
            Serial.println("Failed to get DHT11 reading, skipping updateing.");
        }
        else
        {
        }

        for(auto&& oneClient : m_mqttClients)
        {
            oneClient->publish(std::string(bathRoomGeneralTopic)+"mq4Analog", std::to_string(sensorValue));
            oneClient->publish(std::string(bathRoomGeneralTopic)+"mq4Digital", std::to_string(sensorValueDigital));
            if(m_temperature != NAN)
            {
                oneClient->publish(std::string(bathRoomGeneralTopic)+"temp", std::to_string(m_temperature));
                oneClient->publish(std::string(bathRoomGeneralTopic)+"humi", std::to_string(m_humidity));
            }
            // Serial.println("publishing.");
            // oneClient->publish("Test",std::to_string(x).c_str());
        }
        // Convert the analog reading (which goes from 0 - 1023) to a voltage (0 - 3.2V):
        // float voltage = sensorValue * (3.2 / 1023.0);
        // print out the value you read:
        // Serial.printf("voltage of M4 sensor is: %f, digital: %d\n",sensorValue, sensorValueDigital);
    }


}

void Application::updateHazeredSensors()
{
    static bool mq4SensorValueDigital;
    if(mq4SensorValueDigital != m_MQ4Sensor.readDigitalValue())
    {
        mq4SensorValueDigital = m_MQ4Sensor.readDigitalValue();

        for(auto&& oneClient : m_mqttClients)
        {
            oneClient->publish(std::string(bathRoomGeneralTopic)+"mq4Analog", std::to_string(m_MQ4Sensor.readAnalogValue()));
            oneClient->publish(std::string(bathRoomGeneralTopic)+"mq4Digital", std::to_string(mq4SensorValueDigital));

            // Serial.println("publishing.");
            // oneClient->publish("Test",std::to_string(x).c_str());
        }
    }
}

}       //namespace app