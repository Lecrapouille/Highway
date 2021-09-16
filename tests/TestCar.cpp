#include "gmock/gmock.h"
#include "gtest/gtest.h"

#define protected public
#define private public
#  pragma GCC diagnostic push
#  pragma GCC diagnostic ignored "-Wfloat-equal"
#    include "Car/Car.hpp"
#  pragma GCC diagnostic pop
#undef protected
#undef private

//--------------------------------------------------------------------------
TEST(TestCarShape, Constructor)
{
    CarShape c(CarDimensions::get("Renault.Twingo"));

    ASSERT_FLOAT_EQ(c.position().x, 0.0f);
    ASSERT_FLOAT_EQ(c.position().y, 0.0f);
    ASSERT_FLOAT_EQ(c.heading(), 0.0f);

    ASSERT_EQ(c.wheels().size(), 4u);
    ASSERT_FLOAT_EQ(c.wheel(CarShape::WheelName::FL).position.x, 0.0f);
    ASSERT_FLOAT_EQ(c.wheel(CarShape::WheelName::FL).position.y, 0.0f);
    ASSERT_FLOAT_EQ(c.wheel(CarShape::WheelName::FR).position.x, 0.0f);
    ASSERT_FLOAT_EQ(c.wheel(CarShape::WheelName::FR).position.y, 0.0f);
    ASSERT_FLOAT_EQ(c.wheel(CarShape::WheelName::RL).position.x, 0.0f);
    ASSERT_FLOAT_EQ(c.wheel(CarShape::WheelName::RL).position.y, 0.0f);
    ASSERT_FLOAT_EQ(c.wheel(CarShape::WheelName::RR).position.x, 0.0f);
    ASSERT_FLOAT_EQ(c.wheel(CarShape::WheelName::RR).position.y, 0.0f);
    ASSERT_FLOAT_EQ(c.wheel(CarShape::WheelName::FL).steering, 0.0f);
    ASSERT_FLOAT_EQ(c.wheel(CarShape::WheelName::FR).steering, 0.0f);
    ASSERT_FLOAT_EQ(c.wheel(CarShape::WheelName::RL).steering, 0.0f);
    ASSERT_FLOAT_EQ(c.wheel(CarShape::WheelName::RR).steering, 0.0f);
}

//--------------------------------------------------------------------------
TEST(TestCarShape, Init)
{
    CarShape c(CarDimensions::get("Renault.Twingo"));
    c.set(sf::Vector2f(100.0f, 50.0f), 2.0f, 3.0f);

    ASSERT_FLOAT_EQ(c.position().x, 100.0f);
    ASSERT_FLOAT_EQ(c.position().y, 50.0f);
    ASSERT_FLOAT_EQ(c.heading(), 2.0f);

    ASSERT_EQ(c.wheels().size(), 4u);
    ASSERT_FLOAT_EQ(c.wheel(CarShape::WheelName::FL).position.x, 99.665848f);
    ASSERT_FLOAT_EQ(c.wheel(CarShape::WheelName::FL).position.y, 52.58765f);
    ASSERT_FLOAT_EQ(c.wheel(CarShape::WheelName::FR).position.x, 98.260078f);
    ASSERT_FLOAT_EQ(c.wheel(CarShape::WheelName::FR).position.y, 51.944286f);
    ASSERT_FLOAT_EQ(c.wheel(CarShape::WheelName::RL).position.x, 100.70289f);
    ASSERT_FLOAT_EQ(c.wheel(CarShape::WheelName::RL).position.y, 50.321682f);
    ASSERT_FLOAT_EQ(c.wheel(CarShape::WheelName::RR).position.x, 99.297112f);
    ASSERT_FLOAT_EQ(c.wheel(CarShape::WheelName::RR).position.y, 49.678318f);
    ASSERT_FLOAT_EQ(c.wheel(CarShape::WheelName::FL).steering, 3.0f);
    ASSERT_FLOAT_EQ(c.wheel(CarShape::WheelName::FR).steering, 3.0f);
    ASSERT_FLOAT_EQ(c.wheel(CarShape::WheelName::RL).steering, 0.0f);
    ASSERT_FLOAT_EQ(c.wheel(CarShape::WheelName::RR).steering, 0.0f);
}

//--------------------------------------------------------------------------
TEST(TestCarKinematic, Constructor)
{
    CarShape shape(CarDimensions::get("Renault.Twingo"));
    CarKinematic c(shape);

    ASSERT_FLOAT_EQ(c.acceleration(), 0.0f);
    ASSERT_FLOAT_EQ(c.speed(), 0.0f);
    ASSERT_FLOAT_EQ(c.position().x, 0.0f);
    ASSERT_FLOAT_EQ(c.position().y, 0.0f);
    ASSERT_FLOAT_EQ(c.heading(), 0.0f);

    ASSERT_EQ(c.shape().wheels().size(), 4u);
    ASSERT_FLOAT_EQ(c.shape().wheel(CarShape::WheelName::FL).position.x, 0.0f);
    ASSERT_FLOAT_EQ(c.shape().wheel(CarShape::WheelName::FL).position.y, 0.0f);
    ASSERT_FLOAT_EQ(c.shape().wheel(CarShape::WheelName::FR).position.x, 0.0f);
    ASSERT_FLOAT_EQ(c.shape().wheel(CarShape::WheelName::FR).position.y, 0.0f);
    ASSERT_FLOAT_EQ(c.shape().wheel(CarShape::WheelName::RL).position.x, 0.0f);
    ASSERT_FLOAT_EQ(c.shape().wheel(CarShape::WheelName::RL).position.y, 0.0f);
    ASSERT_FLOAT_EQ(c.shape().wheel(CarShape::WheelName::RR).position.x, 0.0f);
    ASSERT_FLOAT_EQ(c.shape().wheel(CarShape::WheelName::RR).position.y, 0.0f);
    ASSERT_FLOAT_EQ(c.shape().wheel(CarShape::WheelName::FL).steering, 0.0f);
    ASSERT_FLOAT_EQ(c.shape().wheel(CarShape::WheelName::FR).steering, 0.0f);
    ASSERT_FLOAT_EQ(c.shape().wheel(CarShape::WheelName::RL).steering, 0.0f);
    ASSERT_FLOAT_EQ(c.shape().wheel(CarShape::WheelName::RR).steering, 0.0f);
}

//--------------------------------------------------------------------------
TEST(TestCarKinematic, Init)
{
    CarShape shape(CarDimensions::get("Renault.Twingo"));
    CarKinematic c(shape);
    c.init(sf::Vector2f(100.0f, 50.0f), 2.0f, 3.0f, 4.0f);

    ASSERT_FLOAT_EQ(c.acceleration(), 0.0f);
    ASSERT_FLOAT_EQ(c.speed(), 3.0f);
    ASSERT_FLOAT_EQ(c.position().x, 100.0f);
    ASSERT_FLOAT_EQ(c.position().y, 50.0f);
    ASSERT_FLOAT_EQ(c.heading(), 2.0f);

    ASSERT_EQ(c.shape().wheels().size(), 4u);
    ASSERT_FLOAT_EQ(c.shape().wheel(CarShape::WheelName::FL).position.x, 99.665848f);
    ASSERT_FLOAT_EQ(c.shape().wheel(CarShape::WheelName::FL).position.y, 52.58765f);
    ASSERT_FLOAT_EQ(c.shape().wheel(CarShape::WheelName::FR).position.x, 98.260078f);
    ASSERT_FLOAT_EQ(c.shape().wheel(CarShape::WheelName::FR).position.y, 51.944286f);
    ASSERT_FLOAT_EQ(c.shape().wheel(CarShape::WheelName::RL).position.x, 100.70289f);
    ASSERT_FLOAT_EQ(c.shape().wheel(CarShape::WheelName::RL).position.y, 50.321682f);
    ASSERT_FLOAT_EQ(c.shape().wheel(CarShape::WheelName::RR).position.x, 99.297112f);
    ASSERT_FLOAT_EQ(c.shape().wheel(CarShape::WheelName::RR).position.y, 49.678318f);
    ASSERT_FLOAT_EQ(c.shape().wheel(CarShape::WheelName::FL).steering, 4.0f);
    ASSERT_FLOAT_EQ(c.shape().wheel(CarShape::WheelName::FR).steering, 4.0f);
    ASSERT_FLOAT_EQ(c.shape().wheel(CarShape::WheelName::RL).steering, 0.0f);
    ASSERT_FLOAT_EQ(c.shape().wheel(CarShape::WheelName::RR).steering, 0.0f);
}

//--------------------------------------------------------------------------
TEST(TestCar, Constructor)
{
    Car c("Renault.Twingo");

    ASSERT_FLOAT_EQ(c.acceleration(), 0.0f);
    ASSERT_FLOAT_EQ(c.speed(), 0.0f);
    ASSERT_FLOAT_EQ(c.position().x, 0.0f);
    ASSERT_FLOAT_EQ(c.position().y, 0.0f);
    ASSERT_FLOAT_EQ(c.heading(), 0.0f);
    ASSERT_EQ(c.wheels().size(), 4u);
    ASSERT_FLOAT_EQ(c.wheel(CarShape::WheelName::FL).position.x, 0.0f);
    ASSERT_FLOAT_EQ(c.wheel(CarShape::WheelName::FL).position.y, 0.0f);
    ASSERT_FLOAT_EQ(c.wheel(CarShape::WheelName::FL).steering, 0.0f);
    /*ASSERT_FLOAT_EQ(c.wheel(1).position().x, 0.0f);
    ASSERT_FLOAT_EQ(c.wheel(1).position().y, 0.0f);
    ASSERT_FLOAT_EQ(c.wheel(2).position().x, 0.0f);
    ASSERT_FLOAT_EQ(c.wheel(2).position().y, 0.0f);
    ASSERT_FLOAT_EQ(c.wheel(3).position().x, 0.0f);
    ASSERT_FLOAT_EQ(c.wheel(3).position().y, 0.0f);*/
}

//--------------------------------------------------------------------------
TEST(TestCar, Init)
{
    Car c("Renault.Twingo");
    c.init(sf::Vector2f(100.0f, 50.0f), 1.0f, 2.0f, 3.0f);

    ASSERT_FLOAT_EQ(c.acceleration(), 0.0f);
    ASSERT_FLOAT_EQ(c.speed(), 2.0f);
    ASSERT_FLOAT_EQ(c.position().x, 100.0f);
    ASSERT_FLOAT_EQ(c.position().y, 50.0f);
    ASSERT_FLOAT_EQ(c.heading(), 1.0f);
    ASSERT_EQ(c.wheels().size(), 4u);
    ASSERT_FLOAT_EQ(c.wheel(CarShape::WheelName::FL).position.x, 101.99689f);
    ASSERT_FLOAT_EQ(c.wheel(CarShape::WheelName::FL).position.y, 51.6793f);
    ASSERT_FLOAT_EQ(c.wheel(CarShape::WheelName::FL).steering, 3.0f);
}
