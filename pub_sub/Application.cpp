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

Application::Application()
{

}

void Application::addClient(std::string brokerIp)
{
    Serial.printf("creating a new client to broker: %s\n", brokerIp.c_str());
    m_mqttClients.push_back(std::make_unique<mqtt::mqttClient>(brokerIp, "#"));
}

void Application::init()
{
    //starting up the serial
    Serial.begin(115200);
    Serial.println("start of setup");

    Serial.println("connected to WIFI network. Blocking execution until connected!");
    m_wifiManager.autoConnect(m_wifiManager.getDefaultAPName().c_str(), "__1234*5678__");
    Serial.println("connected to WIFI network.");

    // creating clients to all the different brokerIps
    for(auto& oneIp : m_brokerIps)
    {
        m_mqttClients.push_back(std::make_unique<mqtt::mqttClient>(oneIp, "#"));   //if used smart pointer
        // m_mqttClients.push_back(mqtt::mqttClient(oneIp, "#"));
    }

    // init-ing all the broker connections that were created.
    // for(auto&& oneClient : m_mqttClients)
    for(auto& oneClient : m_mqttClients)
    {
        oneClient->init();
    }
}

void Application::run()
{
    m_timerTasks.tick();

    for(auto&& oneClient : m_mqttClients)
    {
        oneClient->loop();
    }

}