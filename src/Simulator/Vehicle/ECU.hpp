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
#  include "Math/Units.hpp"
#  include "Common/Components.hpp"
#  include "Common/Observer.hpp"
#  include "MyLogger/Logger.hpp"

// ****************************************************************************
//! \brief Electronic Control Unit. An ECU is an embedded system in automotive
//! electronics that controls one or more of the electrical systems or
//! subsystems in a car or other motor vehicle.
// ****************************************************************************
class ECU: public Component, public SensorObserver
{
public:

   enum Event { Message };

    //-------------------------------------------------------------------------
    //! \brief
    //-------------------------------------------------------------------------
    typedef std::function<void(std::string const&)> Callback;

    //-------------------------------------------------------------------------
    //! \brief Implement Entity-Component-System to allow adding compositions
    //! dynamically.
    //-------------------------------------------------------------------------
    COMPONENT_CLASSTYPE(ECU, Component);

    //-------------------------------------------------------------------------
    //! \brief Needed because of pure virtual methods.
    //-------------------------------------------------------------------------
    virtual ~ECU() = default;

    //-------------------------------------------------------------------------
    //! \brief Observer pattern. Bind the given \c sensor to get information
    //! from it. A sensor can be used by several ECUs.
    //-------------------------------------------------------------------------
    void observe(Sensor& sensor)
    {
        sensor.attachObserver(*this);
    }

    //-------------------------------------------------------------------------
    //! \brief Register a callback for reacting to SFML press events.
    //-------------------------------------------------------------------------
    inline void callback(size_t const event, Callback&& cb)
    {
        m_callbacks[event] = cb;
    }

    //-------------------------------------------------------------------------
    //! \brief
    //-------------------------------------------------------------------------
    bool reactTo(size_t const event, std::string const& txt)
    {
        auto it = m_callbacks.find(event);
        if (it != m_callbacks.end())
        {
            it->second(txt);
            return true;
        }
        return false;
    }

    //-------------------------------------------------------------------------
    //! \brief Do computation.
    //-------------------------------------------------------------------------
    virtual void update(Second const dt) = 0;

private:

    //! \brief List of reactions to do when events occured
    std::map<size_t, Callback> m_callbacks;
};

#endif
