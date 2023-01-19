//=============================================================================
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
//=============================================================================

#ifndef MONITORING_HPP
#  define MONITORING_HPP

#  include "Common/FileSystem.hpp"
#  include <SFML/System/Clock.hpp>
#  include <SFML/System/Time.hpp>
#  include <fstream>
#  include <vector>
#  include <functional>
#  include <string>
#  include <chrono>

// *****************************************************************************
//! \brief Class logging states from several instances. This allows to generate
//! CSV files of the simulation and i.e. to be used in sofware such as Scilab,
//! Simulink, Julia, Gnuplot. For example you can monitor the position and the
//! the speed of several vehicles. Observations will packed as rows.
//! \fixme this class is not thread safe and does not check if tracked states
//! are still available.
// *****************************************************************************
class Monitor
{
private:

    //--------------------------------------------------------------------------
    //! \brief Internal function for logging observations.
    //--------------------------------------------------------------------------
    using Observation = std::function<void(Monitor&)>;

public:

    //--------------------------------------------------------------------------
    //! \brief Create or replace the monitoring file.
    //! \param[in] path the path to the monitoring file we want to create.
    //! \param[in] separator the CSV field separator. Default is ';'
    //! \note Return true if the file has been created successfully, else, no
    //! data will be recored if the file has not been opened correctly.
    //! \note the close() function is automatically called.
    //--------------------------------------------------------------------------
    bool open(fs::path const& path, const char separator = ';')
    {
        m_separator = separator;
        if (m_outfile.is_open())
            this->close();
        m_outfile.open(path);
        return m_outfile.is_open();
    }

    //--------------------------------------------------------------------------
    //! \brief Close the monitoring file. No data will be recoreded.
    //--------------------------------------------------------------------------
    void close()
    {
        m_outfile.close();
        m_observations.clear();
        m_headers.clear();
        m_init = true;
    }

    //--------------------------------------------------------------------------
    //! \brief Check if the monitoring file has been created successfully.
    //--------------------------------------------------------------------------
    operator bool() const { return m_outfile.is_open(); }

    //--------------------------------------------------------------------------
    //! \brief Bind states of an external instance to this current monitor
    //! instance. Shall be paired with \c header() method.
    //! \return Return this instance.
    //--------------------------------------------------------------------------
    template<typename... Args>
    Monitor& observe(Args&... args)
    {
        m_observations.push_back([&](Monitor& m) { m.write(args...); });
        return *this;
    }

    //--------------------------------------------------------------------------
    //! \brief Bind header states for filling the header of the monitoring file.
    //! Shall be paired with \c header() method.
    //! \return Return this instance.
    //--------------------------------------------------------------------------
    template<typename... Args>
    Monitor& header(Args&... args)
    {
        m_headers.push_back([&](Monitor& m) { m.write(args...); });
        return *this;
    }

    //--------------------------------------------------------------------------
    //! \brief Record in the monitoring file all external states as a single
    //! line.
    //! \param[in] elapsed_time Simulation elapsed time.
    //--------------------------------------------------------------------------
    void record(Second const elapsed_time)
    {
        using namespace std::chrono;

        if (!m_outfile.is_open())
            return ;

        if (m_init)
        {
            m_init = false;
            m_outfile << "time [s]";
            for (auto const& fun: m_headers)
            {
                fun(*this);
            }
        }
        else
        {
            m_outfile << elapsed_time.value();
            for (auto const& fun: m_observations)
            {
                fun(*this);
            }
        }
        m_outfile << std::endl;
    }

private:

    //--------------------------------------------------------------------------
    //! \brief Write in the monitoring file like printf().
    //--------------------------------------------------------------------------
    template<typename... Args>
    void write(Args&... args)
    {
        ((m_outfile << m_separator << args), ...);
    }

private:

    //! \brief Store functions that will fill the header of the monitoring file.
    std::vector<Observation> m_headers;
    //! \brief Store functions that will fill the content of the monitoring file.
    std::vector<Observation> m_observations;
    //! \brief The handle of the opened monitoring file.
    std::ofstream m_outfile;
    //! \brief CSV field separator.
    std::string m_separator;
    //! \brief if true then record the header file else record observations.
    bool m_init = true;
};

#endif
