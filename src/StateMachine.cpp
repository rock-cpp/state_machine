#include "StateMachine.hpp"
#include "Config.hpp"
#include <boost/lexical_cast.hpp>
#include "State.hpp"
#include "Transition.hpp"

namespace state_machine
{
   
StateMachine::StateMachine() : currentState(nullptr), idCounterState(0), idCounterTransition(0)
{
    double frequency = boost::lexical_cast<double>(Config::getConfig().getValue("executeFrequency"));
    executionStep = base::Time::fromSeconds( 1.0 / frequency);
    
    executeCallback = [](){};
}

/* 
 * Runs the execute from the active state with a certain frequency. Returns if the mission is finished
 */
bool StateMachine::execute()
{
    lastUpdate = base::Time::now();
    
    State *newState = currentState->execute();

    if(newState != currentState)
    {
        if(currentState->autoDestroy())
            delete currentState;
    }
    
    currentState = newState; 
    
    executeCallback();
    
    timePassed = base::Time::now() - lastUpdate;
    //   helper->message("Executed in " + std::to_string(timePassed.toMicroseconds()) + " µs");
    if (timePassed < executionStep) 
    {
        usleep((executionStep - timePassed).toMicroseconds());
    }
    return false;
}

void StateMachine::executeSubState(State* subState)
{
    State *executingState = currentState;
    
    State::SubState sub;
    bool found = false;
    for(const State::SubState &s : currentState->getSubStates())
    {
        if(currentState == s.state)
        {
            found = true;
            sub = s;
            break;
        }
    }
    
    if(!found)
    {
        throw std::runtime_error("Error, subState was executed, that never got registered");
    }
    
    StateMachine::getInstance().transitionTriggered(sub.toSubState);

    currentState->enter(executingState);
    currentState = subState;
    
    while(currentState != executingState)
    {
        execute();

    }
}

void StateMachine::setExecuteCallback(std::function< void () > loopCallback)
{
    executeCallback = loopCallback;
}

unsigned int StateMachine::getNewStateId()
{
    int ret = idCounterState;
    idCounterState++;
    return ret;
}

unsigned int StateMachine::getNewTransitionId()
{
    int ret = idCounterTransition;
    idCounterTransition++;
    return ret;
}


void StateMachine::transitionTriggered(Transition* tr)
{
    serialization::Event ev;
    ev.id = tr->getId();
    ev.type = serialization::TransitionTriggered;
    events.push_back(ev);    
    
    ev.id = tr->next->getId();
    ev.type = serialization::StateChanged;
    events.push_back(ev);
}

void StateMachine::registerState(State* state)
{
    states[state] = state->getName();
}

void StateMachine::deRegisterState(State* state)
{
    states.erase(state);
}


void StateMachine::init(State* initState)
{
    currentState = initState;
    lastUpdate = base::Time::now();
}

void StateMachine::start()
{
    serialization::Event ev;
    ev.id = currentState->getId();
    ev.type = serialization::StateChanged;
    events.push_back(ev);
    
    currentState->enter(nullptr);
    currentState->execute();
};

}