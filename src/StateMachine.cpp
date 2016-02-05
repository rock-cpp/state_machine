#include "StateMachine.hpp"
#include "Config.hpp"
#include <boost/lexical_cast.hpp>
#include "State.hpp"
#include "Transition.hpp"
#include "RPCCall.hpp"

namespace state_machine
{
   
StateMachine::StateMachine() : currentState(nullptr), idCounterState(0), idCounterTransition(0)
{
    double frequency = boost::lexical_cast<double>(Config::getConfig().getValue("executeFrequency"));
    executionStep = base::Time::fromSeconds( 1.0 / frequency); 
    executeCallback = [](){};
}

bool StateMachine::registerPreemtptionState(NetworkState* state)
{
    return preemptionStates.insert(state).second;
}

/**
 * Executes preemtion, checkPreemption has to be run first, returns value of executeSubState for every preemptingState as a map State*,bool
 */
void StateMachine::executePreemption()
{
    while(!preemptingStates.empty()) 
    {
      State* preemptingState = preemptingStates.front();
      if(preemptingState && currentState->getId() != preemptingState->getId()) 
      {
	  //Add Edge to have transition from preemtionState to currentState, so executeSubState can finish
	  Transition* premptionSuccess = preemptingState->addEdge("Preemption done", currentState, [&](){return preemptingState->finished();});
	  
	  //Delete old success and failure transitions so executeSubState finishes and save them
	  Transition* oldSuccessTr = preemptingState->getSuccessTransition();
	  Transition* oldFailureTr = preemptingState->getFailureTransition();
	  preemptingState->deleteEdge(preemptingState->getSuccessTransition());
	  preemptingState->deleteEdge(preemptingState->getFailureTransition());
	  
	  //Register subState and save transition to delete it after execution
	  Transition* toSubState = currentState->registerSubState(preemptingState);
	  
	  currentState->executeSubState(preemptingState);
	  
	  currentState->deRegisterSubState(preemptingState);
	  
	  if(oldFailureTr) 
          {
	      preemptingState->addEdge(oldFailureTr->getName(), oldFailureTr->next, oldFailureTr->guard);
	  }
	  if(oldSuccessTr) 
          {
	      preemptingState->addEdge(oldSuccessTr->getName(), oldSuccessTr->next, oldSuccessTr->guard);
	  }
	  preemptingState->deleteEdge(premptionSuccess);
	  preemptingState->deleteEdge(toSubState);
      }
      preemptingStates.pop();
    }
}


/**
 * If this is called the state machine checks for interruptions by registered states and returns if premption is wanted and execute should be called
 */
bool StateMachine::checkPreemption(State* preemptedState)
{
    bool preempted = false;
    for(NetworkState* state : preemptionStates) 
    {
	if(currentState->getId() != state->getId() && state->preemptionHook(preemptedState)) 
	{
	    preemptingStates.push(state);
	    preempted = true;
	} 
    }
    return preempted;
}


/* 
 * Runs the execute from the active state with a certain frequency. Returns if the mission is finished
 */
bool StateMachine::execute()
{
    lastUpdate = base::Time::now();
    Transition *transition = currentState->checkTransitions();
    
    if(transition)
    {
        transitionTriggered(transition);
        currentState->exit();
        transition->next->enterExt(currentState);
        if(currentState->autoDestroy())
        {
            delete currentState;
        }
        currentState = transition->next;
    }
    currentState->executeFunction();
    executeCallback();
    timePassed = base::Time::now() - lastUpdate;
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
                {
                    delete currentState;
                }
                currentState = transition->next;
                return result;
            }
            transition->next->enterExt(currentState);
            if(currentState->autoDestroy()) 
            {
                delete currentState;
            }
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

State* StateMachine::getStateByName(std::string name)
{
    for (auto entry : states) 
    {
        if(entry.second.compare(name) == 0) 
        {
            return entry.first;
        }
    }
    return nullptr;
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
}

}