#ifndef QQ_HPP
#define QQ_HPP

#include "StateMachine.hpp"

enum MotorStateID { IDLE, STARTING, SPINNING, STOPPING, IGNORING_EVENT, CANNOT_HAPPEN, MAX_STATES };

// *****************************************************************************
//! \brief
// *****************************************************************************
class MotorControl : public StateMachine<MotorControl, MotorStateID>
{
public:

    MotorControl();
    void halt();
    void refspeed(int const value);
    inline float refspeed() const
    {
        return m_refspeed;
    }

protected:

    virtual const char* stringify(MotorStateID const state) const override;

private:

    float m_refspeed = 0.0f;
};

#endif
