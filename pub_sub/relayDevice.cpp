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
#include "Application.hpp"


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

    bathRoomGPIO::bathRoomGPIO(const uint8_t controlPin, const GPIOtype type, const std::string handle, const std::optional<uint8_t> toggelButton, const uint8_t internalResistor) :
        m_controlPin(controlPin),
        m_toggelButton(toggelButton),
        m_type(type),
        m_buttonDoAct(false)
        // m_handle(handle)     // m_handle is not a member, must be initialized in its own class
    {
        _PRINTLN("start of officeGPIO constructor ===================");
        // initializing handler
        m_handle = handle;
        m_currentState = true;        // specific for my need, GPIO device is active low

        // verify if we can use this pin or not
        if(canUsePin(controlPin))
        {
            //inserting GPIO into used list
            alreadyUsedGPIOs.push_back(m_controlPin);
            pinMode(m_controlPin, OUTPUT);
        }
        else
        {
            ///TODO: Print serial error message
            //maybe static_assert
            return;
        }

        _PRINTF("gpio pin: %d, m_type: %d, handle: %s\n", controlPin, bool(type),handle.c_str());

        //default state for device is off
        // switchOff();
        if(m_toggelButton)
        {
            if(canUsePin(m_toggelButton.value()))
            {
                alreadyUsedGPIOs.push_back(m_toggelButton.value());
                _PRINTF("ToggelButton is set to pin: %d\n", m_toggelButton.value());
                pinMode(m_toggelButton.value(), internalResistor);
            }
            else
            {
                _PRINTLN("Error[m_toggelButton], GPIO setup will not complete!!!");
            }
        }

        // Application::m_timerTasks;
        // m_buttonCheck.every(100, [&](void*) -> bool
        // Application::m_timerTasks.every(100, [&](void*) -> bool
        // {
        //     if(digitalRead(m_toggelButton) == LOW)
        //     {
        //         toggel();
        //     }
        //     return true;
        // });

        _PRINTLN("end of bathRoomGPIO constructor ===================");
    }

    bool bathRoomGPIO::canUsePin(const uint8_t& newPin) const
    {
        // make sure that the pin is not already used by another object
        for(auto pin : alreadyUsedGPIOs)
        {
            if(newPin == pin)
            {
                _PRINTF("Pin number: %d is already used!\n", newPin);
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
                _PRINTF("Pin number: %d can be used.\n", newPin);
                // the pin was found.
                return true;
            }
        }

        _PRINTF("Pin number: %d is not valid!\n", newPin);
        // pin is not used but it was invalid
        return false;
    }

    void bathRoomGPIO::switchOn()
    {
        // my devices are active low
        if(m_type == GPIOtype::activeLow)
        {
            m_currentState = false;
        }
        else
        {
            m_currentState = true;
        }
        _PRINTF("[switchOn]setting GPIO: %d[%s] state to: %d\n", m_controlPin, m_handle.c_str(), m_currentState);
        Debug.printf("[switchOn]setting GPIO: %d[%s] state to: %d\n", m_controlPin, m_handle.c_str(), m_currentState);
        digitalWrite(m_controlPin, m_currentState);
    }

    void bathRoomGPIO::switchOff()
    {
        if(m_type == GPIOtype::activeLow)
        {
            m_currentState = true;
        }
        else
        {
            m_currentState = false;
        }
        _PRINTF("[switchOff]setting GPIO: %d[%s] state to: %d\n", m_controlPin, m_handle.c_str(), m_currentState);
        Debug.printf("[switchOff]setting GPIO: %d[%s] state to: %d\n", m_controlPin, m_handle.c_str(), m_currentState);
        digitalWrite(m_controlPin, m_currentState);
    }

    void bathRoomGPIO::toggel()
    {
        _PRINTF("Toggeling pin %d[%s] from state %d to %d\n",
                        m_controlPin, m_handle.c_str(), m_currentState, !m_currentState);
        Debug.printf("Toggeling pin %d[%s] from state %d to %d\n",
                        m_controlPin, m_handle.c_str(), m_currentState, !m_currentState);
        m_currentState = !(m_currentState);
        digitalWrite(m_controlPin, m_currentState);
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
            _PRINTLN("UNKOWN ACTION, DISCARDING!");
            break;
        }
    }

    void bathRoomGPIO::checkButton(const std::vector<std::shared_ptr<mqtt::mqttClient>>& mqttClients)
    {
        // if this GPIO device is was not set for a toggle pin.
        if(!m_toggelButton)
        {
            // _PRINTF("device %s connected to pin: %d, with type m_type: %d, has not toggle pin!!\n",handle.c_str(),controlPin,bool(type));
            return;
        }

        // read the state of the switch into a local variable:
        bool state = digitalRead(m_toggelButton.value());

        // If button is pushed (pulled up)
        if(state == LOW)
        {
            // make sure deboucing time have passsed and we didn't act on the button yet
            if (m_buttonDoAct &&
                ((millis() - m_lastDebounceTime) > BUTTONDEBOUNCINGDELAY_MS))
            {
                m_buttonDoAct = false;

                _PRINTF("Button Pressed(GPIO%d) for %s, toggeling state.", m_toggelButton.value(), m_handle.c_str());
                toggel();

                for(auto& oneClient : mqttClients)
                {
                    if(bathRoom::GPIOtype::activeHigh == m_type)
                    {
                        oneClient->publishState(bathRoomInputCommands+m_handle, m_currentState);
                    }
                    else    //activeLow
                    {
                        // invert the state because 0 means on and 1 means off
                        oneClient->publishState(bathRoomInputCommands+m_handle, !m_currentState);
                    }
                }
            }

        }
        else
        {
            m_lastDebounceTime = millis();
            m_buttonDoAct = true;
        }
    }

    void bathRoomGPIO::checkButton()
    {
        checkButton(std::vector<std::shared_ptr<mqtt::mqttClient>>{});
    }

    helper::state bathRoomGPIO::whatIsDeviceState() const
    {
        if(m_type == GPIOtype::activeLow)
        {
            // if it's active low then that mean the device is on when
            // output low(0)
            return static_cast<helper::state>(!m_currentState);
        }
        else
        {
            return static_cast<helper::state>(m_currentState);
        }
    }

} // namespace bathRoom