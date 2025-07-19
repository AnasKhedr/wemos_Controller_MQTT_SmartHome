//! \file ISimpleDevice.hpp
//! \authors Anas Khedr
//! \brief interface class for any simple device
//! \version 0.1
//! \date 24-May-2021
//! \copyright This work is licensed under a Creative Commons Attribution 4.0 International License. Copyright (c) 2021

//---------------------------------------------------------------------------
// Includes
//---------------------------------------------------------------------------
#include <string>

//---------------------------------------------------------------------------
// Functions
//---------------------------------------------------------------------------
class ISimpleDevice
{
    // enum deviceStates : bool
    // {
    //     OFF = 0,
    //     ON  = 1
    // };

public:
    //---------------------------------------------------------------------------
    //! \brief default destructor
    //! nothing to delete
    virtual ~ISimpleDevice() = default;

    //---------------------------------------------------------------------------
    //! \brief pure Virtual function to turn on device functionality
    //! to be implemented by each device
    virtual void switchOn() = 0;

    //---------------------------------------------------------------------------
    //! \brief pure Virtual function to turn of device functionality
    //! to be implemented by each device
    virtual void switchOff() = 0;

    //---------------------------------------------------------------------------
    //! \brief pure Virtual function to toggel state of device between
    //! on and off
    virtual void toggel() = 0;

protected:
    // deviceStates m_state;
    bool m_currentState;

    bool m_lastState;

    //---------------------------------------------------------------------------
    //! \brief handle string representing the device name in the mqtt message
    //! that will be used to control the device.
    //!
    std::string m_handle;
};

// More Fuctions
