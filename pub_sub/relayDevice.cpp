//---------------------------------------------------------------------------
//! \file relayDevice.cpp
//! \authors Anas Khedr
//! \brief 
//! \version 0.1
//! \date 24-May-2021
//! \copyright This work is licensed under a Creative Commons Attribution 4.0 International License. Copyright (c) 2021

//---------------------------------------------------------------------------
// Includes
//---------------------------------------------------------------------------
#include "relayDevice.hpp"



//---------------------------------------------------------------------------
//! \brief vector containing all the GPIO already in use
//! \note  this is not a thread safe implementation but the vector will only
//! be used when creating the object so it's ok to leave it this way.
//!
static std::vector<uint8_t> alreadyUsedGPIOs;

//---------------------------------------------------------------------------
// Functions
//---------------------------------------------------------------------------
namespace bathRoom
{

    bathRoomGPIO::bathRoomGPIO(const uint8_t GPIOPin, const GPIOtype type, const uint8_t internalResistor) :
        m_GPIOPin(GPIOPin),
        m_type(type)
    {
        // verify if we can use this pin or not
        if(canUsePin(GPIOPin))
        {
            //inserting GPIO into used list
            alreadyUsedGPIOs.push_back(GPIOPin);
        }
        else
        {
            ///TODO: Print serial error message
            //maybe static_assert
            return;
        }


        // if the device type is control then we need to control it via GPIO output pin
        if(m_type == GPIOtype::control)
        {
            pinMode(m_GPIOPin, OUTPUT);
        }
        // else if the device type is read then we need to get data from it via GPIO input pin
        else
        {
            pinMode(m_GPIOPin, internalResistor);
        }
    }

    bool bathRoomGPIO::canUsePin(const uint8_t& newPin) const
    {
        for(auto pin : alreadyUsedGPIOs)
        {
            if(newPin == pin)
            {
                // pin is already being used so you can't use it
                return false;
            }
        }

        // pin is not used so you can use it
        return true;
    }

    void bathRoomGPIO::switchOn()
    {
        m_state = true;
        digitalWrite(m_GPIOPin, m_state);
    }

    void bathRoomGPIO::switchOff()
    {
        m_state = false;
        digitalWrite(m_GPIOPin, m_state);
    }

    void bathRoomGPIO::toggel()
    {
        Serial.printf("Toggeling pin %d from state %d to %d\n",m_GPIOPin,m_state,!m_state);
        m_state = !m_state;
        digitalWrite(m_GPIOPin, m_state);
    }

} // namespace bathRoom