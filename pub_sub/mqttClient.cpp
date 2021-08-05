//---------------------------------------------------------------------------
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
#include "mqttClient.hpp"


//---------------------------------------------------------------------------
// Functions
//---------------------------------------------------------------------------

namespace mqtt
{
    mqttClient::mqttClient(std::string ip, std::string subscriptionTopic, callbackType callback) :
        m_pubSubClient(ip.c_str(), MQTTPORT, callback, m_wifiClient), m_brokerIp(ip), isBrokerInitialized(false)
        ,m_subscriptionTopic(subscriptionTopic)
        ,m_lastReconnectAttempt(0)
        ,m_clientId("ESP8266Client-Office")
    {
        // m_pubSubClient.setBufferSize(512);
        // m_pubSubClient.setKeepAlive(180);
        // m_pubSubClient.connect(m_clientId.c_str());
    }
    // mqttClient::mqttClient(std::string ip, std::string subscriptionTopic, callbackType callback) :
    //     m_pubSubClient(m_wifiClient), m_brokerIp(ip)
    // {
    //     m_pubSubClient.setServer(m_brokerIp.c_str(), MQTTPORT);
    //     m_pubSubClient.setCallback(callback);
    //     // connect(ip,topic);
    //     // isBrokerInitialized=false;
    //     // m_subscriptionTopic = subscriptionTopic;
    // }

    bool mqttClient::init()
    {
        m_pubSubClient.setBufferSize(512);
        m_pubSubClient.setKeepAlive(10);
        //generate random clientID string
        String clientId = "ESP8266Client-Office";
        // clientId += String(random(0xffff), HEX);

        Serial.print("[mqttClient] client ID: ");
        Serial.println(clientId.c_str());
        // Serial.printf(" to connect to broker: %s\n", ip);

        if (!m_pubSubClient.connected())
        {
            Serial.printf("[mqttClient no connected] Connecting to MQTT broker: %s on port: %d\n", m_brokerIp.c_str(), MQTTPORT);

            yield();
            if (m_pubSubClient.connect(clientId.c_str()))
            {
                yield();
                Serial.printf("connected to %s, subscribing.\n", clientId.c_str());
                m_pubSubClient.publish("/home/office/", "connected");
                // if(m_pubSubClient.subscribe("#"))
                if(m_pubSubClient.subscribe(m_subscriptionTopic.c_str()))
                {
                    yield();
                    Serial.printf("[mqttClient] connected to %s and subscribed to topic %s.\n",m_brokerIp.c_str(),
                                    m_subscriptionTopic.c_str());
                    isBrokerInitialized = true;
                    return true;
                }
                else
                {
                    yield();
                    Serial.printf("[mqttClient] connected to %s but failed subscribed to topic %s.\n",m_brokerIp.c_str(),
                                    m_subscriptionTopic.c_str());
                    return false;
                }
            }
            else
            {
                yield();
                // Serial.print("[mqttClient] failed to connect to port  with state:\n");
                Serial.print("failed, rc= ");
                Serial.println(m_pubSubClient.state());
                ///ERROR: this line casues the crash.
                // Serial.printf("[mqttClient] failed to connect to %s port %s with state: %s\n", m_brokerIp.c_str(),
                //                     MQTTPORT, helper::toString(m_pubSubClient.state()));
                return false;
                // delay(2000);
            }
        }
        return false;
    }

    void mqttClient::loop()
    {
        // if the ESP was disconnected, then attempt to reconnect.
        if (!m_pubSubClient.connected())
        {
            // Serial.printf("connection state to: %s  is: %d\n", m_brokerIp.c_str(), m_pubSubClient.connected());
            static uint8_t numberOfRetries = 0;
            unsigned long now = millis();

            // if 2 seconds have passed since the last attempt
            if ((now - m_lastReconnectAttempt) > 2000)
            {
                Serial.println("Attempting to reconnect to broker.");
                numberOfRetries++;
                m_lastReconnectAttempt = now;
                // Attempt to reconnect
                if (init())
                {
                    Serial.println("Reconnected successfully.");
                    m_lastReconnectAttempt = 0;
                    numberOfRetries = 0;
                }
                else
                {
                    if(numberOfRetries > MQTTLOOPERCONNECTRETRIES)
                    {
                        Serial.printf("Max retries: %d reached for reconnection with %s, resetting ....\n",
                                        MQTTLOOPERCONNECTRETRIES, m_brokerIp.c_str());
                        ESP.restart();
                    }
                    else
                    {
                        Serial.printf("Failed to reconnect retry #: %d, retrying again in 2 seconds.\n", numberOfRetries);
                    }
                }
            }
        }
        else
        {
            // Client is already connected
            m_pubSubClient.loop();
        }
    }

    void mqttClient::publish(const std::string& Topic, const std::string& payload)
    {
        // try to initialize the broker if it was no initialized successfully before
        if(!isBrokerInitialized)
        {
            Serial.printf("[mqttClient] attempting to initialize %s port %s before publising.\n",
                            m_brokerIp.c_str(), MQTTPORT);
            init();
        }

        m_pubSubClient.publish(Topic.c_str(), payload.c_str());
    }

    template<class T>
    void mqttClient::publish(const std::string& Topic, const T& payload)
    {
        publish(Topic, std::to_string(payload));
    }

    PubSubClient& mqttClient::getClient()
    {
        return m_pubSubClient;
    }

    void mqttClient::publishState(const std::string& topic, const std::string& payload)
    {
        Serial.printf("Publishing state, topic: %s, payload: %s.\n",topic.c_str() , payload.c_str());
        m_pubSubClient.publish(topic.c_str(), payload.c_str());
    }

    void mqttClient::publishState(const std::string& topic, const helper::actions& action)
    {
        Serial.printf("Publishing state, topic: %s, payload: %d\n",topic.c_str() , static_cast<int>(action));
        m_pubSubClient.publish(topic.c_str(), std::to_string(static_cast<int>(action)).c_str());
    }

    void mqttCallback(char *topic, byte *payload, unsigned int length)
    {
        Serial.print("Message arrived [");
        Serial.print(topic);
        Serial.print("] ");
        std::string message(reinterpret_cast<char*>(payload), length);
        Serial.print(message.c_str());
        Serial.println("\n--------------");
    }

}       // namespace mqtt