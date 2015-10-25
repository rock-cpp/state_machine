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
    
    if(currentState->getIsPreemptable()) 
    {
            for(auto state : getAllStates()) 
            {
                if(currentState != state.first && state.first->preemptionHook()) 
                {
                    state.first->addEdge("Preemption success", currentState, [&](){return state.first->finished();});
                    state.first->addEdge("Preemption failure", currentState, [&](){return state.first->failed();});
                    Transition* oldSuccessTr = state.first->deleteEdge("Success");
                    Transition* oldFailureTr = state.first->deleteEdge("Failed");
                    currentState->registerSubState(state.first);
                    
                    currentState->executeSubState(state.first);
                    
                    currentState->deRegisterSubState(state.first);
                    state.first->addEdge(oldSuccessTr->getName(), oldSuccessTr->next, oldSuccessTr->guard);
                    state.first->addEdge(oldFailureTr->getName(), oldFailureTr->next, oldFailureTr->guard);
                    state.first->deleteEdge("Preemption success");
                    state.first->deleteEdge("Preemption failure");
                    
                    break;
                } 
            }
    }
    
    Transition *transition = currentState->checkTransitions();
    
    if(transition)
    {
        transitionTriggered(transition);
        currentState->exit();
        transition->next->enterExt(currentState);

        if(currentState->autoDestroy())
            delete currentState;
        
        currentState = transition->next;
    }
    

    
    currentState->executeFunction();

    executeCallback();
    
    timePassed = base::Time::now() - lastUpdate;
    //   helper->message("Executed in " + std::to_string(timePassed.toMicroseconds()) + " µs");
    if (timePassed < executionStep) 
    {
        usleep((executionStep - timePassed).toMicroseconds());
    }
    return false;
}

bool StateMachine::executeSubState(State* subState)
{
    State *executingState = currentState;
    
    State::SubState sub;
    bool found = false;
    for(const State::SubState &s : currentState->getSubStates())
    {
        if(subState == s.state)
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
    
    transitionTriggered(sub.toSubState);

    currentState = subState;
    currentState->enterExt(executingState);
    
    while(true)
    {
        lastUpdate = base::Time::now();
        
        Transition *transition = currentState->checkTransitions();
        
        if(transition)
        {
            
            transitionTriggered(transition);
            currentState->exit();
            if(transition->next == executingState)
            {
                bool result = currentState->finished();
                
                if(currentState->autoDestroy())
                    delete currentState;
                
                currentState = transition->next;
                
                return result;
            }
            
            transition->next->enterExt(currentState);

            if(currentState->autoDestroy())
                delete currentState;
            
            currentState = transition->next;
        }
        
        
        currentState->executeFunction();

        executeCallback();
        
        timePassed = base::Time::now() - lastUpdate;
        if (timePassed < executionStep) 
        {
            usleep((executionStep - timePassed).toMicroseconds());
        }
    }
    return false;
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
    debugStream << "Transition " << tr->getName() << " triggered. Leaving state " << currentState->getName() << " and entering state " << tr->next->getName() << std::endl;

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

void StateMachine::start(State* initState)
{
    currentState = initState;
    
    serialization::Event ev;
    ev.id = currentState->getId();
    ev.type = serialization::StateChanged;
    events.push_back(ev);

    lastUpdate = base::Time::now();
    
    currentState->enterExt(nullptr);
    
    currentState->executeFunction();
};

}