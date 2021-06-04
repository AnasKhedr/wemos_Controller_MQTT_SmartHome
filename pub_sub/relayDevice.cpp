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


// static const uint8_t D0   = 16;
// static const uint8_t D1   = 5;
// static const uint8_t D2   = 4;
// static const uint8_t D3   = 0;
// static const uint8_t D4   = 2;
// static const uint8_t D5   = 14;
// static const uint8_t D6   = 12;
// static const uint8_t D7   = 13;
// static const uint8_t D8   = 15;
// static const uint8_t D9   = 3;
// static const uint8_t D10  = 1;


//---------------------------------------------------------------------------
//! \brief vector containing all the GPIO already in use
//! \note  this is not a thread safe implementation but the vector will only
//! be used when creating the object so it's ok to leave it this way.
//!
static std::vector<uint8_t> alreadyUsedGPIOs;

//---------------------------------------------------------------------------
//! \brief a list of all the valid pins on the ESP board.
//!
static std::vector<uint8_t> validPins{D0,D1,D2,D3,D4,D5,D6,D7,D8,D9,D10};

//---------------------------------------------------------------------------
// Functions
//---------------------------------------------------------------------------
namespace bathRoom
{

    bathRoomGPIO::bathRoomGPIO(const uint8_t GPIOPin, const GPIOtype type, const std::string handle, const uint8_t internalResistor) :
        m_GPIOPin(GPIOPin),
        m_type(type)
        // m_handle(handle)     // m_handle is not a member, must be initialized in its own class
    {
        // initializing handler
        m_handle = handle;
        m_state = true;        // specific for my need, GPIO device is active low

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

        Serial.println("start of bathRoomGPIO constructor  ********************");
        Serial.printf("gpio pin: %d, m_type: %d, handle: %s\n",GPIOPin,bool(type),handle.c_str());


        // if the device type is control then we need to control it via GPIO output pin
        if(m_type == GPIOtype::control)
        {
            Serial.println("GPIOtype::control -------------------");
            pinMode(m_GPIOPin, OUTPUT);
        }
        // else if the device type is read then we need to get data from it via GPIO input pin
        else
        {
            Serial.println("else +++++++++++++++++++");
            pinMode(m_GPIOPin, internalResistor);
        }
        Serial.println("end of bathRoomGPIO constructor ===================");
    }

    bool bathRoomGPIO::canUsePin(const uint8_t& newPin) const
    {
        // make sure that the pin is not already used by another object
        for(auto pin : alreadyUsedGPIOs)
        {
            if(newPin == pin)
            {
                Serial.printf("Pin number: %d is already used!\n", newPin);
                // pin is already being used so you can't use it
                return false;
            }
        }

        // make sure pin number is valid because if it's not then you'll
        // keep getting a crash from the device and you wont know why!!!
        for(auto oneValidPin : validPins)
        {
            if(oneValidPin == newPin)
            {
                Serial.printf("Pin number: %d is not valid!\n", newPin);
                // the pin was found.
                return true;
            }
        }

        Serial.printf("Pin number: %d can be used.\n", newPin);
        // pin is not used but it was invalid
        return false;
    }

    void bathRoomGPIO::switchOn()
    {
        m_state = true;
        Serial.printf("setting GPIO: %d[%s] state to: %d\n", m_GPIOPin, m_handle.c_str(), m_state);
        digitalWrite(m_GPIOPin, m_state);
    }

    void bathRoomGPIO::switchOff()
    {
        m_state = false;
        Serial.printf("setting GPIO: %d[%s] state to: %d\n", m_GPIOPin, m_handle.c_str(), m_state);
        digitalWrite(m_GPIOPin, m_state);
    }

    void bathRoomGPIO::toggel()
    {
        Serial.printf("Toggeling pin %d[%s] from state %d to %d\n",
                        m_GPIOPin, m_handle.c_str(), m_state, !m_state);
        m_state = !(m_state);
        digitalWrite(m_GPIOPin, m_state);
    }

    void bathRoomGPIO::act(const helper::actions& action)
    {
        switch (action)
        {
        case helper::actions::TURN_ON :
            switchOn();
            break;
        case helper::actions::TURN_OFF :
            switchOff();
            break;
        case helper::actions::TOGGEL :
            toggel();
            break;

        default:
            Serial.println("UNKOWN ACTION, DISCARDING!");
            break;
        }
    }

} // namespace bathRoom