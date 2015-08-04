#include "State.hpp"
#include "StateMachine.hpp"
#include "Transition.hpp"

namespace state_machine
{



State::State(const std::string &name_) : id(StateMachine::getInstance().getNewStateId()), msg(StateMachine::getInstance().getDebugStream()), destroyOnExit(false), name(name_)
{
    StateMachine::getInstance().registerState(this);
}

const std::string& State::getName() const
{
    return name;
}

bool State::autoDestroy()
{
    return destroyOnExit;
}

void State::autoDestroy(bool deleteIt)
{
    destroyOnExit = deleteIt;
}

void State::registerSubState(State* subState)
{
    State::SubState sub;
    sub.state = subState;
    sub.toSubState = addEdge("To" + subState->getName(), subState, [](){return false;});
    subStates.push_back(sub);
}

Transition* State::checkTransitions() const
{
    for (Transition* transition : transitions) {
        if (transition->guard()){
            return transition;
        }
    }
    return nullptr;
}

void State::executeSubState(State *subState)
{
    StateMachine::getInstance().executeSubState(subState);
}

Transition *State::addEdge(const std::string &name, State* next, std::function<bool()> guard) 
{
    Transition *tr = new Transition(name, this, next, guard);
    this->transitions.push_back(tr);
    return tr;
}

}