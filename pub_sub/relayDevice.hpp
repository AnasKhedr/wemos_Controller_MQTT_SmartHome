//---------------------------------------------------------------------------
//! \file relayDevice.hpp
//! \authors Anas Khedr
//! \brief
//! \version 0.1
//! \date 24-May-2021
//! \copyright This work is licensed under a Creative Commons Attribution 4.0 International License. Copyright (c) 2021

#ifndef _RELAY_HPP
#define _RELAY_HPP


//---------------------------------------------------------------------------
// Includes
//---------------------------------------------------------------------------
#include <stdint.h>
#include <vector>
#include <optional>             //I'd rather use boot::optional but Ubuntu is a shitty OS
#include <Arduino.h>            //the compiler will faile to regonize pinMode and OUTPUT without Arduino.h
#include "ISimpleDevice.hpp"
#include "helper.hpp"
#include "mqttClient.hpp"


//---------------------------------------------------------------------------
//! \brief
//! @note       : Used Design Patterns:
//!             1- tell don't ask
namespace office
{
    //---------------------------------------------------------------------------
    //! \brief
    //!
    enum class GPIOtype : uint8_t
    {
        activeLow,          //! <   if the device requires digital low(0V) signal to activate
        activeHigh,         //! <   if the device requires digital high signal to activate
        NONE                //! <   if the device is no applicable for control (sensor)
    };

    class officeGPIO  : public ISimpleDevice
    {

    public:

        //---------------------------------------------------------------------------
        //! \brief Construct a new bath Room G P I O object
        //! 
        //! \param controlPin
        //! \param type
        //! \param internalResistor
        //! \todo deprecate \p type
        //!
        officeGPIO(const uint8_t controlPin,
                    const GPIOtype type,
                    const std::string topic = "NULL",
                    const std::optional<uint8_t> toggelButton = std::nullopt,
                    const uint8_t internalResistor = INPUT_PULLUP);

        //---------------------------------------------------------------------------
        //! \brief Destroy the bath Room G P I O object
        //! 
        //!
        ~officeGPIO() = default;

        //---------------------------------------------------------------------------
        //! \brief turn on device functionality implementation
        //!
        virtual void switchOn() override;

        //---------------------------------------------------------------------------
        //! \brief turn off device functionality implementation
        //!
        virtual void switchOff() override;

        //---------------------------------------------------------------------------
        //! \brief toggel device state functionality implementation
        //!
        virtual void toggel() override;

        //---------------------------------------------------------------------------
        //! \brief 
        //! 
        //! \param action
        //!
        void act(const helper::actions& action);

        //---------------------------------------------------------------------------
        //! \brief 
        //! 
        //! \param mqttClients
        //!
        void checkButton(const std::vector<std::shared_ptr<mqtt::mqttClient>>& mqttClients);
        void checkButton();

        //---------------------------------------------------------------------------
        //! \brief Get the Current State GPIO pin if configured as an output
        //!
        //! \return helper::state::on if the device is on
        //! \return helper::state::off if the device is off
        //!
        helper::state whatIsDeviceState() const;


    private:
        //---------------------------------------------------------------------------
        //! \brief
        //!
        const uint8_t m_controlPin;

        //---------------------------------------------------------------------------
        //! \brief 
        //! 
        //!
        const std::optional<uint8_t> m_toggelButton;

        //---------------------------------------------------------------------------
        //! \brief
        //!
        const GPIOtype m_type;

        //---------------------------------------------------------------------------
        //! \brief check if the pin number from the parameter is already used by someone
        //! else or not
        //! \return true    - if pin is already used by someone else
        //! \return false   - if pin is not used and can be used
        //!
        bool canUsePin(const uint8_t& newPin) const;

        //---------------------------------------------------------------------------
        //! \brief a flage denoting when we should consider a button pressed(act on)
        //! true - means you should consider button press as a new press and act
        //! false- means that we already acted on the button press and no further action is needed
        bool m_buttonDoAct;

        unsigned long m_lastDebounceTime;
    };

} // namespace office
#endif  //_RELAY_HPP