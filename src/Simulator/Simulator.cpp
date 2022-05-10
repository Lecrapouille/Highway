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

#include "Simulator/Simulator.hpp"
#include "Renderer/Renderer.hpp"

#define DEFAULT_CAR_COLOR 178, 174, 174
#define EGO_CAR_COLOR 124, 99, 197
#define COLISION_COLOR 255, 0, 0

//------------------------------------------------------------------------------
Simulator::Simulator(sf::RenderWindow& renderer)
    : m_renderer(renderer)
{
    BluePrints::init();
}

//------------------------------------------------------------------------------
bool Simulator::load(Scenario const& scenario)
{
    m_simulation.name = scenario.name;
    m_simulation.create = scenario.create;
    m_simulation.halt = scenario.halt;
    m_simulation.react = scenario.react;

    activate();
    std::cout << "Loading simulation name: " << m_simulation.name()
              << std::endl;
    m_renderer.setTitle(m_simulation.name());

    return true;
}

//------------------------------------------------------------------------------
bool Simulator::load(const char* lib_name)
{
    if (m_simulation.load(lib_name))
    {
        activate();
        std::cout << "Loading simulation name: " << m_simulation.name()
                  << std::endl;
        m_renderer.setTitle(m_simulation.name());

        return true;
    }

    std::cerr << "Exception: "<< m_simulation.error() << std::endl;
    return true;
}

//------------------------------------------------------------------------------
bool Simulator::reload()
{
    return load(m_simulation.path().c_str()); // FIXME useless
}

//------------------------------------------------------------------------------
void Simulator::reactTo(size_t key)
{
    //if (m_simulation) // FIXME
    {
        m_simulation.react(*this, key);
    }
}

//------------------------------------------------------------------------------
bool Simulator::running() const
{
    return !m_simulation.halt(*this);
}

//------------------------------------------------------------------------------
void Simulator::activate()
{
    // if (!m_simulation) { // FIXME
    //   cerr << "failed" << endl;
    //   return ;
    // }
    m_ego = &m_simulation.create(m_city);
    follow(m_ego);
    messagebox("Simulation '" + std::string(m_simulation.name()) + "' activate", sf::Color::Green);
    m_time.restart();
}

//------------------------------------------------------------------------------
void Simulator::deactivate()
{
    m_city.reset();
}

//------------------------------------------------------------------------------
inline static bool isEgo(Car const& car)
{
    return car.name[0] == 'e' && car.name[1] == 'g' && car.name[2] == 'o';
}

//------------------------------------------------------------------------------
void Simulator::showCollisions(Car& ego)
{
    ego.color = sf::Color(EGO_CAR_COLOR);
    for (auto& it: m_city.cars())
    {
        if ((&*it != &ego) && (ego.collides(*it)))
        {
            it->color = sf::Color(COLISION_COLOR);
            ego.color = sf::Color(COLISION_COLOR);
        }
        else
        {
            it->color = sf::Color(DEFAULT_CAR_COLOR);
        }
    }
}

//------------------------------------------------------------------------------
void Simulator::update(const float dt)
{
    // Update physics, ECU, sensors ...
    for (auto& it: m_city.cars())
    {
        it->update(dt);
        if (isEgo(*it))
        {
            showCollisions(*it);
        }
    }

    // Make the camera follows the car
    if (m_follow != nullptr)
    {
        m_camera = m_follow->position();
    }
}

// Mauvaise facon de faire:
// class DrawableCity<City>(City& city) : m_city(city)
// { City& m_city; draw() { }
// };

//------------------------------------------------------------------------------
void Simulator::draw()
{
    // Draw the city
    for (auto const& it: m_city.parkings())
    {
        Renderer::draw(*it, m_renderer);
    }

    // Draw vehicle and ego
    for (auto const& it: m_city.cars())
    {
        Renderer::draw(*it, m_renderer);
    }

    // Draw ghost cars
    /*for (auto const& it: m_city.ghosts())
      {
      Renderer::draw(*it, m_renderer);
      }*/
}

//------------------------------------------------------------------------------
void Simulator::hud()
{
    Renderer::draw(m_city.grid(), m_renderer);
}
