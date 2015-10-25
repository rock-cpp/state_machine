#include "State.hpp"
#include "StateMachine.hpp"
#include "Transition.hpp"

namespace state_machine
{



State::State(const std::string &name_) : State(name_, nullptr, nullptr)
{
    parentState = this;
    isPreemptable = false;
}

State::State(const std::string& name_, State* success) : State(name_, success, nullptr)
{
    parentState = this;
    isPreemptable = false;
}

State::State(const std::string& name_, State* success, State* failue) : id(StateMachine::getInstance().getNewStateId()), msg(StateMachine::getInstance().getDebugStream()), 
                                         isFinished(false), hasFailed(false),  successState(success), failureState(failue), destroyOnExit(false), 
                                         name(name_)
{
    parentState = this;
    isPreemptable = false;
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
    subState->setParentState(this->getParentState());
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

bool State::executeSubState(State *subState)
{
    return StateMachine::getInstance().executeSubState(subState);
}

bool State::executeSubState(State& subState)
{
    return StateMachine::getInstance().executeSubState(&subState);
}

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

const State* State::getParentState() const
{
    if(!parentState) 
    {
        return this;
    } 
    else
    {
        return parentState;
    }
    
}

void State::setParentState(const State* state)
{
    parentState = state;
}



Transition *State::addEdge(const std::string &name, State* next, std::function<bool()> guard) 
{
    Transition *tr = new Transition(name, this, next, guard);
    this->transitions.push_back(tr);
    return tr;
}

Transition* State::deleteEdge(const std::string& name)
{
    for (unsigned i = 0; i < transitions.size(); i++) {
        if (transitions[i]->getName().compare(name) == 0) {
            Transition* oldTr = transitions[i];
            transitions.erase(transitions.begin() + i);
            return oldTr;
        }
    }
    return nullptr;
}

void State::deRegisterSubState(State *subState)
{
    for (unsigned i = 0; i < subStates.size(); i++) {
        if (subStates[i].state->getId() == subState->getId()) {
            subStates.erase(subStates.begin() + i);
            return;
        }
    }
    return;
}



bool State::getIsPreemptable()
{
    return isPreemptable;
}

bool State::preemptionHook()
{
    return false;
}

State* State::getFailureState()
{
    return failureState;
}

State* State::getSuccessState()
{
    return successState;
}



}