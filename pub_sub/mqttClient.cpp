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
        m_brokerIp(ip), isBrokerInitialized(false)
        ,m_subscriptionTopic(subscriptionTopic)
        ,m_callback(callback)
        ,m_lastReconnectAttempt(0)
    {
        // m_pubSubClient.setServer(m_brokerIp.c_str(), MQTTPORT);
        m_mqttClient.onConnect([this](bool sessionPresent){onMqttConnect(sessionPresent);});
        m_mqttClient.onDisconnect([this](AsyncMqttClientDisconnectReason reason){onMqttDisconnect(reason);});
        m_mqttClient.onMessage([this](char* topic, char* payload, AsyncMqttClientMessageProperties properties, size_t len, size_t index, size_t total)
        {
            // onMqttMessage(topic, payload, properties, len, index, total);
            m_callback(topic, payload, len);
        });

        m_mqttClient.setClientId(CLIENTID);
        m_mqttClient.setServer(m_brokerIp.c_str(), MQTTPORT);

        // m_mqttClient.onDisconnect(onMqttDisconnect);
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
        Serial.printf("[mqttClient] client ID: %s\n", CLIENTID);

        if (!m_mqttClient.connected())
        {
            Serial.printf("[mqttClient] Connecting to MQTT broker: %s on port: %d\n", m_brokerIp.c_str(), MQTTPORT);

            yield();
            m_mqttClient.connect();
            if (m_mqttClient.connected())
            {
                yield();
                Serial.printf("connected to %s, subscribing.\n", CLIENTID);
                // m_pubSubClient.publish("/home/bathroom/", "connected");
                // // if(m_pubSubClient.subscribe("#"))
                // if(m_pubSubClient.subscribe(m_subscriptionTopic.c_str()))
                // {
                //     yield();
                //     Serial.printf("[mqttClient] connected to %s and subscribed to topic %s.\n",m_brokerIp.c_str(),
                //                     m_subscriptionTopic.c_str());
                //     isBrokerInitialized = true;
                //     return true;
                // }
                // else
                // {
                //     yield();
                //     Serial.printf("[mqttClient] connected to %s but failed subscribed to topic %s.\n",m_brokerIp.c_str(),
                //                     m_subscriptionTopic.c_str());
                //     return false;
                // }
            }
            else
            {
                yield();
                Serial.print("[mqttClient] failed to connect to port  with state:\n");
                m_mqttClient.connect();
                // Serial.print("failed, rc= ");
                // Serial.println(m_pubSubClient.state());
                // ///ERROR: this line casues the crash.
                // // Serial.printf("[mqttClient] failed to connect to %s port %s with state: %s\n", m_brokerIp.c_str(),
                // //                     MQTTPORT, helper::toString(m_pubSubClient.state()));
                // return false;
                // // delay(2000);
            }
        }
        return false;
    }

    bool mqttClient::loop()
    {
        bool returnValue = false;

        // if the ESP was disconnected, then attempt to reconnect.
        if (!m_mqttClient.connected())
        {
            // Serial.printf("connection state to: %s  is: %d\n", m_brokerIp.c_str(), m_pubSubClient.connected());
            static uint8_t numberOfRetries = 0;
            unsigned long now = millis();

            // if 5 seconds have passed since the last attempt
            if ((now - m_lastReconnectAttempt) > MQTTRECONNECTIONINTERVALS)
            {
                Serial.printf("Attempting to reconnect to broker: %s\n", m_brokerIp.c_str());
                numberOfRetries++;
                m_lastReconnectAttempt = now;
                // Attempt to reconnect
                if (reconnect())
                {
                    returnValue = true;
                    Serial.println("Reconnected successfully.");
                    m_lastReconnectAttempt = 0;
                    numberOfRetries = 0;
                }
                else
                {
                    returnValue = false;
                    if(numberOfRetries > MQTTLOOPERCONNECTRETRIES)
                    {
                        Serial.printf("Max retries: %d reached for reconnection with %s, resetting ....\n",
                                        MQTTLOOPERCONNECTRETRIES, m_brokerIp.c_str());
                        ESP.restart();
                    }
                    else
                    {
                        now = millis();
                        Serial.printf("Failed to reconnect retry #: %d, retrying again in %d seconds.\n", numberOfRetries, MQTTRECONNECTIONINTERVALS/1000);
                    }
                }
            }
        }
        else
        {
            returnValue = true;
            // Client is already connected
        }

        return returnValue;
    }

    void mqttClient::publish(const std::string& Topic, const std::string& payload)
    {
        m_mqttClient.publish(Topic.c_str(), 0, false, payload.c_str());
    }

    template<class T>
    void mqttClient::publish(const std::string& Topic, const T& payload)
    {
        if(m_mqttClient.connected())
        {
            publish(Topic, 0, false, std::to_string(payload));
        }
        else
        {
            Serial.printf("Broker: %s not connected, skipping.\n", m_brokerIp.c_str());
        }
    }

    AsyncMqttClient& mqttClient::getClient()
    {
        return m_mqttClient;
    }

    void mqttClient::publishState(const std::string& topic, const std::string& payload)
    {
        Serial.printf("Publishing state, topic: %s, payload: %s.\n",topic.c_str() , payload.c_str());
        if(m_mqttClient.connected())
        {
            m_mqttClient.publish(topic.c_str(), 0, false, payload.c_str());
        }
        else
        {
            Serial.printf("Broker: %s not connected, skipping.\n", m_brokerIp.c_str());
        }
    }

    void mqttClient::publishState(const std::string& topic, const bool& state)
    {
        if(m_mqttClient.connected())
        {
            Serial.printf("Publishing state, topic: %s, payload: %d\n",topic.c_str() , state);
            m_mqttClient.publish(topic.c_str(), 0, false, std::to_string(state).c_str());
        }
        else
        {
            Serial.printf("Broker: %s not connected, skipping.\n", m_brokerIp.c_str());
        }
    }

    bool mqttClient::reconnect()
    {
        Serial.println("attempting to re-connect.");
        m_mqttClient.connect();

        return m_mqttClient.connected();
    }

    void mqttClient::onMqttDisconnect(AsyncMqttClientDisconnectReason reason)
    {
        Serial.println("Disconnected from MQTT.");
        m_isConnected = false;
    }

    void mqttClient::onMqttConnect(bool sessionPresent)
    {
        m_isConnected = true;

        Serial.println("Connected to MQTT.");
        Serial.print("Session present: ");
        Serial.println(sessionPresent);
        uint16_t packetIdSub = m_mqttClient.subscribe(m_subscriptionTopic.c_str(), 0);
        Serial.print("Subscribing at QoS 0, packetId: ");
        Serial.println(packetIdSub);
        m_mqttClient.publish("/home/bathroom/", 0, true, "connected");
    }

    void mqttClient::onMqttMessage(char* topic, char* payload, AsyncMqttClientMessageProperties properties, size_t len, size_t index, size_t total)
    {
        Serial.println("Publish received.");
        Serial.print("  topic: ");
        Serial.println(topic);
        Serial.print("  qos: ");
        Serial.println(properties.qos);
        Serial.print("  dup: ");
        Serial.println(properties.dup);
        Serial.print("  retain: ");
        Serial.println(properties.retain);
        Serial.print("  len: ");
        Serial.println(len);
        Serial.print("  index: ");
        Serial.println(index);
        Serial.print("  total: ");
        Serial.println(total);
        std::string message(payload,len);
        Serial.printf("  payload: %s\n", message.c_str());
    }

    void mqttCallback(char *topic, char *payload, size_t length)
    {
        Serial.print("Message arrived [");
        Serial.print(topic);
        Serial.print("] ");
        std::string message(payload, length);
        Serial.print(message.c_str());
        Serial.println("\n--------------");
    }

}       // namespace mqtt