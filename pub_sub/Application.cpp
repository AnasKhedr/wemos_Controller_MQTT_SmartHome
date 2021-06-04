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

Application::Application()
{
    //starting up the serial
    Serial.begin(115200);
    Serial.println("start of Application");

    //factory
    m_ControlGPIOsList["light1"] = std::make_shared<bathRoom::bathRoomGPIO>(D1, bathRoom::GPIOtype::control,"light1");
    m_ControlGPIOsList["light2"] = std::make_shared<bathRoom::bathRoomGPIO>(D2, bathRoom::GPIOtype::control,"light2");
    m_ControlGPIOsList["light3"] = std::make_shared<bathRoom::bathRoomGPIO>(D3, bathRoom::GPIOtype::control,"light3");
}

void Application::addClient(std::string brokerIp)
{
    Serial.printf("adding broker ip: %s to list.\n", brokerIp.c_str());
    m_brokerIps.push_back(brokerIp);
}

void Application::init()
{

    Serial.println("connected to WIFI network. Blocking execution until connected!");
    m_wifiManager.autoConnect(m_wifiManager.getDefaultAPName().c_str(), "__1234*5678__");
    Serial.println("connected to WIFI network.");

    // creating clients to all the different brokerIps
    for(auto& oneIp : m_brokerIps)
    {
        Serial.printf("creating a new client to broker: %s\n", oneIp.c_str());
        m_mqttClients.push_back(std::make_unique<mqtt::mqttClient>(oneIp, "#",
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
        Serial.printf("Device: %s does not exist!\n", device.c_str());
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

    for(auto&& oneClient : m_mqttClients)
    {
        oneClient->loop();
        // Serial.println("publishing.");
        // oneClient->publish("Test",std::to_string(x).c_str());
    }

    // delay(1000);
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

}       //namespace app