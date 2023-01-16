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

#include "gmock/gmock.h"
#include "gtest/gtest.h"

#define protected public
#define private public
#  pragma GCC diagnostic push
#  pragma GCC diagnostic ignored "-Wfloat-equal"
#    include "Vehicle/Vehicle.hpp"
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
    ASSERT_FLOAT_EQ(c.wheel(CarShape::WheelName::FR).position.x, 99.665848f);
    ASSERT_FLOAT_EQ(c.wheel(CarShape::WheelName::FR).position.y, 52.58765f);
    ASSERT_FLOAT_EQ(c.wheel(CarShape::WheelName::FL).position.x, 98.260078f);
    ASSERT_FLOAT_EQ(c.wheel(CarShape::WheelName::FL).position.y, 51.944286f);
    ASSERT_FLOAT_EQ(c.wheel(CarShape::WheelName::RR).position.x, 100.70289f);
    ASSERT_FLOAT_EQ(c.wheel(CarShape::WheelName::RR).position.y, 50.321682f);
    ASSERT_FLOAT_EQ(c.wheel(CarShape::WheelName::RL).position.x, 99.297112f);
    ASSERT_FLOAT_EQ(c.wheel(CarShape::WheelName::RL).position.y, 49.678318f);
    ASSERT_FLOAT_EQ(c.wheel(CarShape::WheelName::FR).steering, 3.0f);
    ASSERT_FLOAT_EQ(c.wheel(CarShape::WheelName::FL).steering, 3.0f);
    ASSERT_FLOAT_EQ(c.wheel(CarShape::WheelName::RR).steering, 0.0f);
    ASSERT_FLOAT_EQ(c.wheel(CarShape::WheelName::RL).steering, 0.0f);
}

//--------------------------------------------------------------------------
TEST(TestTricycleKinematic, Constructor)
{
    CarShape shape(CarDimensions::get("Renault.Twingo"));
    TricycleKinematic c("trycicle", shape);

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
TEST(TestTricycleKinematic, Init)
{
    CarShape shape(CarDimensions::get("Renault.Twingo"));
    TricycleKinematic c("trycicle", shape);
    c.init(sf::Vector2f(100.0f, 50.0f), 2.0f, 3.0f, 4.0f);

    ASSERT_FLOAT_EQ(c.acceleration(), 0.0f);
    ASSERT_FLOAT_EQ(c.speed(), 3.0f);
    ASSERT_FLOAT_EQ(c.position().x, 100.0f);
    ASSERT_FLOAT_EQ(c.position().y, 50.0f);
    ASSERT_FLOAT_EQ(c.heading(), 2.0f);

    ASSERT_EQ(c.shape().wheels().size(), 4u);
    ASSERT_FLOAT_EQ(c.shape().wheel(CarShape::WheelName::FR).position.x, 99.665848f);
    ASSERT_FLOAT_EQ(c.shape().wheel(CarShape::WheelName::FR).position.y, 52.58765f);
    ASSERT_FLOAT_EQ(c.shape().wheel(CarShape::WheelName::FL).position.x, 98.260078f);
    ASSERT_FLOAT_EQ(c.shape().wheel(CarShape::WheelName::FL).position.y, 51.944286f);
    ASSERT_FLOAT_EQ(c.shape().wheel(CarShape::WheelName::RR).position.x, 100.70289f);
    ASSERT_FLOAT_EQ(c.shape().wheel(CarShape::WheelName::RR).position.y, 50.321682f);
    ASSERT_FLOAT_EQ(c.shape().wheel(CarShape::WheelName::RL).position.x, 99.297112f);
    ASSERT_FLOAT_EQ(c.shape().wheel(CarShape::WheelName::RL).position.y, 49.678318f);
    ASSERT_FLOAT_EQ(c.shape().wheel(CarShape::WheelName::FR).steering, 4.0f);
    ASSERT_FLOAT_EQ(c.shape().wheel(CarShape::WheelName::FL).steering, 4.0f);
    ASSERT_FLOAT_EQ(c.shape().wheel(CarShape::WheelName::RR).steering, 0.0f);
    ASSERT_FLOAT_EQ(c.shape().wheel(CarShape::WheelName::RL).steering, 0.0f);
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
    ASSERT_FLOAT_EQ(c.wheel(CarShape::WheelName::FR).position.x, 101.99689f);
    ASSERT_FLOAT_EQ(c.wheel(CarShape::WheelName::FR).position.y, 51.6793f);
    ASSERT_FLOAT_EQ(c.wheel(CarShape::WheelName::FR).steering, 3.0f);
}
