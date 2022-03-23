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
    BluePrints::init<CarBluePrint>(
        {
            // https://www.renault-guyane.fr/cars/TWINGOB07Ph2h/DimensionsEtMotorisations.html
            { "Renault.Twingo", { 3.615f, 1.646f, 2.492f, 0.494f, 0.328f, 10.0f } },
            // https://www.largus.fr/images/images/ds3-crossback-dimensions-redimensionner.png
            { "Citroen.DS3", { 4.118f, 1.79f, 2.558f, 0.7f, 0.328f, 10.4f } },
            // https://www.bsegfr.com/images/books/5/8/index.47.jpg
            { "Citroen.C3", { 3.941f, 1.728f, 2.466f, 0.66f, 0.328f, 10.7f } },
            // https://www.vehikit.fr/nissan
            { "Nissan.NV200", { 4.321f, 1.219f, 2.725f, 0.840f, 0.241f, 10.6f} },
            // https://audimediacenter-a.akamaihd.net/system/production/media/78914/images/82a9fc874e33b8db4c849665c633c5148c3212d0/A196829_full.jpg?1582526293
            { "Audi.A6", { 4.951f, 1.902f, 2.924f, 1.105f, 0.328f, 11.7f } },
            //
            { "QQ", { 3.9876841117376247f, 1.8508483613211535f, 2.6835034227027936f, 0.584285581986275f, 0.328f, 11.7f } },
            //
            { "Mini.Cooper", { 3.62f, 1.68f, 2.46f, 0.58f, 0.328f, 10.7f } },
        });

    BluePrints::init<TrailerBluePrint>(
        {
            { "generic", { 1.646f, 1.646f, 2.5f, 0.494f, 0.2f } },
        });

    //----------------------------------------------------------------------
    //! \brief Private static database. For example:
    //! https://www.virages.com/Blog/Dimensions-Places-De-Parking
    //! ../../doc/pics/PerpendicularSpots.gif
    //! ../../doc/pics/DiagonalSpots.gif
    //! ../../doc/pics/ParallelSpots.jpg
    //----------------------------------------------------------------------
    BluePrints::init<ParkingBluePrint>(
        {
            { "epi.0", { 5.0f, 2.0f, 0u } },
            { "epi.45", { 4.8f, 2.2f, 45u } },
            { "epi.60", { 5.15f, 2.25f, 60u } },
            { "epi.75", { 5.1f, 2.25f, 75u } },
            { "epi.90", { 5.0f, 2.3f, 90u } },
            { "creneau", { 5.0f, 2.0f, 0u } },
            { "bataille", { 5.0f, 2.3f, 90u } },
        });
}

//------------------------------------------------------------------------------
sf::Vector2f Simulator::world(sf::Vector2i const& p)
{
    return m_renderer.mapPixelToCoords(p);
}

//------------------------------------------------------------------------------
void Simulator::reset()
{
    city.m_cars.clear();
    city.m_parkings.clear();
    city.m_ego = nullptr;
}

//------------------------------------------------------------------------------
void Simulator::update(const float dt)
{
    // Update car physics
    for (auto& it: city.m_cars)
    {
        it->update(dt);
    }

    if (city.m_ego != nullptr)
    {
        // Update the Ego's car physics
        city.m_ego->update(dt);

        // Collide with other car ?
        city.m_ego->color = sf::Color(EGO_CAR_COLOR);
        for (auto& it: city.m_cars)
        {
            if (city.m_ego->collides(*it))
            {
                it->color = sf::Color(COLISION_COLOR);
                city.m_ego->color = sf::Color(COLISION_COLOR);
            }
            else
            {
                it->color = sf::Color(DEFAULT_CAR_COLOR);
            }
        }
    }
}

//------------------------------------------------------------------------------
void Simulator::draw(sf::RenderWindow& renderer, sf::View& view)
{
    // Make the camera follows the self-parking car
    if (city.m_ego != nullptr)
    {
        view.setCenter(city.m_ego->position());
        m_renderer.setView(view);
    }

    // Draw the world
    for (auto const& it: city.m_parkings)
    {
        Renderer::draw(it, m_renderer);
    }

    // Draw cars
    for (auto const& it: city.m_cars)
    {
        Renderer::draw(*it, m_renderer);
    }

    // Draw cars
    for (auto const& it: city.m_ghosts)
    {
        Renderer::draw(*it, m_renderer);
    }

    if (city.m_ego != nullptr)
    {
        Renderer::draw(*city.m_ego, m_renderer);
    }
}
