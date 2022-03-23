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
#  include <cstdio>

#  if defined(NDEBUG)
#    define DEBUG(...)
#  else
#    define DEBUG printf
#  endif

// *****************************************************************************
//! \brief
// *****************************************************************************
template<typename FSM, class STATES_ID>
class StateMachine
{
public:

    using bFuncPtr = bool (FSM::*)();
    using xFuncPtr = void (FSM::*)();

    //--------------------------------------------------------------------------
    //! \brief
    //--------------------------------------------------------------------------
    struct State
    {
        bFuncPtr guard = nullptr;
        xFuncPtr entering = nullptr;
        xFuncPtr leaving = nullptr;
        xFuncPtr onevent = nullptr;
    };

    using States = std::array<State, STATES_ID::MAX_STATES>;
    using Transitions = std::map<STATES_ID, STATES_ID>;

    //--------------------------------------------------------------------------
    //! \brief
    //--------------------------------------------------------------------------
    StateMachine(STATES_ID const initial)
        : m_current_state(initial), m_initial_state(initial)
    {}

    //--------------------------------------------------------------------------
    //! \brief
    //--------------------------------------------------------------------------
    virtual ~StateMachine() = default;

    //--------------------------------------------------------------------------
    //! \brief Restore to initial states.
    //--------------------------------------------------------------------------
    inline void reset()
    {
        m_current_state = m_initial_state;
        m_nesting_state = STATES_ID::CANNOT_HAPPEN;
        m_nesting = false;
    }

    //--------------------------------------------------------------------------
    //! \brief Return the current state.
    //--------------------------------------------------------------------------
    inline STATES_ID state() const
    {
        return m_current_state;
    }

    //--------------------------------------------------------------------------
    //! \brief Return the current state as string (shall not be free'ed).
    //--------------------------------------------------------------------------
    inline const char* c_str() const
    {
        return stringify(m_current_state);
    }

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

private:

    //--------------------------------------------------------------------------
    //! \brief Return the given state as string (shall not be free'ed).
    //! \note implement this method in derived class.
    //--------------------------------------------------------------------------
    virtual const char* stringify(STATES_ID const state) const = 0;

protected:

    //! \brief Container of states
    States m_states;

    //! \brief Current active state
    STATES_ID m_current_state;

private:

    //! \brief Save initial state
    STATES_ID m_initial_state;

    STATES_ID m_next_state;
    STATES_ID m_nesting_state = STATES_ID::CANNOT_HAPPEN;
    bool m_nesting = false;
};

//------------------------------------------------------------------------------
template<class FSM, class STATES_ID> // FIXME use new version
void StateMachine<FSM, STATES_ID>::transit(STATES_ID const new_state) // FIXME std::any
{
    DEBUG("[STATE MACHINE] Reacting to event from state %s\n",
          stringify(m_current_state));

#if defined(THREAD_SAFETY)
    // If try_lock failed it is not important: it just means that we have called
    // an internal event from this method and internal states are still
    // protected.
    m_mutex.try_lock();
#endif

    m_nesting_state = new_state;

    // Reaction from internal event (therefore coming from this method called by
    // one of the action functions: memorize and leave the function: it will
    // continue thank to the while loop. This avoids recursion.
    if (m_nesting)
    {
        DEBUG("[STATE MACHINE] Internal event. Memorize state %s\n",
              stringify(new_state));
        return ;
    }

    do
    {
        m_next_state = m_nesting_state;
        m_nesting_state = STATES_ID::CANNOT_HAPPEN;

        // Forbidden event: kill the system
        if (m_next_state == STATES_ID::CANNOT_HAPPEN)
        {
            std::cerr << "[STATE MACHINE] Forbidden event. Aborting!"
                      << std::endl;
            exit(1);
        }

        // Do not react to this event
        else if (m_next_state == STATES_ID::IGNORING_EVENT)
        {
            DEBUG("[STATE MACHINE] Ignoring external event\n");
            return ;
        }

        // Unknown state: kill the system
        else if (m_next_state >= STATES_ID::MAX_STATES)
        {
            std::cerr << "[STATE MACHINE] Unknown state. Aborting!"
                      << std::endl;
            exit(1);
        }

        // Transition to new new state. Local variable mandatory since state
        // reactions can modify current state (side effect).
        STATES_ID current_state = m_current_state;
        m_current_state = m_next_state;

        // Reaction: call the member function associated to the current state
        StateMachine<FSM, STATES_ID>::State const& nst = m_states[m_next_state];
        StateMachine<FSM, STATES_ID>::State const& cst = m_states[current_state];

        // Call the guard
        m_nesting = true;
        bool guard_res = (nst.guard == nullptr);
        if (!guard_res)
        {
            guard_res = (reinterpret_cast<FSM*>(this)->*nst.guard)();
        }

        if (!guard_res)
        {
            DEBUG("[STATE MACHINE] Transition refused by the %s guard. Stay"
                  " in state %s\n", stringify(new_state), stringify(current_state));
            m_current_state = current_state;
            m_nesting = false;
            return ;
        }
        else
        {
            // The guard allowed the transition to the next state
            DEBUG("[STATE MACHINE] Transitioning to new state %s\n",
                  stringify(new_state));

            // Entry and leaving actions are not made if the event specified by
            // the "on event" clause happened.
            if (cst.onevent != nullptr)
            {
                DEBUG("[STATE MACHINE] Do the %s 'on event' actions\n",
                      stringify(current_state));
                (reinterpret_cast<FSM*>(this)->*cst.onevent)();
                return ;
            }

            // Transitioning to a new state?
            else if (current_state != m_next_state)
            {
                if (cst.leaving != nullptr)
                {
                    DEBUG("[STATE MACHINE] State %s 'leaving' actions\n",
                          stringify(current_state));

                    // Do reactions when leaving the current state
                    (reinterpret_cast<FSM*>(this)->*cst.leaving)();
                }

                if (nst.entering != nullptr)
                {
                    DEBUG("[STATE MACHINE] State %s 'entry' actions\n",
                          stringify(new_state));

                    // Do reactions when entring into the new state
                    (reinterpret_cast<FSM*>(this)->*nst.entering)();
                }
            }
            else
            {
                DEBUG("[STATE MACHINE] Was previously in this mode: no "
                      "actions to perform\n");
            }
        }
        m_nesting = false;
    }
    while (m_nesting_state != STATES_ID::CANNOT_HAPPEN);

#if defined(THREAD_SAFETY)
    m_mutex.unlock();
#endif
}

#endif // STATE_MACHINE_HPP
