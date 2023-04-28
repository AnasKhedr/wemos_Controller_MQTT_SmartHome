//! \file helper.hpp
//! \authors Anas Khedr
//! \brief
//! \version 0.1
//! \date 25-May-2021
//! \copyright This work is licensed under a Creative Commons Attribution 4.0 International License. Copyright (c) 2021
//!

#ifndef _HELPTER_HPP
#define _HELPTER_HPP

//---------------------------------------------------------------------------
// Includes
//---------------------------------------------------------------------------
#include "RemoteDebug.h" // https://github.com/JoaoLopesF/RemoteDebug --> V2.1.2
#include "common.hpp"
#include <Arduino.h>
#include <PubSubClient.h>
#include <limits> // for numeric_limits
#include <string>

static RemoteDebug Debug;

#define _PRINTF(f_, ...) \
    Serial.printf((f_), __VA_ARGS__); \
    Debug.printf((f_), __VA_ARGS__); \
    Debug.handle();
#define _PRINTLN(f_) \
    Serial.println((f_)); \
    Debug.println((f_)); \
    Debug.handle();
#define _PRINT(f_) \
    Serial.print((f_)); \
    Debug.print((f_)); \
    Debug.handle();

namespace helper
{
//---------------------------------------------------------------------------
//! \brief for an controlling the devices (relay module)
//!
enum actions : uint8_t
{
    TURN_OFF = 0, /*!< turn off the device */
    TURN_ON = 1, /*!< turn on the device */
    TOGGLE = 2, /*!< turn on the device if it's off and turn it off if it's on */
    // COUNT = 3,      /*!< the number of elements in this enum */
    UNKOWN = std::numeric_limits<uint8_t>::max()
};

enum state : bool
{
    OFF = 0, /*!< if the device state is on*/
    ON = 1 /*!< if the device state is off*/
};
// std::function<void (char* , byte* , unsigned int)> mqttCallback;

// std::pair<std::string, double> mqttCallback(char *topic, byte *payload, unsigned int length)

std::string toString(const int& mqttFailedState);

void printMqttMessage(const std::string& topic, const std::string message);

//---------------------------------------------------------------------------
//! \brief convert C temperature to a nymea friendly value that would be
//! understood by the generic temperature thing.
//! \param temperature  -- the temperature in Celsius
//! \return float       -- the MQTT value that would be understood by Nymea
//!
float converTempToNymea(float temperature);

} // namespace helper

#endif //_HELPTER_HPP
