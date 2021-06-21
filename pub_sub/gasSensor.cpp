//---------------------------------------------------------------------------
//! \file gasSesnor.cpp
//! \authors Anas Khedr
//! \brief 
//! \version 0.1
//! \date 07-Jun-2021
//! \copyright This work is licensed under a Creative Commons Attribution 4.0 International License. Copyright (c) 2021
//! 


//---------------------------------------------------------------------------
// Includes
//---------------------------------------------------------------------------
#include "gasSensor.hpp"



//---------------------------------------------------------------------------
// Functions
//---------------------------------------------------------------------------

gasSensor::gasSensor(const uint8_t& gpioPin, const uint8_t& pullUpDown, const uint8_t analogPin) :
    ISimpleSensor(gpioPin, pullUpDown),
    m_analogPin(analogPin)
{

}

bool gasSensor::readDigitalValue()
{
    return digitalRead(m_sensorPin);
}

int gasSensor::readAnalogValue()
{
    return analogRead(m_analogPin);
}

void gasSensor::setInputAndPullResistor(const uint8_t& gpioPin, const uint8_t& pullUpDown)
{
    m_sensorPin = gpioPin;
    pinMode(gpioPin, pullUpDown);
}