// 2021 -- 2022 Quentin Quadrat quentin.quadrat@gmail.com
//
// This is free and unencumbered software released into the public domain.
//
// Anyone is free to copy, modify, publish, use, compile, sell, or
// distribute this software, either in source code form or as a compiled
// binary, for any purpose, commercial or non-commercial, and by any
// means.
//
// In jurisdictions that recognize copyright laws, the author or authors
// of this software dedicate any and all copyright interest in the
// software to the public domain. We make this dedication for the benefit
// of the public at large and to the detriment of our heirs and
// successors. We intend this dedication to be an overt act of
// relinquishment in perpetuity of all present and future rights to this
// software under copyright law.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
// EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
// MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
// IN NO EVENT SHALL THE AUTHORS BE LIABLE FOR ANY CLAIM, DAMAGES OR
// OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,
// ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
// OTHER DEALINGS IN THE SOFTWARE.
//
// For more information, please refer to <https://unlicense.org>

#include "Application/GUISimulation.hpp"

#define WINDOW_WIDTH 1024
#define WINDOW_HEIGHT 1024

//-----------------------------------------------------------------------------
//! \brief simple demo of simulation.
const char* simulation_name()
{
    return "Simple simulation demo";
}

//-----------------------------------------------------------------------------
//! \brief simple demo of simulation.
void react_to(Simulator& simulator, size_t key)
{
    // Allow the ego car to react to callbacks set with Vehicle::callback()
    simulator.ego().reactTo(key);
}

//-----------------------------------------------------------------------------
//! \brief simple demo of simulation.
static Car& customize(Car& car)
{
    // Add reactions from keyboard
    car.callback(sf::Keyboard::PageDown, [&car]()
    {
        //car.turningIndicator(false, m_turning_right ^ true);
    });

    car.callback(sf::Keyboard::PageUp, [&car]()
    {
        //car.turningIndicator(m_turning_left ^ true, false);
    });

    car.callback(sf::Keyboard::Up, [&car]()
    {
        car.refSpeed(1.0f);
    });

    car.callback(sf::Keyboard::Down, [&car]()
    {
        car.refSpeed(0.0f);
    });

    car.callback(sf::Keyboard::Right, [&car]()
    {
        car.refSteering(car.refSteering() - 0.1f);
    });

    car.callback(sf::Keyboard::Left, [&car]()
    {
        car.refSteering(car.refSteering() + 0.1f);
    });

    return car;
}

//-----------------------------------------------------------------------------
//! \brief simple demo of simulation.
static bool halt_simulation_when(Simulator const& simulator)
{
    return false; // Always runs
}

//-----------------------------------------------------------------------------
//! \brief simple demo of simulation.
static Car& create_city(City& city)
{
    //BluePrints::init(); // FIXME a quel endroit ?
    city.reset();

    // Create parallel or perpendicular or diagnoal parking slots
    const int angle = 0u;
    std::string dim = "epi." + std::to_string(angle);
    Parking& parking0 = city.addParking(dim.c_str(), sf::Vector2f(97.5f, 100.0f)); // .attachTo(road1, offset);
    Parking& parking1 = city.addParking(dim.c_str(), parking0.position() + parking0.delta());
    Parking& parking2 = city.addParking(dim.c_str(), parking1.position() + parking1.delta());
    Parking& parking3 = city.addParking(dim.c_str(), parking2.position() + parking2.delta());
    city.addParking(dim.c_str(), parking3.position() + parking3.delta());

    // Add parked cars (static)
    city.addCar("Renault.Twingo", parking0);
    city.addCar("Renault.Twingo", parking1);
    city.addCar("Renault.Twingo", parking3);

    // Self-parking car (dynamic). Always be the last in the container
    return customize(city.addEgo("Renault.Twingo", parking0.position() + sf::Vector2f(0.0f, 5.0f)));
}

//-----------------------------------------------------------------------------
//! \brief Set callbacks implemented in Simulation/Simulation.cpp and needed for
//! creating the simulation.
static void simple_simulation_demo(Simulator& simulator)
{
    Scenario s = {
        .name = simulation_name,
        .create = create_city,
        .halt = halt_simulation_when,
        .react = react_to,
    };

    // FIXME since this is not loaded from .so file not sure there is not side
    // effects.
    simulator.load(s);
}

// -----------------------------------------------------------------------------
int main(int argc, char* argv[])
{
    try
    {
        Application app(WINDOW_WIDTH, WINDOW_HEIGHT, "Auto Parking");
        GUISimulation gui(app);

        // No argument: load an ultra basic simulation
        if (argc == 1)
        {
            simple_simulation_demo(gui.simulator);
        }
        // Single argument: load a shared library file holding functions for custom
        // simulation.
        else
        {
            if (!gui.simulator.load(argv[1]))
            {
                std::cerr << "Fatal: failed loading a simulation file. Aborting ..."
                          << std::endl;
                return EXIT_FAILURE;
            }
        }

        app.push(gui);
        app.loop();
    }
    catch (std::string const& msg)
    {
        std::cerr << "Fatal: " << msg << std::endl;
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
