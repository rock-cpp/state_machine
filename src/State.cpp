#include "State.hpp"
#include "Config.hpp"
#include <boost/lexical_cast.hpp>

namespace state_machine
{

Transition::Transition(const std::string &name, State *prev, State *next, std::function<bool()> guard) :
        prev(prev), next(next), guard(guard), name(name), id(StateMachine::getInstance().getNewTransitionId())
{

}



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


State* State::execute() 
{
//     for(auto iter = helper->allTaskStatesReader.begin(); iter != helper->allTaskStatesReader.end(); ++iter)
//     {
//       if(iter->second->read(helper->allTaskStates[iter->first]) == RTT::NoData) {
//         helper->allTaskStates[iter->first] = iter->first->getTaskState();
//       }
//     }
//     helper->sendTaskStatus();
    for (Transition* transition : transitions) {
        if (transition->guard()){
            
            StateMachine::getInstance().transitionTriggered(transition);
            
            msg << "Transition " << transition->getName() << " triggered. Leaving state " << getName() << " and entering state " << transition->next->getName() << std::endl;
            this->exit();
            transition->next->enter(this);
    //         this->helper->message("$:" + std::to_string(transition->id) + ":" + std::to_string(transition->next->id));

            //also call execute at least once
            transition->next->executeFunction();
            
            return transition->next;
        }
    }
    
    executeFunction();
    return this;
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