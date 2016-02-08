#include "State.hpp"
#include "StateMachine.hpp"
#include "Transition.hpp"


namespace state_machine
{
 

State::State(const std::string& name_, State* success, State* failue) : id(StateMachine::getInstance().getNewStateId()), msg(StateMachine::getInstance().getDebugStream()), 
                                         isFinished(false), hasFailed(false),  successState(success), failureState(failue), destroyOnExit(false), 
                                         name(name_)
{
    parentState = this;
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

Transition* State::registerSubState(State* subState)
{
    subState->setParentState(this);
    State::SubState sub;
    sub.state = subState;
    sub.toSubState = addEdge("To" + subState->getName(), subState, [](){return false;});
    subStates.push_back(sub);
    return sub.toSubState;
}

Transition* State::checkTransitions() const
{
    for (Transition* transition : transitions) 
    {
        if (transition->guard())
        {
            return transition;
        }
    }
    return nullptr;
}

Transition* State::getSuccessTransition()
{
    for (Transition* tr : transitions) 
    {
        if(tr->next == getSuccessState()) 
        {
            return tr;
        }
    }
    return nullptr;
}

Transition* State::getFailureTransition()
{
    for (Transition* tr : transitions) 
    {
        if(tr->next == getFailureState()) 
        {
            return tr;
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

Transition *State::addEdge(const std::string &name, State* next, std::function<bool()> guard) 
{
    Transition *tr = new Transition(name, this, next, guard);
    this->transitions.push_back(tr);
    return tr;
}

void State::deleteEdge(Transition* tr)
{
    if(!tr) 
    {
	return;
    }
    for (unsigned i = 0; i < transitions.size(); i++) 
    {
        if (transitions[i]->getId() == tr->getId()) 
        {
            transitions.erase(transitions.begin() + i);
            return;
        }
    }
    return;
}

const std::vector< State::SubState > &State::getSubStates() const
{
    return subStates;
}

/**
 * Deletes all Transitions from this to substate
 */
void State::deRegisterSubState(State *subState)
{
    if(!subState) 
    {
    	return;
    }
    std::vector<SubState>::iterator it;
    it = subStates.begin();
    while(it != subStates.end()) 
    {
        if(it->state->getId() == subState->getId()) 
        {
            it = subStates.erase(it);
        } 
        else 
        {
            it++;
        }
    }
    return;
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
