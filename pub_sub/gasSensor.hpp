//---------------------------------------------------------------------------
//! \file gasSensor.hpp
//! \authors Anas Khedr
//! \brief 
//! \version 0.1
//! \date 07-Jun-2021
//! \copyright This work is licensed under a Creative Commons Attribution 4.0 International License. Copyright (c) 2021
//! 

#ifndef _GASSENSOR_HPP
#define _GASSENSOR_HPP

//---------------------------------------------------------------------------
// Includes
//---------------------------------------------------------------------------
#include "ISimpleSensor.hpp"


class gasSensor : ISimpleSensor
{
    public:
        gasSensor(const uint8_t& gpioPin, const uint8_t& pullUpDown = INPUT_PULLUP, const uint8_t analogPin = A0);

        //---------------------------------------------------------------------------
        //! \brief Set the Input And Pull Resistor object
        //! 
        //! \param gpioPin
        //! \param pullUpDown
        //! \warning  you should not use this function if you'll use the custom
        //! constructor, but must use it if you'll use the deault constructor.
        //!
        virtual void setInputAndPullResistor(const uint8_t& gpioPin, const uint8_t& pullUpDown) override;

        //---------------------------------------------------------------------------
        //! \brief 
        //! 
        //! \return true
        //! \return false
        //!
        virtual bool readDigitalValue() override;

        //---------------------------------------------------------------------------
        //! \brief 
        //! 
        //! \return int
        //!
        virtual int readAnalogValue() override;

    private:
        uint8_t m_analogPin;
        // Data members
};


#endif  // _GASSENSOR_HPP