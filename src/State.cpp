#include "State.hpp"
#include "StateMachine.hpp"
#include "Transition.hpp"

namespace state_machine
{



State::State(const std::string &name_) : State(name_, nullptr, nullptr)
{
}

State::State(const std::string& name_, State* success) : State(name_, success, nullptr)
{
}

State::State(const std::string& name_, State* success, State* failue) : id(StateMachine::getInstance().getNewStateId()), msg(StateMachine::getInstance().getDebugStream()), 
                                         isFinished(false), hasFailed(false),  successState(success), failureState(failue), destroyOnExit(false), 
                                         name(name_)
{
    StateMachine::getInstance().registerState(this);
    
    if(successState)
    {
        addEdge("Success", successState, [&](){return finished();});
    }

    if(failureState)
    {
        addEdge("Failed", failureState, [&](){return failed();});
    }
}

void State::enterExt(const State* lastState)
{
    hasFailed = false;
    isFinished = false;
    enter(lastState);
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
void State::fail()
{
    hasFailed = true;
}

bool State::failed() const
{
    return hasFailed;
}

void State::finish()
{
    isFinished = true;
}

bool State::finished() const
{
    return isFinished;
}



Transition *State::addEdge(const std::string &name, State* next, std::function<bool()> guard) 
{
    Transition *tr = new Transition(name, this, next, guard);
    this->transitions.push_back(tr);
    return tr;
}

}