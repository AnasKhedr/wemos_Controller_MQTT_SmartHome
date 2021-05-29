//---------------------------------------------------------------------------
//! \file mattClient.cpp
//! \authors Anas Khedr
//! \brief 
//! \version 0.1
//! \date 25-May-2021
//! \copyright This work is licensed under a Creative Commons Attribution 4.0 International License. Copyright (c) 2021
//! 

//---------------------------------------------------------------------------
// Includes
//---------------------------------------------------------------------------
#include "mqttClient.hpp"


//---------------------------------------------------------------------------
// Functions
//---------------------------------------------------------------------------

namespace mqtt
{
    mqttClient::mqttClient(std::string ip, std::string topic, callbackType callback) :
        m_pubSubClient(ip.c_str(), 1883, callback, m_wifiClient)
    {
        // connect(ip,topic);
    }

    void mqttClient::init()
    {
        //generate random clientID string
        String clientId = "ESP8266Client-";
        clientId += String(random(0xffff), HEX);

        Serial.print("Generated random client ID: ");
        Serial.println(clientId.c_str());
        // Serial.printf(" to connect to broker: %s\n", ip);

        while (!m_pubSubClient.connected())
        {
            Serial.println("Connecting to MQTT...");

            if (m_pubSubClient.connect(clientId.c_str()))
            {
                m_pubSubClient.subscribe("#");
                Serial.println("connected");
            }
            else
            {

                Serial.print("failed with state: ");
                Serial.println(m_pubSubClient.state());
                delay(2000);
            }
        }
    }

    void mqttClient::loop()
    {
        m_pubSubClient.loop();
    }

    void mqttClient::publish(const std::string& Topic, const std::string& payload)
    {
        m_pubSubClient.publish(Topic.c_str(), payload.c_str());
    }

    PubSubClient& mqttClient::getClient()
    {
        return m_pubSubClient;
    }

}       // namespace mqtt