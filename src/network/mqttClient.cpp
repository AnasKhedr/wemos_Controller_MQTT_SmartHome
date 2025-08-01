//! \file mqttClient.cpp
//! \authors Anas Khedr
//! \brief
//! \version 0.1
//! \date 25-May-2021
//! \copyright This work is licensed under a Creative Commons Attribution 4.0 International License. Copyright (c) 2021
//!

//---------------------------------------------------------------------------
// Includes
//---------------------------------------------------------------------------
#include "network/mqttClient.hpp"

//---------------------------------------------------------------------------
// Functions
//---------------------------------------------------------------------------

namespace mqtt
{
mqttClient::mqttClient(std::string ip, std::string subscriptionTopic, callbackType callback)
    : m_pubSubClient(ip.c_str(), MQTTPORT, callback, m_wifiClient), m_brokerIp(ip), isBrokerInitialized(false),
      m_subscriptionTopic(subscriptionTopic)
{
    // connect(ip,topic);
}

bool mqttClient::init()
{
    // generate random clientID string
    String clientId = "BathroomClient-";
    clientId += String(random(0xffff), HEX);

    Serial.print("[mqttClient] Generated random client ID: ");
    Serial.println(clientId.c_str());
    // Serial.printf(" to connect to broker: %s\n", ip);

    if (!m_pubSubClient.connected())
    {
        Serial.printf("[mqttClient] Connecting to MQTT broker: %s on port: %d\n", m_brokerIp.c_str(), MQTTPORT);

        if (m_pubSubClient.connect(clientId.c_str()))
        {
            // if(m_pubSubClient.subscribe(m_subscriptionTopic.c_str()))
            Serial.printf("connected to %s, subscribing.\n", clientId.c_str());
            if (m_pubSubClient.subscribe(m_subscriptionTopic.c_str()))
            {
                // Serial.printf("[mqttClient] connected to %s and subscribed to topic %s.\n",m_brokerIp.c_str(),
                //                 m_subscriptionTopic.c_str());
                isBrokerInitialized = true;
                return true;
            }
            else
            {
                // Serial.printf("[mqttClient] connected to %s but failed subscribed to topic %s.\n",m_brokerIp.c_str(),
                //                 m_subscriptionTopic.c_str());
                return false;
            }
        }
        else
        {
            Serial.printf("[mqttClient] failed to connect to %s port %s with state: %s\n", m_brokerIp.c_str(), MQTTPORT,
                          helper::toString(m_pubSubClient.state()).c_str());
            return false;
            // delay(2000);
        }
    }
    return false;
}

void mqttClient::loop()
{
    m_pubSubClient.loop();
}

void mqttClient::publish(const std::string& Topic, const std::string& payload)
{
    // try to initialize the broker if it was no initialized successfully before
    if (!isBrokerInitialized)
    {
        Serial.printf("[mqttClient] attempting to initialize %s port %s before publising.\n", m_brokerIp.c_str(), MQTTPORT);
        init();
    }

    m_pubSubClient.publish(Topic.c_str(), payload.c_str());
}

template<class T> void mqttClient::publish(const std::string& Topic, const T& payload)
{
    publish(Topic, std::to_string(payload));
}

PubSubClient& mqttClient::getClient()
{
    return m_pubSubClient;
}

void mqttClient::publishState(const std::string& topic, const std::string& payload)
{
    Serial.printf("Publishing state, topic: %s, payload: %s.\n", topic.c_str(), payload.c_str());
    m_pubSubClient.publish(topic.c_str(), payload.c_str());
}

void mqttClient::publishState(const std::string& topic, const helper::actions& action)
{
    Serial.printf("Publishing state, topic: %s, payload: %d\n", topic.c_str(), static_cast<int>(action));
    m_pubSubClient.publish(topic.c_str(), std::to_string(static_cast<int>(action)).c_str());
}

void mqttCallback(char* topic, byte* payload, unsigned int length)
{
    Serial.print("Message arrived [");
    Serial.print(topic);
    Serial.print("] ");
    std::string message(reinterpret_cast<char*>(payload), length);
    Serial.print(message.c_str());
    Serial.println("\n--------------");
}

} // namespace mqtt