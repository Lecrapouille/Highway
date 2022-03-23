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

#include "main.hpp"
#include "Application/GUISimulation.hpp"

#define WINDOW_WIDTH 1024
#define WINDOW_HEIGHT 1024

//------------------------------------------------------------------------------
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-variable"
void create_world(City& city, size_t const angle, bool const /*parked*/)
{
    city.reset();

    // Create a road
    // Road& road1 = addRoad(RoadDimensions::get("road.2ways"), curvature, length);

    // Create parallel or perpendicular or diagnoal parking slots
    std::string dim = "epi." + std::to_string(angle);
    Parking& parking0 = city.addParking(dim.c_str(), sf::Vector2f(97.5f, 100.0f)); // .attachTo(road1, offset);
    Parking& parking1 = city.addParking(dim.c_str(), parking0.position() + parking0.delta());
    Parking& parking2 = city.addParking(dim.c_str(), parking1.position() + parking1.delta());
    Parking& parking3 = city.addParking(dim.c_str(), parking2.position() + parking2.delta());
    Parking& parking4 = city.addParking(dim.c_str(), parking3.position() + parking3.delta());

    // Add parked cars (static)
    Car& car0 = city.addCar("Renault.Twingo", parking0);
    Car& car1 = city.addCar("Renault.Twingo", parking1);
    Car& car2 = city.addCar("Renault.Twingo", parking3);

    // Debug Renault.Twingo
    //addGhost("Renault.Twingo", sf::Vector2f(107.994f, 100.0f), 0.0f); // Em0
    //addGhost("Renault.Twingo", sf::Vector2f(109.275f, 100.193f), 0.300045f); // Em1, ThetaE1
    //addGhost("Renault.Twingo", sf::Vector2f(108.345f, 99.7811f), 0.300045f + 0.235058f); // Em2, ThetaE1 + ThetaE2

    // Debug QQ
    //addGhost("QQ", sf::Vector2f(108.405f, 100.0f), 0.0f); // Em0
    //addGhost("QQ", sf::Vector2f(109.06f, 100.042f), 0.126959f); // Em1, ThetaE1
    //addGhost("QQ", sf::Vector2f(108.59f, 99.9596f), 0.126959f + 0.0919716f); // Em2, ThetaE1 + ThetaE2

    // Self-parking car (dynamic). Always be the last in the container
    Car& ego = city.addEgo("QQ", parking0.position() + sf::Vector2f(0.0f, 5.0f), 0.0f, 0.0f);

#if 0
    // Make the car react to some I/O events
    ego.registerCallback(sf::Keyboard::PageDown, [](Car& ego) {
        ego.turningIndicator(false, ego.turning_right() ^ true);
    });
    ego.registerCallback(sf::Keyboard::PageUp, [](Car& ego) {
        ego.turningIndicator(ego.turning_left() ^ true, false);
    });
    ego.registerCallback(sf::Keyboard::Up, [](Car& ego) {
        ego.setRefSpeed(1.0f);
    });
    ego.registerCallback(sf::Keyboard::Down, [](Car& ego) {
        ego.setRefSpeed(0.0f);
    });
    ego.registerCallback(sf::Keyboard::Right, [](Car& ego) {
        float ref = ego.getRefSteering() - 0.1f;
        ego.setRefSteering(constrain(ref, -ego.dim.max_steering_angle, ego.dim.max_steering_angle));
    });
    ego.registerCallback(sf::Keyboard::Left, [](Car& ego) {
        float ref = ego.getRefSteering() + 0.1f;
        ego.setRefSteering(constrain(ref, -ego.dim.max_steering_angle, ego.dim.max_steering_angle));
    });

    // With trailer
    //Trailer& tr = ego.attachTrailer(TrailerDimensions::get("generic"), DEG2RAD(30.0f));
    //std::cout << tr << std::endl;
#endif
}
#pragma GCC diagnostic pop

// -----------------------------------------------------------------------------
int main()
{
    Application app(WINDOW_WIDTH, WINDOW_HEIGHT, "Auto Parking");
    GUISimulation gui(app);
    gui.bgColor = sf::Color(255,255,255,255);

    try
    {
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
