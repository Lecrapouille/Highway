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

#include "Core/Simulator/BluePrints.hpp"
#include "Core/Simulator/Vehicle/BluePrint.hpp"
#include "nlohmann/json.hpp"
#include <map>

template<class BLUEPRINT>
std::unordered_map<std::string, std::map<std::string, BLUEPRINT>> BluePrints::sm_items;

std::vector<std::function<void()>> BluePrints::m_clear_functions;

//------------------------------------------------------------------------------
std::string BluePrints::load(fs::path path)
{
    std::stringstream error;

    // Check if file exists
    std::ifstream file(path);
    if (!file)
    {
        error << "Failed opening '" << path << "'. Reason was '"
              << strerror(errno) << "'" << std::endl;
        return error.str();
    }

    // Load the JSON content into dictionaries
    nlohmann::json json;
    try
    {
        file >> json;
    }
    catch (std::exception const& e)
    {
        error << "Failed parsing '" << path << "'. Reason was '"
              << e.what() << "'" << std::endl;
        return error.str();
    }

    // Check correct blueprint file.
    if (!(json.contains("type") && json.contains("revision") &&
          std::string(json["type"]) == "blueprints" &&
          json["revision"] == 1))
    {
        error << "Failed parsing '" << path << "'. Reason was '"
              << "'not a valid blueprint file'" << std::endl;
        return error.str();
    }

    // Parse vehicle blueprints
    if (!json.contains("vehicles"))
    {
        error << "Failed parsing '" << path << "'. Reason was '"
              << "Missing JSON 'vehicles' field'" << std::endl;
        return error.str();
    }

    BluePrints::init<vehicle::BluePrint>({});
    std::map<std::string, vehicle::BluePrint> vehicle_blueprints;
    for (nlohmann::json const& it: json["vehicles"])
    {
        BluePrints::add<vehicle::BluePrint>(
            std::string(it["mark"]).c_str(),
            {
                Meter(it["length_m"]),
                Meter(it["width_m"]),
                Meter(it["wheelbase_m"]),
                Meter(it["back_overhang_m"]),
                Meter(it["wheels"]["radius_m"]),
                Meter(it["turning_diameter_m"])
            });
    }

    return {};
}