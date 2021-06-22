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

gasSensor::gasSensor(const uint8_t& gpioPin, Adafruit_ADS1115& ads, const uint8_t channel, const uint8_t& pullUpDown) :
    ISimpleSensor(gpioPin, pullUpDown),
    m_ads(ads),
    m_ADSChannel(channel)
{

}

bool gasSensor::readDigitalValue()
{
    return digitalRead(m_sensorPin);
}

float gasSensor::readAnalogValue()
{
    return m_ads.readADC_SingleEnded(m_ADSChannel)*bitToVolt;
}

void gasSensor::setInputAndPullResistor(const uint8_t& gpioPin, const uint8_t& pullUpDown)
{
    m_sensorPin = gpioPin;
    pinMode(gpioPin, pullUpDown);
}