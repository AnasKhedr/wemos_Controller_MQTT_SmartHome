//---------------------------------------------------------------------------
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


class ISimpleSensor
{
    public:
        ISimpleSensor(const uint8_t& gpioPin, const uint8_t& pullUpDown) {pinMode(m_sensorPin=gpioPin, pullUpDown);};

        ISimpleSensor() = default;

        virtual ~ISimpleSensor() = default;

        virtual void setInputAndPullResistor(const uint8_t& gpioPin, const uint8_t& pullUpDown) = 0;

        virtual bool readDigitalValue() = 0;

        virtual int readAnalogValue() = 0;

        // Data members

    protected:
        // Data members
        uint8_t m_sensorPin;
};

