#include "Events.hpp"
#include "State.hpp"

state_machine::serialization::State::State(const state_machine::State& state)
{
    name = state.getName();
    id = state.getId();
}

state_machine::serialization::Transition::Transition(const state_machine::Transition& trans)
{
    if(trans.next)
    {
        to = State(*(trans.next));
    }
    
    if(trans.prev)
    {
        from = State(*(trans.prev));
    }
    
    id = trans.getId();
}

state_machine::serialization::StateMachine::StateMachine(const state_machine::StateMachine& sm)
{
    //dump state machine 
    for(auto const &p: sm.getAllStates())
    {
        State state(*(p.first));
        allStates.push_back(state);
        for(auto const &t:p.first->getTransitions())
        {
            //filter incomplete/inactive transitions
            if(t->prev && t->next)
            {
                Transition tr(*t);
                allTransitions.push_back(tr);
            }
        }
    }
}