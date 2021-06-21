//---------------------------------------------------------------------------
//! \file helper.cpp
//! \authors Anas Khedr
//! \brief file containing helper functions
//! \version 0.1
//! \date 25-May-2021
//! \copyright This work is licensed under a Creative Commons Attribution 4.0 International License. Copyright (c) 2021
//!


//---------------------------------------------------------------------------
// Includes
//---------------------------------------------------------------------------
#include <random>
#include <string>
#include "helper.hpp"


namespace helper
{

    std::string toString(int mqttFailedState)
    {
        switch (mqttFailedState)
        {
        case MQTT_CONNECTION_TIMEOUT:
            return "MQTT_CONNECTION_TIMEOUT";
            break;
        case MQTT_CONNECTION_LOST:
            return "MQTT_CONNECTION_LOST";
            break;
        case MQTT_CONNECT_FAILED:
            return "MQTT_CONNECT_FAILED";
            break;
        case MQTT_DISCONNECTED:
            return "MQTT_DISCONNECTED";
            break;
        case MQTT_CONNECTED:
            return "MQTT_CONNECTED";
            break;
        case MQTT_CONNECT_BAD_PROTOCOL:
            return "MQTT_CONNECT_BAD_PROTOCOL";
            break;
        case MQTT_CONNECT_BAD_CLIENT_ID:
            return "MQTT_CONNECTION_TIMEOUT";
            break;
        case MQTT_CONNECT_UNAVAILABLE:
            return "MQTT_CONNECT_UNAVAILABLE";
            break;
        case MQTT_CONNECT_BAD_CREDENTIALS:
            return "MQTT_CONNECT_BAD_CREDENTIALS";
            break;
        case MQTT_CONNECT_UNAUTHORIZED:
            return "MQTT_CONNECT_UNAUTHORIZED";
            break;

        default:
            return "Unknow error(you shouldnot see this)!";
        }
    }

    void printMqttMessage(const std::string& topic, const std::string message)
    {
        Serial.print("Message arrived [");
        Serial.print(topic.c_str());
        Serial.print("] ");
        Serial.print(message.c_str());
        Serial.println("\n--------------");
    }

    // class logger
    // {
    //     std::string operation<<()
    // };

}   // namespace helper
