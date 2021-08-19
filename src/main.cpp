#include <iostream>
#include <map>

struct ParkingDimension
{
    //! \brief Vehicle width [meter]
    float width;
    //! \brief Vehicle length [meter]
    float length;
    //! \brief Orientation [rad]
    float angle;
};

struct CarDimension
{
    //--------------------------------------------------------------------------
    //! \brief Define vehicle constants
    //! \param[in] w: car width [meter]
    //! \param[in] l: car length [meter]
    //! \param[in] wb: wheelbase length [meter]
    //! \param[in] bo: back overhang [meter]
    //! \param[in] wr: wheel radius [meter]
    //! \param[in] tc: turning circle [meter]
    //--------------------------------------------------------------------------
    CarDimension(const float w, const float l, const float wb,
                 const float bo, const float wr, const float /*tc*/)
        : width(w), length(l), wheelbase(wb), back_overhang(bo),
          front_overhang(length - wheelbase - back_overhang), wheel_radius(wr)
    {}

    friend std::ostream& operator<<(std::ostream& os, CarDimension const& dim)
    {
        return os << "length=" << dim.length
                  << ", width=" << dim.width
                  << ", wheelbase=" << dim.wheelbase
                  << ", back_overhang=" << dim.back_overhang
                  << ", front_overhang=" << dim.front_overhang
                  << std::endl;
    }

    //! \brief Vehicle width [meter]
    float width;
    //! \brief Vehicle length [meter]
    float length;
    //! \brief Empattement [meter]
    float wheelbase;
    //! \brief Porte a faux arriere [meter]
    float back_overhang;
    //! \brief Porte a faux avant [meter]
    float front_overhang;
    //! \brief Rayon roue [meter]
    float wheel_radius;
    //! \brief Epaisseur roue [meter] (only used for the rendering)
    //float wheel_width = 0.1f;
    //! \brief Limit of steering angle absolute angle [rad]
    //float max_steering_angle;
};

class CarDimensions
{
public:

    static bool add(const char* n, CarDimension const& dim)
    {
        return dictionary().insert(std::map<std::string, CarDimension>::value_type(n, dim)).second;
    }

    static CarDimension get(const char* n)
    {
        return dictionary().at(n);
    }

private:

    static std::map<std::string, CarDimension>& dictionary()
    {
        static std::map<std::string, CarDimension> dico = {
            // https://www.renault-guyane.fr/cars/TWINGOB07Ph2h/DimensionsEtMotorisations.html
            { "Renault.Twingo", { 1.646f, 3.615f, 2.492f, 0.494f, 0.328f, 10.0f } },
            // https://www.largus.fr/images/images/ds3-crossback-dimensions-redimensionner.png
            { "Citroel.DS3", { 1.79f, 4.118f, 2.558f, 0.7f, 0.328f, 10.4f } },
            // https://www.vehikit.fr/nissan
            { "Nissan.NV200", { 1.219f, 4.321f, 2.725f, 0.840f, 0.241f, 10.6f} },
            // https://audimediacenter-a.akamaihd.net/system/production/media/78914/images/82a9fc874e33b8db4c849665c633c5148c3212d0/A196829_full.jpg?1582526293
            { "Audi.A6", { 1.886f, 4.93f, 2.924f, 1.095f, 0.328f, 11.7f } },
        };
        return dico;
    }
};

struct TurningRadius
{
    TurningRadius(CarDimension const& dim, float const steering)
    {
        const float e = dim.wheelbase;
        const float w = dim.width;
        const float p = dim.front_overhang;

        // Rayon de braquage [m] (turning radius)
        R = e / sinf(steering);

        // Inner radius [m]
        Ri = sqrtf(R * R - e * e) - (w / 2.0f);

        // Outer radius [m]
        Re = sqrtf((.Ri + w) * (Ri + w) + (e + p) * (e + p));

        // Largeur balayee [m]
        bal = Re - Ri;

        // surlargeur [m]
        sur = bal - w;
    }

    //! \brief turning radius of the virtual wheel located in the middle of
    //! the front axle [meter].
    float R;
    //! \brief turning radius of the internal wheel [meter].
    float Ri;
    //! \brief turning radius of the external wheel [meter].
    float Re;
    //! \brief Largeur balayee [meter]
    float bal;
    //! \brief Surlargeur [meter]
    float sur;
};

struct Wheel
{
   enum Type { FL, FR, RR, RL };

   //! \brief Relative position from
   sf::Vector2f offset;
   //! \brief current position in the world
   sf::Vector2f position;
   //! \brief yaw angle
   float steering;
   //! \brief speed
   //float speed;
};

class CarShape
{
public:

    CarShape(CarDimension const& dim, sf::Vector2f const& back, float const yaw, float const steering)
       : dim(d)
    {
        // Wheel offset along the Y-axis
        const float K = dim.width / 2 - dim.wheel_width / 2;

        wheels[FL].offset = sf::Vector2f(dim.wheelbase, -K);
        wheels[FR].offset = sf::Vector2f(dim.wheelbase, K);
        wheels[RL].offset = sf::Vector2f(0.0f, -K);
        wheels[RR].offset = sf::Vector2f(0.0f, K);
        //wheels[FL].speed = wheels[FR].speed = NAN;
        //wheels[RL].speed = wheels[RR].speed = NAN;

        update(back, yaw, steering)
    }

    void update(sf::Vector2f const& back, float const yaw, float const steering)
    {
        m_heading = yaw_;
        m_steering = steering_;
        m_position = back;

        const float COS_YAW = cosf(yaw);
        const float SIN_YAW = sinf(yaw);
        const float FRONT_X = back.x + wheels[FL].offset.x * COS_YAW;
        const float FRONT_Y = back.y + wheels[FL].offset.x * SIN_YAW;
        const float OFFSET_SIN = wheels[FL].offset.y * SIN_YAW;
        const float OFFSET_COS = wheels[FL].offset.y * COS_YAW;

        wheels[FL].position.x = FRONT_X - OFFSET_SIN;
        wheels[FL].position.y = FRONT_Y + OFFSET_COS;
        wheels[FR].position.x = FRONT_X + OFFSET_SIN;
        wheels[FR].position.x = FRONT_Y - OFFSET_COS;

        wheels[RL].position.x = back.x + OFFSET_SIN;
        wheels[RL].position.y = back.y - OFFSET_COS;
        wheels[RR].position.x = back.x - OFFSET_SIN;
        wheels[RR].position.x = back.y + OFFSET_COS;

        wheels[FL].steering = wheels[FR].steering = steering;
        wheels[RL].steering = wheels[RR].steering = 0.0f;
    }

    inline std::array<Wheel, 4> const& wheels() const { return m_wheels; }
    inline Wheel const& wheel(Wheel::Type const i) const { return m_wheels[i]; }
    inline float heading() const { return m_heading; }
    inline float steering() const { return m_steering; }
    inline sf::Vector2f position() const { return position; }

public:

    CarDimensions const& dim;

protected:

    std::array<Wheel, 4> m_wheels;
    sf::Vector2f const& m_position;
    float m_heading;
    float m_steering;
};

class Kinematic
{
public:

    Kinematic(CarShape& shape_)
        : shape(shape_)
    {}

    void init(sf::Vector2f const& position, float heading)
    {
        shape.position = position;
        shape.heading = heading;
    }

    void update(float const dt, Control const& control);
    {
        float const steering = control.outputs.steering;
        float const speed = control.outputs.speed;

        float heading += dt * body_speed * tanf(steering) / shape.dim.wheelbase;
        float position.x += dt * body_speed * cosf(heading);
        float position.y += dt * body_speed * sinf(heading);
    }

private:

    CarShape& shape;
};

class Car
{
public:

    Car(CarDimension const& dim)
        : shape(dim), turning_radius(dim, dim.max_steering_angle)
    {}

    Car(const char* model)
        : Car(CarDimensions::get(name))
    {}

    inline float Rmin() const { return min_radius.fake_front; }
    inline float Rimin() const { return min_radius.internal; }
    inline float Remin() const { return min_radius.external; }
    inline float Rmintild() const { return min_radius.internal + dim.width / 2.0f; }

public:

    CarShape shape;

private:

    TurningRadius min;
    Control control;
    Physic physic;
};

class CarDrawable : public sf::Drawable
{
public:

    CarSDrawable(Car const& car_)
        : car(car_)
    {
        CarDimensions const& dim = car.shape.dim;

        body.setSize(sf::Vector2f(dim.length, dim.width));
        body.setOrigin(dim.back_overhang, body.getSize().y / 2); // Origin on the middle of the rear wheels
        body.setFillColor(sf::Color(165, 42, 42));
        body.setOutlineThickness(ZOOM);
        body.setOutlineColor(sf::Color::Blue);

        wheel.setSize(sf::Vector2f(dim.wheel_radius * 2, dim.wheel_width));
        wheel.setOrigin(wheel.getSize().x / 2, wheels[i].getSize().y / 2);
        wheel.setFillColor(sf::Color::Black);
        wheel.setOutlineThickness(ZOOM);
        wheel.setOutlineColor(sf::Color::Yellow);
    }

protected:

    void draw(sf::RenderTarget& target, sf::RenderStates states) const override final
    {
        target.draw(body, states);
        for (int i = 0; i < 4; ++i)
        {
            wheel.setPosition(shape.wheels[i].position);
            wheel.setRotation(RAD2DEG(shape.yaw +  shape.wheels[i].steering));
            target.draw(wheel, states);
        }
    }

public:

    Car& car;

private:

    //! \brief The body drawn as a rectangle.
    sf::RectangleShape body;
    //! \brief The front wheel drawn as a rectangle.
    sf::RectangleShape wheel;
};

class Simulation : public sf::Drawable
{
public:

    void addedCar(Car& c)
    {
        m_cars.push_back(CarDrawable(c));
    }

    void update(float const dt)
    {
        for (auto const& it = m_cars)
        {
            it.update(dt;
        }
    }

protected:

    void draw(sf::RenderTarget& target, sf::RenderStates states) const override final
    {
        for (auto const& it = m_cars)
        {
            it.draw(target, states);
        }
    }

    std::queu<CarDrawable> m_cars;
};

int main()
{
   CarDimensions::add("Audi.A7", CarDimension(1.646f, 3.615f, 2.492f, 0.494f, 0.328f, 10.0f));

   std::cout << CarDimensions::get("Audi.A7") << std::endl;

   return 0;
}

