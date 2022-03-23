#include "qq.hpp"
#include <iostream>

// -----------------------------------------------------------------------------
static void enteringStateIdle(MotorControl& mc)
{
    std::cout << "Entering in state Idle: " << mc.refspeed() << std::endl;
}

// -----------------------------------------------------------------------------
static void enteringStateStop(MotorControl& mc)
{
    std::cout << "Entering in state Stop: " << mc.refspeed() << std::endl;
    mc.transit(MotorStateID::IDLE);
}

// -----------------------------------------------------------------------------
static void enteringStateStart(MotorControl& mc)
{
    std::cout << "Entering in state Start: " << mc.refspeed() << std::endl;
}

// -----------------------------------------------------------------------------
static void enteringStateSpinning(MotorControl& mc)
{
    std::cout << "Entering in state Spinning: " << mc.refspeed() << std::endl;
}

// -----------------------------------------------------------------------------
MotorControl::MotorControl()
    : StateMachine(MotorStateID::IDLE)
{
    m_states[MotorStateID::IDLE].entering = enteringStateIdle;
    m_states[MotorStateID::STOPPING].entering = enteringStateStop;
    m_states[MotorStateID::STARTING].entering = enteringStateStart;
    m_states[MotorStateID::SPINNING].entering = enteringStateSpinning;
}

// -----------------------------------------------------------------------------
void MotorControl::halt()
{
    m_refspeed = 0.0f;
    std::cout << "Halt event" << std::endl;

    static Transitions s_transitions = {
        { MotorStateID::STARTING, MotorStateID::STOPPING },
        { MotorStateID::SPINNING, MotorStateID::STOPPING },
    };

    react(s_transitions);
}

// -----------------------------------------------------------------------------
void MotorControl::refspeed(int const value)
{
    m_refspeed = value;
    std::cout << "New reference speed event: " << value << std::endl;

    static Transitions s_transitions = {
        { MotorStateID::IDLE, MotorStateID::STARTING },
        { MotorStateID::STARTING, MotorStateID::SPINNING },
        { MotorStateID::SPINNING, MotorStateID::SPINNING },
        { MotorStateID::STOPPING, MotorStateID::CANNOT_HAPPEN },
    };

    react(s_transitions);
}

// -----------------------------------------------------------------------------
const char* MotorControl::stringify(MotorStateID const state) const
{
    static const char* s_states[] = {
        [MotorStateID::IDLE] = "IDLE",
        [MotorStateID::STARTING] = "STARTING",
        [MotorStateID::SPINNING] = "SPINNING",
        [MotorStateID::STOPPING] = "STOPPING",
    };

    return s_states[state];
}

// -----------------------------------------------------------------------------
int main()
{
    MotorControl mc;
    std::cout << mc.state() << std::endl;

    mc.refspeed(100);
    std::cout << mc.state() << std::endl;

    mc.refspeed(200);
    std::cout << mc.state() << std::endl;

    mc.refspeed(300);
    std::cout << mc.state() << std::endl;

    mc.halt();
    std::cout << mc.state() << std::endl;

    return 0;
}
