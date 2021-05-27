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

    // std::pair<std::string, double> mqttCallback(char *topic, byte *payload, unsigned int length)
    void mqttCallback(char *topic, byte *payload, unsigned int length)
    {
        Serial.print("Message arrived [");
        Serial.print(topic);
        Serial.print("] ");
        for (int i = 0; i < length; i++)
        {
            Serial.print((char)payload[i]);
        }
        Serial.println("\n--------------");

        // std::make_pair<std::string, std::string>(std::string(topic), std::string(payload,));
    }


    //source: https://stackoverflow.com/a/47978023/6184259
    std::string random_string()
    {
        std::string str("0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz");

        std::random_device rd;
        std::mt19937 generator(rd());

        std::shuffle(str.begin(), str.end(), generator);

        return str.substr(0, 8);    // assumes 32 < number of characters in str
    }

}   // namespace helper
