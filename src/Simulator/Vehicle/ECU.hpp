//=====================================================================
// https://github.com/Lecrapouille/Highway
// Highway: Open-source simulator for autonomous driving research.
// Copyright 2021 -- 2023 Quentin Quadrat <lecrapouille@gmail.com>
//
// This file is part of Highway.
//
// Highway is free software: you can redistribute it and/or modify it
// under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful, but
// WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with Highway.  If not, see <http://www.gnu.org/licenses/>.
//=====================================================================

#ifndef ECU_HPP
#  define ECU_HPP

#  include "Simulator/Sensors/Sensor.hpp"
#  include "Common/Components.hpp"

// ****************************************************************************
//! \brief Base class for Electronic Control Units.
//!
//! An Electronic Control Unit (ECU) is an embedded system in automotive
//! electronics that controls one or more of the electrical systems or
//! subsystems in a car or other motor vehicle.
//!
//! You can bind sensors to the ECU with the \c observe method. Sensors shall be
//! first placed on the vehicle shape before being used and they can be shared
//! by several ECUs. Sensors will then notify the ECU with the virtual \c
//! onSensorUpdated method (to be implemented in the concrete class).
//! The ECU performs computation in the virtula \c update method (to be
//! implemented in the concrete class) and notify events (listener).
//!
//! TODO ECU output (like controlling actuators) to be defined.
// ****************************************************************************
class ECU: public Component, public SensorObserver
{
public:

    //-------------------------------------------------------------------------
    //! \brief
    //-------------------------------------------------------------------------
    typedef std::function<void()> Callback;

    //-------------------------------------------------------------------------
    //! \brief
    //-------------------------------------------------------------------------
    class Listener
    {
    public:

        virtual ~Listener() = default;
        virtual void onMessageToLog(std::string const& /*message*/) const {};
    };

    //-------------------------------------------------------------------------
    //! \brief
    //-------------------------------------------------------------------------
    ECU()
    {
        static ECU::Listener dummy_listener;
        setListener(dummy_listener);
    }

    //-------------------------------------------------------------------------
    //! \brief Needed because of pure virtual methods.
    //-------------------------------------------------------------------------
    virtual ~ECU() = default;

    //-------------------------------------------------------------------------
    //! \brief Do computation given the delta time [second] from the previous
    //! call.
    //-------------------------------------------------------------------------
    virtual void update(Second const dt) = 0;

    // -------------------------------------------------------------------------
    //! \brief Attach a listener (or replace the old listener) to the ECU.
    // -------------------------------------------------------------------------
    inline void setListener(ECU::Listener& listener)
    {
        m_listener = &listener;
    }

    //-------------------------------------------------------------------------
    //! \brief
    //-------------------------------------------------------------------------
    template<typename... Args>
    void logMessage(Args&... args) const
    {
        std::stringstream ss;
        ((ss << args), ...);
        m_listener->onMessageToLog(ss.str());
    }

    //-------------------------------------------------------------------------
    //! \brief Register a callback for reacting to the given event.
    //! \param[in] event: the event id to react to.
    //! \param[in] cb: the function to call when the event occured.
    //-------------------------------------------------------------------------
    inline void callback(size_t const event, ECU::Callback&& cb)
    {
        m_callbacks[event] = cb;
    }

    //-------------------------------------------------------------------------
    //! \brief React to the current event.
    //! \param[in] event: the event id to react to.
    //! \return true if the ECU has reacted to an known event, else returns
    //! false.
    //-------------------------------------------------------------------------
    bool reactTo(size_t const event)
    {
        auto it = m_callbacks.find(event);
        if (it != m_callbacks.end())
        {
            it->second();
            return true;
        }
        return false;
    }

public: // Inheritance with \c Component class

    //-------------------------------------------------------------------------
    //! \brief Implement Entity-Component-System to allow adding compositions
    //! dynamically.
    //-------------------------------------------------------------------------
    COMPONENT_CLASSTYPE(ECU, Component);

public: // Inheritance with \c SensorObserver. The virtual void
        // onSensorUpdated(Sensor& sensor) shall be implemented in the derived
        // class.

    //-------------------------------------------------------------------------
    //! \brief Observer pattern. Bind the given \c sensor to get information
    //! from it. A sensor can be used by several ECUs.
    //-------------------------------------------------------------------------
    void observe(Sensor& sensor)
    {
        sensor.attachObserver(*this);
    }

private:

    //! \brief List of reactions to do when events occured.
    std::map<size_t, ECU::Callback> m_callbacks;
    //! \brief ECU event listener.
    ECU::Listener *m_listener = nullptr;
};

#endif
