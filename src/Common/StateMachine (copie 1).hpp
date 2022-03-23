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

#ifndef STATE_MACHINE_HPP
#  define STATE_MACHINE_HPP

#  include <array>
#  include <map>
#  include <iostream>

// *****************************************************************************
//! \brief
// *****************************************************************************
template<typename FSM, class STATES_ID>
class StateMachine
{
public:

    using bFuncPtr = bool (*)(FSM&);
    using xFuncPtr = void (*)(FSM&);

    //--------------------------------------------------------------------------
    //! \brief TODO complete with entering, leaving ...
    //--------------------------------------------------------------------------
    struct State
    {
        xFuncPtr action = nullptr;
    };

    using States = std::array<State, STATES_ID::MAX_STATES>;
    using Transitions = std::map<STATES_ID, STATES_ID>;

    //--------------------------------------------------------------------------
    //! \brief
    //--------------------------------------------------------------------------
    StateMachine(STATES_ID const initial)
        : m_initial_state(initial), m_current_state(initial)
    {}

    //--------------------------------------------------------------------------
    //! \brief
    //--------------------------------------------------------------------------
    virtual ~StateMachine() = default;

    //--------------------------------------------------------------------------
    //! \brief Return the current state.
    //--------------------------------------------------------------------------
    inline STATES_ID state() const
    {
        return m_current_state;
    }

    //--------------------------------------------------------------------------
    //! \brief Restore to initial states.
    //--------------------------------------------------------------------------
    inline void reset()
    {
        m_current_state = m_initial_state;
    }

    //#if !defined(NDEBUG)
    //--------------------------------------------------------------------------
    //! \brief Return the current state as string (shall not be free'ed).
    //--------------------------------------------------------------------------
    inline const char* c_str() const
    {
        return stringify(m_current_state);
    }
    //#endif

    //--------------------------------------------------------------------------
    //! \brief Internal transition: jump to the desired state. This will call
    //! the guard, leaving actions, entering actions ...
    //--------------------------------------------------------------------------
    void transit(STATES_ID const new_state);

protected:

    //--------------------------------------------------------------------------
    //! \brief From current state, jump to the destination state.
    //--------------------------------------------------------------------------
    void react(Transitions const& transitions)
    {
        auto const& it = transitions.find(m_current_state);
        transit(it != transitions.end() ? it->second : STATES_ID::IGNORING_EVENT);
    }

    //#if !defined(NDEBUG)
    //--------------------------------------------------------------------------
    //! \brief Return the given state as string (shall not be free'ed).
    //! \note implement this method in derived class.
    //--------------------------------------------------------------------------
    virtual const char* stringify(STATES_ID const state) const = 0;
    //#endif

protected:

    //! \brief Save initial state
    STATES_ID m_initial_state;

    //! \brief Current active state
    STATES_ID m_current_state;

    //! \brief Container of states
    States m_states;
};

//------------------------------------------------------------------------------
template<class FSM, class STATES_ID>
void StateMachine<FSM, STATES_ID>::transit(STATES_ID const new_state) // FIXME std::Any
{
    // Unknown state
    if (new_state >= STATES_ID::MAX_STATES)
    {
        std::cerr << "[STATE MACHINE] Unknown state. Aborting!" << std::endl;
        exit(1);
    }
    // Trigger the fault
    else if (new_state == STATES_ID::CANNOT_HAPPEN)
    {
        std::cerr << "[STATE MACHINE] Forbidden state. Aborting!" << std::endl;
        exit(1);
    }
    // Do not react to the event
    else if (new_state == STATES_ID::IGNORING_EVENT)
    {
        std::cout << "[STATE MACHINE] Ignoring external event" << std::endl;
        return ;
    }
    // React to new event
    else
    {
        std::cout << "[STATE MACHINE] Transitioning to state "
                  << stringify(new_state) << std::endl;

        // Call the function associated to the current state
        StateMachine<FSM, STATES_ID>::State const& state = m_states[new_state];
        (*state.action)(*reinterpret_cast<FSM*>(this));

        // Transition to new state
        m_current_state = new_state;
    }
}

#endif
