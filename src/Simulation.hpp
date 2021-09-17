// 2021 Quentin Quadrat quentin.quadrat@gmail.com
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

#ifndef MAIN_HPP
#  define MAIN_HPP

#  include "SFML/GUIStates.hpp"
#  include "Car/Car.hpp"
#  include "World/Parking.hpp"
#  include <atomic>

// *****************************************************************************
//! \brief
// *****************************************************************************
class Simulation: public GUIStates
{
public:

    Simulation(Application& application);

    ~Simulation()
    {
        renderer().close();
    }

    //! \brief Convert Window's X-Y position [pixel] to world's X-Y position [meter].
    //! \param[in] p: position in the windows [pixel].
    //! \return position in the world [meter].
    sf::Vector2f world(sf::Vector2i const& p);

    void clear();
    void createWorld(size_t angle, bool const entering);

    IACar& addPlayer(const char* model, sf::Vector2f const& position, float const heading,
                     float const speed = 0.0f, float const steering = 0.0f);
    IACar& addPlayer(CarDimension const& dim, sf::Vector2f const& position, float const heading,
                     float const speed = 0.0f, float const steering = 0.0f);
    Car& addCar(const char* model, Parking& parking);
    Car& addCar(const char* model, sf::Vector2f const& position, float const heading,
                float const speed = 0.0f, float const steering = 0.0f);
    Car& addCar(CarDimension const& dim, sf::Vector2f const& position, float const heading,
                float const speed, float const steering);

    Parking& addParking(const char* type, sf::Vector2f const& position);
    Parking& addParking(ParkingDimension const& dim, sf::Vector2f const& position);

private: // Derived from GUIStates

    //--------------------------------------------------------------------------
    //! \brief Inherit from GUI class. Draw the chessboard and pieces.
    //--------------------------------------------------------------------------
    virtual void draw(const float /*dt*/) override;

    //--------------------------------------------------------------------------
    //! \brief Inherit from GUI class. Update GUI.
    //----------------------------------/---------------------------------------
    virtual void update(const float dt) override;

    //--------------------------------------------------------------------------
    //! \brief Inherit from GUI class. Manage mouse and keyboard events.
    //--------------------------------------------------------------------------
    virtual void handleInput() override;

    //--------------------------------------------------------------------------
    //! \brief Inherit from GUI class. Return true if GUI is alive.
    //--------------------------------------------------------------------------
    virtual bool isRunning() override
    {
        return m_running;
    }

    //--------------------------------------------------------------------------
    //! \brief Called when the GUI has been enabled.
    //--------------------------------------------------------------------------
    virtual void activate() override
    {
        // Do nothing
    }

    //--------------------------------------------------------------------------
    //! \brief Called when the GUI has been disabled.
    //--------------------------------------------------------------------------
    virtual void deactivate() override
    {
        // Do nothing
    }

    // TODO thread for the physics

private:

    //! \brief Alive class ?
    std::atomic<bool> m_running{true};
    //! \brief For managing zoom and camera displacement
    sf::View m_view;
    //! \brief Mouse X,Y position within the world coordinate [meter].
    //! You directly can measure objects (in meter).
    sf::Vector2f m_mouse;
    //! \brief Container of parked cars
    std::deque<std::unique_ptr<Car>> m_cars;
    //! \brief The autonomous cars
    std::unique_ptr<IACar> m_ego = nullptr;
    //! \brief Container of parking slots
    std::deque<Parking> m_parkings;
    // TODO roads and bounding boxes of objects
};

#endif
