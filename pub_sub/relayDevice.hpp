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
#include <Arduino.h>            //the compiler will faile to regonize pinMode and OUTPUT without Arduino.h
#include "ISimpleDevice.hpp"
#include "helper.hpp"
#include "mqttClient.hpp"


//---------------------------------------------------------------------------
//! \brief
//! @note       : Used Design Patterns:
//!             1- tell don't ask
namespace bathRoom
{
    //---------------------------------------------------------------------------
    //! \brief
    //!
    enum class GPIOtype : bool
    {
        control,    //! <
        read        //!<
    };

    class bathRoomGPIO  : public ISimpleDevice
                        // , public Iactor
    {

    public:

        //---------------------------------------------------------------------------
        //! \brief Construct a new bath Room G P I O object
        //! 
        //! \param GPIOPin
        //! \param type
        //! \param internalResistor
        //!
        bathRoomGPIO(const uint8_t GPIOPin,
                    const GPIOtype type,
                    const std::string topic = "NULL",
                    const uint8_t toggelButton = 0,
                    const uint8_t internalResistor = INPUT_PULLUP);

        //---------------------------------------------------------------------------
        //! \brief Destroy the bath Room G P I O object
        //! 
        //!
        ~bathRoomGPIO() = default;

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

        void act(const helper::actions& action);

        void checkButton(std::vector<std::shared_ptr<mqtt::mqttClient>>& mqttClients);


    private:
        //---------------------------------------------------------------------------
        //! \brief
        //!
        const uint8_t m_GPIOPin;

        //---------------------------------------------------------------------------
        //! \brief 
        //! 
        //!
        const uint8_t m_toggelButton;

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
    };

} // namespace bathRoom
#endif  //_RELAY_HPP