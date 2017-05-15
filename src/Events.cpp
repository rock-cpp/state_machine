#include "Events.hpp"
#include "State.hpp"
#include "Transition.hpp"
#include "StateMachine.hpp"

bool state_machine::serialization::Serializable::isValid() const
{
    return std::numeric_limits<unsigned int>::max() == id;
}

state_machine::serialization::State::State(const state_machine::State& state)
{
    name = "    " + state.getName() + "    ";
    id = state.getId();
    if (!state.getParentState()) {
        parentId = id;
    } else {
        parentId = state.getParentState()->getId();
    }
}

bool state_machine::serialization::State::operator==(const state_machine::serialization::State& other) const
{
    return (name == other.name) && (other.id == id) && (parentId == other.parentId);
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
    
    name = "    " + trans.getName() + "    ";
    
    id = trans.getId();
}

bool state_machine::serialization::Transition::operator==(const state_machine::serialization::Transition& other) const
{
    return (other.id == id) && (other.name == name) 
           && (other.to == to) && (other.from == from);
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
