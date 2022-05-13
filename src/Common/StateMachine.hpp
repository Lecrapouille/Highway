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

#  include <map>
#  include <iostream>
#  include <cstdio>
#  include <cassert>

#  if defined(NDEBUG)
#    define DEBUG(...)
#  else
#    define DEBUG printf
#  endif

//-----------------------------------------------------------------------------
//! \brief Return the given state as string (shall not be free'ed).
//! \note implement this method in derived class.
//-----------------------------------------------------------------------------
template<class STATES_ID>
const char* stringify(STATES_ID const state);

// *****************************************************************************
//! \brief Base class of the structure depicting small Finite State Machine
//! (FSM) and running them. This class is not made for defining hierarchical
//! state machine (HSM). See this document for more information about FSM:
//! http://niedercorn.free.fr/iris/iris1/uml/uml09.pdf
//!
//! This class holds the list of states \c State and the current active state.
//! Each of state holds  actions to perform, as function pointers (such as
//! 'on entering', 'on leaving' ...). This class does not hold directly tables
//! for transitioning origin state to destination state when an external event
//! occured. Instead each external event shall be implemented as member function
//! in the derived FSM class and in each member function shall implement this
//! table. This is a compromise to minimize the number of virtual methods.
//!
//! Example: the following state machine, in plantuml syntax:
//! @startuml
//! [*] --> Idle
//! Idle --> Starting : set speed
//! Starting --> Stopping : halt
//! Starting -> Spinning : set speed
//! Spinning -> Stopping: halt
//! Spinning --> Spinning : set speed
//! Stopping -> Idle
//! @enduml
//!
//! A FSM can be depicted by a graph (nodes: states; arcs: transitions) which can
//! be represented by table:
//!
//! +-----------------+------------+-----------+-----------+
//! | States \\ Event | Set Speed  | Halt      | --        |
//! +=================+============+===========+===========+
//! | IDLE            | STARTING   |           |           |
//! +-----------------+------------+-----------+-----------+
//! | STOPPING        |            |           | IDLE      |
//! +-----------------+------------+-----------+-----------+
//! | STARTING        | SPINNING   | STOPPING  |           |
//! +-----------------+------------+-----------+-----------+
//! | SPINNING        | SPINNING   | STOPPING  |           |
//! +-----------------+------------+-----------+-----------+
//!
//! The first column contains all states. The first line contains all events.
//! Each column depict a transition: given the current state (i.e. IDLE) and a
//! given event (i.e. Set Speed) the next state of the state machine will be
//! STARTING. Empty cells are forbiddent transitions. Usually the table is
//! sparse.
//!
//! \tparam FSM the concrete Finite State Machine deriving from this base class.
//! \tparam STATES_ID enumerate for giving an unique identifier for each state.
//!
//! In this class, states are enums \c STATES_ID like: enum StatesID
//! { IDLE = 0, STOPPING, STARTING, SPINNING, MAX_STATES }; and the table of
//! states \c m_states shall be filled with these enums and pointer functions
//! such as 'on entering' set. Each event (i.e. Set Speed) shall be a method
//! having a static table of state transition.
//!
//! This class is fine for small Finite State Machine (FSM) and is limited due
//! to memory footprint (therefore no complex C++ designs, no dynamic containers
//! and few virtual methods). The code is based on the following link
//! https://www.codeproject.com/Articles/1087619/State-Machine-Design-in-Cplusplus-2
//! For bigger state machines, please use something more robust such as Esterel
//! SyncCharts.
// *****************************************************************************
template<typename FSM, class STATES_ID>
class StateMachine
{
public:

    //! \brief Pointer function with no argument and returning a boolean.
    using bFuncPtr = bool (FSM::*)();
    //! \brief Pointer function with no argument and returning void.
    using xFuncPtr = void (FSM::*)();

    //--------------------------------------------------------------------------
    //! \brief Class depicting a state of the state machine and hold pointer
    //! functions for each desired action to perform.
    //--------------------------------------------------------------------------
    struct State
    {
        //! \brief Call the guard fonction validating the event if return true
        //! or set to nullptr. It invalidate the event if return false.
        bFuncPtr guard = nullptr;
        //! \brief Call the "on entry" fonction when entering for the first time
        //! (AND ONLY WHEN ONCE) in the state. Note: the guard prevent this
        //! function.
        xFuncPtr entering = nullptr;
        //! \brief Call the "on leaving" fonction when leavinging for the first
        //! time (AND ONLY WHEN ONCE) the state. Note: the guard prevent this
        //! function.
        xFuncPtr leaving = nullptr;
        //! \brief Call the "on event" fonction when the event occured. Note:
        //! the guard prevent this function. Entry and leaving actions are not
        //! made if this function is not nullptr and return true.
        xFuncPtr onevent = nullptr;
    };

    //! \brief Define the type of container holding all stated of the state
    //! machine.
    using States = std::array<State, STATES_ID::MAX_STATES>;
    //! \brief Define the type of container holding states transitions. Since
    //! a state machine is generally a sparse matrix we use red-back tree.
    using Transitions = std::map<STATES_ID, STATES_ID>;

    //--------------------------------------------------------------------------
    //! \brief Default constructor. Pass the number of states the FSM will use,
    //! set the initial state and if mutex shall have to be used.
    //! \param[in] initial the initial state to start with.
    //--------------------------------------------------------------------------
    StateMachine(STATES_ID const initial) // FIXME should be ok for constexpr
        : m_current_state(initial), m_initial_state(initial)
    {
        assert(initial < STATES_ID::MAX_STATES); // FIXME static_assert not working
    }

    //--------------------------------------------------------------------------
    //! \brief Restore the state machin to its initial state.
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
    //! \brief Internal transition: jump to the desired state from internal event.
    //! This will call the guard, leaving actions, entering actions ...
    //! \param[in] new_state the destination state.
    //--------------------------------------------------------------------------
    void transition(STATES_ID const new_state);

protected:

    //--------------------------------------------------------------------------
    //! \brief From current state, jump to the destination state from external
    //! event.
    //! \param[in] transitions the table of transitions.
    //--------------------------------------------------------------------------
    void transition(Transitions const& transitions)
    {
        auto const& it = transitions.find(m_current_state);
        transition(it != transitions.end() ? it->second : STATES_ID::IGNORING_EVENT);
    }

protected:

    //! \brief Container of states.
    States m_states;

    //! \brief Current active state.
    STATES_ID m_current_state;

private:

    //! \brief Save the initial state need for restoring initial state.
    STATES_ID m_initial_state;
    //! \brief Temporary variable saving the next state.
    STATES_ID m_next_state;
    //! \brief Temporary variable saving the nesting state (needed for internal
    //! event).
    STATES_ID m_nesting_state = STATES_ID::CANNOT_HAPPEN;
    //! \brief is the state nested by an internal event.
    bool m_nesting = false;
};

//------------------------------------------------------------------------------
template<class FSM, class STATES_ID> // FIXME use new version
void StateMachine<FSM, STATES_ID>::transition(STATES_ID const new_state) // FIXME std::any
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
            exit(EXIT_FAILURE);
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
            exit(EXIT_FAILURE);
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
                DEBUG("[STATE MACHINE] Do the state %s 'on event' action\n",
                      stringify(current_state));
                (reinterpret_cast<FSM*>(this)->*cst.onevent)();
                return ;
            }

            // Transitioning to a new state ?
            else if (current_state != m_next_state)
            {
                if (cst.leaving != nullptr)
                {
                    DEBUG("[STATE MACHINE] Do the state %s 'on leaving' action\n",
                          stringify(current_state));

                    // Do reactions when leaving the current state
                    (reinterpret_cast<FSM*>(this)->*cst.leaving)();
                }

                if (nst.entering != nullptr)
                {
                    DEBUG("[STATE MACHINE] Do the state %s 'on entry' action\n",
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
