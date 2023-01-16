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

#include "Common/Prolog.hpp"
#include "MyLogger/Logger.hpp"

//#  pragma GCC diagnostic push
//#    pragma GCC diagnostic ignored "-Wfloat-equal"
#      include "SWI-cpp.h"
//#   pragma GCC diagnostic pop

bool test_prolog(int argc, char **argv)
{
    PlEngine e(argv[0]);

    try
    {
        PlCall("consult('/home/qq/MyGitHub/Highway/data/graph.pl').");

        // Get all nodes: node(X).
        {
            PlTermv av(1);
            PlQuery q("node", av);

            std::cout << "Nodes:" << std::endl;
            while (q.next_solution())
            {
                std::cout << "\t" << (char *)av[0] << std::endl;
            }
        }
    }
    catch (PlException &ex)
    {
        LOGE("%s", (char *) ex);
        return false;
    }

    return true;
}