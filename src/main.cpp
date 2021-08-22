#include <iostream>
#include <map>
#include <deque>
#include <cmath>
#include <SFML/Graphics.hpp>

#define ZOOM 1.0f
#define RAD2DEG(r) ((r) * 57.295779513f)
#define DEG2RAD(d) ((d) * 0.01745329251994f)

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
    float wheel_width = 0.1f;
    //! \brief Limit of steering angle absolute angle [rad]
    float max_steering_angle;
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
        middle_front = e / sinf(steering);

        // Inner radius [m]
        internal = sqrtf(middle_front * middle_front - e * e) - (w / 2.0f);

        // Outer radius [m]
        external = sqrtf((internal + w) * (internal + w) + (e + p) * (e + p));

        // Largeur balayee [m]
        bal = external - internal;

        // surlargeur [m]
        sur = bal - w;
    }

    //! \brief turning radius of the virtual wheel located in the middle of the
    //! front axle [meter].
    float middle_front;
    //! \brief turning radius of the internal wheel [meter].
    float internal;
    //! \brief turning radius of the external wheel [meter].
    float external;
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

    CarShape(CarDimension const& dim_)
       : dim(dim_)
    {
        // Wheel offset along the Y-axis
        const float K = dim.width / 2 - dim.wheel_width / 2;

        m_wheels[Wheel::Type::FL].offset = sf::Vector2f(dim.wheelbase, -K);
        m_wheels[Wheel::Type::FR].offset = sf::Vector2f(dim.wheelbase, K);
        m_wheels[Wheel::Type::RL].offset = sf::Vector2f(0.0f, -K);
        m_wheels[Wheel::Type::RR].offset = sf::Vector2f(0.0f, K);
        //wheels[FL].speed = m_wheels[FR].speed = NAN;
        //wheels[RL].speed = m_wheels[RR].speed = NAN;
    }

    void set(sf::Vector2f const& position, float const heading, float const steering)
    {
        m_heading = heading;
        m_position = position;

        const float COS_YAW = cosf(heading);
        const float SIN_YAW = sinf(heading);
        const float FRONT_X = position.x + m_wheels[Wheel::Type::FL].offset.x * COS_YAW;
        const float FRONT_Y = position.y + m_wheels[Wheel::Type::FL].offset.x * SIN_YAW;
        const float OFFSET_SIN = m_wheels[Wheel::Type::FL].offset.y * SIN_YAW;
        const float OFFSET_COS = m_wheels[Wheel::Type::FL].offset.y * COS_YAW;

        m_wheels[Wheel::Type::FL].position.x = FRONT_X - OFFSET_SIN;
        m_wheels[Wheel::Type::FL].position.y = FRONT_Y + OFFSET_COS;
        m_wheels[Wheel::Type::FR].position.x = FRONT_X + OFFSET_SIN;
        m_wheels[Wheel::Type::FR].position.x = FRONT_Y - OFFSET_COS;

        m_wheels[Wheel::Type::RL].position.x = position.x + OFFSET_SIN;
        m_wheels[Wheel::Type::RL].position.y = position.y - OFFSET_COS;
        m_wheels[Wheel::Type::RR].position.x = position.x - OFFSET_SIN;
        m_wheels[Wheel::Type::RR].position.x = position.y + OFFSET_COS;

        m_wheels[Wheel::Type::FL].steering = m_wheels[Wheel::Type::FR].steering = steering;
        m_wheels[Wheel::Type::RL].steering = m_wheels[Wheel::Type::RR].steering = 0.0f;
    }

    inline std::array<Wheel, 4> const& wheels() const { return m_wheels; }
    inline Wheel const& wheel(Wheel::Type const i) const { return m_wheels[i]; }
    inline float heading() const { return m_heading; }
    inline float steering() const
    {
        return m_wheels[0].steering;
    }

    void steering(float const v)
    {
        m_wheels[Wheel::Type::FL].steering = m_wheels[Wheel::Type::FR].steering = v;
    }
    inline sf::Vector2f position() const { return m_position; }

public:

    CarDimension const& dim;

protected:

    std::array<Wheel, 4> m_wheels;
    sf::Vector2f m_position;
    float m_heading;
};

class CarControl
{
public:

    void set()
    {}

    //----------------------------------------------------------------------
    //! \brief The desired
    //----------------------------------------------------------------------
    struct References
    {
        float body_speed = 0.0f;
        float steering = 0.0f;
    };

    //----------------------------------------------------------------------
    //! \brief
    //----------------------------------------------------------------------
    struct Inputs
    {
    };

    //----------------------------------------------------------------------
    //! \brief
    //----------------------------------------------------------------------
    struct Outputs
    {
        float body_speed = 0.0f;
        float steering = 0.0f;
    };

    //----------------------------------------------------------------------
    //! \brief
    //----------------------------------------------------------------------
    void update(float const /*dt*/ /*, sensors*/)
    {
        outputs.body_speed = ref.body_speed;
        outputs.steering = ref.steering;
    }

    References ref;
    Outputs outputs;
};

class CarPhysics
{
public:

    virtual ~CarPhysics() = default;
    virtual void update(float const dt, CarControl const& control) = 0;
    virtual float acceleration() const = 0;
    virtual float speed() const = 0;
    virtual sf::Vector2f position() const = 0;
};

class CarKinematic: public CarPhysics
{
public:

    CarKinematic(CarShape& shape)
        : m_shape(shape)
    {}

    void init(sf::Vector2f const& position, float const speed,
              float const heading, float const steering)
    {
        m_shape.set(position, heading, steering);
        m_speed = speed;
    }

    virtual void update(float const dt, CarControl const& control) override
    {
        float steering = control.outputs.steering;
        float heading = m_shape.heading();
        sf::Vector2f position = m_shape.position();

        m_speed = control.outputs.body_speed;
        heading += dt * m_speed * tanf(steering) / m_shape.dim.wheelbase;
        position.x += dt * m_speed * cosf(heading);
        position.y += dt * m_speed * sinf(heading);
        m_shape.set(position, heading, steering);
    }

    virtual float acceleration() const override
    {
        return 0.0f;
    }

    virtual float speed() const override
    {
        return m_speed;
    }

    virtual sf::Vector2f position() const override
    {
        return m_shape.position();
    }

private:

    CarShape& m_shape;
    float m_speed = 0.0f;
};

class Trajectory
{
public:

    Trajectory(CarDimension const& dim)
        : m_dim(dim), m_min_turning_radius(dim, dim.max_steering_angle)
    {}

    // ---------------------------------------------------------------------
    //! \brief Compute trajectory waypoints.
    //! \param[in] Xi: the X world coordinate of the initial position of the
    //! middle of the back wheel axle of the car at the begining of the
    //! maneuver.
    //! \param[in] Yi: the Y world coordinate of the initial position of the
    //! middle of the back wheel axle of the car at the begining of the
    //! maneuver.
    //! \param[in] Xf: the desired X world coordinate of the final
    //! destination in where the middle of the back wheel axle of the car
    //! has to be at the end of the maneuver.
    //! \param[in] Yf: the desired Y world coordinate of the final
    //! destination in where the middle of the back wheel axle of the car
    //! has to be at the end of the maneuver.
    // ---------------------------------------------------------------------
    bool computePathPlanning(float const /*Xi*/, float const /*Yi*/,
                             float const /*Xf*/, float const /*Yf*/)
    {
        return false;
    }

    // ---------------------------------------------------------------------
    //! \brief Generated tables of timed references for the auto-pilot to
    //! follow the desired trajectory computed by computePathPlanning().
    //! \param[in] vmax: maximal velocity [m/s] for the car.
    //! \param[in] ades: desired acceleration [m/s^2] for the car.
    // ---------------------------------------------------------------------
    void generateReferenceTrajectory(float const /*vmax*/, float const /*ades*/)
    {}

    inline float Rmin() const { return m_min_turning_radius.middle_front; }
    inline float Rimin() const { return m_min_turning_radius.internal; }
    inline float Remin() const { return m_min_turning_radius.external; }
    inline float Rmintild() const { return m_min_turning_radius.internal + m_dim.width / 2.0f; }

private:

    CarDimension const& m_dim;
    TurningRadius m_min_turning_radius;
};

class Car
{
public:

    Car(CarDimension const& dim_)
        : dim(dim_), m_shape(dim_), m_physics(m_shape)
    {}

    Car(const char* model)
        : Car(CarDimensions::get(model))
    {}

    inline void init(sf::Vector2f const& position, float const speed,
                     float const heading, float const steering)
    {
        m_physics.init(position, speed, heading, steering);
    }

    void update(float const /*dt*/)
    {

    }

    inline float acceleration() const
    {
        return m_physics.acceleration();
    }

    inline float speed() const
    {
        return m_physics.speed();
    }

    inline sf::Vector2f position() const
    {
        return m_physics.position();
    }

    inline float heading() const
    {
        return m_shape.heading();
    }

    inline std::array<Wheel, 4> const& wheels() const
    {
        return m_shape.wheels();
    }

    inline Wheel const& wheel(Wheel::Type const i) const
    {
        return m_shape.wheel(i);
    }

    inline CarShape const& shape() const
    {
        return m_shape;
    }

public:

    CarDimension const dim;

private:

    CarShape m_shape;
    CarKinematic m_physics;
    CarControl m_control;
};

class CarDrawable : public sf::Drawable
{
public:

    void bind(Car const& car)
    {
        m_car = &car;
        CarDimension const& dim = car.dim;

        m_body_shape.setSize(sf::Vector2f(dim.length, dim.width));
        m_body_shape.setOrigin(dim.back_overhang, m_body_shape.getSize().y / 2); // Origin on the middle of the rear wheels
        m_body_shape.setFillColor(sf::Color(165, 42, 42));
        m_body_shape.setOutlineThickness(ZOOM);
        m_body_shape.setOutlineColor(sf::Color::Blue);

        m_wheel_shape.setSize(sf::Vector2f(dim.wheel_radius * 2, dim.wheel_width));
        m_wheel_shape.setOrigin(m_wheel_shape.getSize().x / 2, m_wheel_shape.getSize().y / 2);
        m_wheel_shape.setFillColor(sf::Color::Black);
        m_wheel_shape.setOutlineThickness(ZOOM);
        m_wheel_shape.setOutlineColor(sf::Color::Yellow);
    }

protected:

    void draw(sf::RenderTarget& target, sf::RenderStates states) const override final
    {
        if (m_car == nullptr)
            return ;

        target.draw(m_body_shape, states);
        for (int i = 0; i < 4; ++i)
        {
            CarShape const& shape = m_car->shape();
            Wheel const& wheel = shape.wheel(static_cast<Wheel::Type>(i));

            m_wheel_shape.setPosition(wheel.position);
            m_wheel_shape.setRotation(RAD2DEG(shape.heading() + wheel.steering));
            target.draw(m_wheel_shape, states);
        }
    }

private:

    Car const* m_car = nullptr;
    //! \brief The body drawn as a rectangle.
    mutable sf::RectangleShape m_body_shape;
    //! \brief The front wheel drawn as a rectangle.
    mutable sf::RectangleShape m_wheel_shape;
};

class Simulation : public sf::Drawable
{
public:

    void addedCar(Car& c)
    {
        m_cars.push_back(c);
    }

    void update(float const dt)
    {
        for (auto& it: m_cars)
        {
            it.update(dt);
        }
    }

protected:

    void draw(sf::RenderTarget& target, sf::RenderStates states) const override final
    {
        CarDrawable d;

        for (auto const& it: m_cars)
        {
            d.bind(it);
            target.draw(d, states);
        }
    }

    std::deque<Car> m_cars;
};

int main()
{
   CarDimensions::add("Audi.A7", CarDimension(1.646f, 3.615f, 2.492f, 0.494f, 0.328f, 10.0f));

   std::cout << CarDimensions::get("Audi.A7") << std::endl;

   return 0;
}
