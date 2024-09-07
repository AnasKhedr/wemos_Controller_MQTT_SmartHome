//! \file ISimpleSensor.hpp
//! \authors Anas Khedr
//! \brief
//! \version 0.1
//! \date 07-Jun-2021
//! \copyright This work is licensed under a Creative Commons Attribution 4.0 International License. Copyright (c) 2021
//!

//---------------------------------------------------------------------------
// Includes
//---------------------------------------------------------------------------
#include <Arduino.h>
#include <optional>

#ifndef _ISIMPLESENSOR_HPP
#define _ISIMPLESENSOR_HPP

class ISimpleSensor
{
public:
    ISimpleSensor(std::optional<uint8_t> gpioPin, const uint8_t& pullUpDown)
    {
        m_sensorPin = gpioPin;
        if (gpioPin)
        {
            pinMode(gpioPin.value(), pullUpDown);
        }
    };

    ISimpleSensor() = default;

    virtual ~ISimpleSensor() = default;

    virtual void setInputAndPullResistor(const uint8_t& gpioPin, const uint8_t& pullUpDown) = 0;

    virtual bool readDigitalValue() = 0;

    virtual float readAnalogValue() = 0;

    // Data members

protected:
    // Data members
    std::optional<uint8_t> m_sensorPin;
};

#endif //_ISIMPLESENSOR_HPP