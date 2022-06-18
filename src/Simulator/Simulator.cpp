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
#include "Renderer/FontManager.hpp"

//------------------------------------------------------------------------------
void Simulator::ScenarioLoader::onLoading()
{
    m_scenario.name = prototype<const char* (void)>("simulation_name");
    m_scenario.create = prototype<Car& (City&)>("create_city");
    m_scenario.halt = prototype<bool (Simulator const&)>("halt_simulation_when");
    m_scenario.react = prototype<void(Simulator&, size_t)>("react_to");
}

//------------------------------------------------------------------------------
Simulator::Simulator(sf::RenderWindow& renderer)
    : m_renderer(renderer), m_loader(m_scenario)
{
    m_message_bar.font(FontManager::instance().font("main font"));
}

//------------------------------------------------------------------------------
bool Simulator::load(Scenario const& scenario)
{
    m_scenario.name = scenario.name;
    m_scenario.create = scenario.create;
    m_scenario.halt = scenario.halt;
    m_scenario.react = scenario.react;

    // Check if it has been loaded correctly
    m_scenario_loaded = (m_scenario.name != nullptr) &&
                        (m_scenario.create != nullptr) &&
                        (m_scenario.halt != nullptr) &&
                        (m_scenario.react != nullptr);
    if (!m_scenario_loaded)
    {
        m_message_bar.entry("Failed loading the scenario because has "
                            "detected nullptr functions", sf::Color::Red);
        return false;
    }
    return true;
}

//------------------------------------------------------------------------------
bool Simulator::load(const char* lib_name)
{
    m_scenario_loaded = m_loader.load(lib_name);
    if (!m_scenario_loaded)
    {
        m_message_bar.entry("Failed loading the scenario: " + m_loader.error(),
                            sf::Color::Red);
        return false;
    }

    return true;
}

//------------------------------------------------------------------------------
bool Simulator::reload()
{
    return m_loader.reload();
}

//------------------------------------------------------------------------------
void Simulator::create()
{
    if (!m_scenario_loaded)
    {
        std::cout << "Scenari pas charge\n";
        return ;
    }

    std::string name(m_scenario.name());

    // Set simulation name on the GUI
    m_message_bar.entry("Starting simulation " + name, sf::Color::Green);
    m_renderer.setTitle(m_scenario.name());

    // Create a new city
    m_city.reset();
    BluePrints::init();
    m_ego = &m_scenario.create(m_city);

    // Make the camera follow the ego car
    follow(m_ego);
}

//------------------------------------------------------------------------------
void Simulator::activate()
{
    m_time.restart();
}

//------------------------------------------------------------------------------
void Simulator::deactivate()
{
    // Nothing to do
}

//------------------------------------------------------------------------------
void Simulator::release()
{
    m_city.reset();
    //m_scenario_loaded = false;
}

//------------------------------------------------------------------------------
void Simulator::reactTo(size_t key)
{
    m_scenario.react(*this, key);
}

//------------------------------------------------------------------------------
bool Simulator::running() const
{
    return m_scenario_loaded && (!m_scenario.halt(*this));
}

//------------------------------------------------------------------------------
inline static bool isEgo(Car const& car)
{
    return car.name[0] == 'e' && car.name[1] == 'g' && car.name[2] == 'o';
}

//------------------------------------------------------------------------------
void Simulator::collisions(Car& ego)
{
    bool collided = false;

    ego.clear_collided();
    for (auto& it: m_city.cars())
    {
        // Do not collide to itself
        if (it.get() == &ego)
            continue ;

        it->clear_collided();
        if (ego.collides(*it))
        {
            collided = true;
        }
    }

    if (collided)
    {
        m_message_bar.entry("Collision", sf::Color::Red);
    }
}

//------------------------------------------------------------------------------
void Simulator::update(const float dt)
{
    // Auto reload the scenario file if it has changed.
    //m_loader.reloadIfChanged();

    // Update physics, ECU, sensors ...
    for (auto& it: m_city.cars())
    {
        it->update(dt);
        if (isEgo(*it))
        {
            collisions(*it);
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
void Simulator::draw_simulation()
{
    // Draw the spatial hash grid
    //Renderer::draw(m_city.grid(), m_renderer);

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
void Simulator::draw_hud()
{
    m_message_bar.size(m_renderer.getSize());
    m_renderer.draw(m_message_bar);
}
