#include "State.hpp"
#include "StateMachine.hpp"
#include "Transition.hpp"


namespace state_machine
{
    
bool InitState::configure()
{
    bool success = true;
    
    for(TaskWithConfig &t: allTasks)
    {
        
        if(!confHelper.applyConfig(t.task, t.config))
        {
            success = false;
        }
        
        if(!trHelper->configureTransformer(t.task))
        {
            throw std::runtime_error("Init::Failed to configure transformer for task " + t.task->getName());
        }

        std::cout << "Init::Configuring " << t.task->getName() << std::endl;
        if(!t.task->configure())
        {
            std::string config = "[";
            for(auto conf: t.config)
            {
                config += conf + ", ";
            }
            config += "]";
            throw std::runtime_error("Init::Failed to configure task " + t.task->getName() + " with configuration " + config);
        }
        
        std::cout << "Init::Configured " << t.task->getName() << std::endl;
        
    }
    return success;
}
    
void InitState::start()
{
    for(TaskWithConfig &t: allTasks)
    {
        if(!t.task->start())
        {
            throw std::runtime_error("Init::Failed to start task " + t.task->getName());
        }
        std::cout << "Init::Started " << t.task->getName() << std::endl;

    }
}
    
void InitState::executeFunction()
{
    setup();
    configure();
    connect();
    start();   
}

bool NetworkState::preemptionHook(State* preemptedState)
{
    if(preemptionWanted) 
    {
        for( std::string stateName : *preemptingTasks) 
        {
            if(stateName == preemptedState->getName()) 
            {
                return true;
            }
        }
    }
    return false;
}

NetworkState::NetworkState(const std::string& name): State(name)
{
    preemptingTasks = new std::vector<std::string>();
}

void InitState::updateConfig(RTT::TaskContext* task, const std::vector< std::string >& configs)
{
    for(TaskWithConfig &t: allTasks)
    {
        if(t.task == task)
        {
            t.config = configs;
            return;
        }
    }
    
    throw std::runtime_error("Init::updateConfig : No task " + task->getName() + " registered");
}

void InitState::updateConfig(RTT::TaskContext* task, const std::string& config, const std::string& config2)
{
    std::vector< std::string > configs;
    configs.push_back(config);
    configs.push_back(config2);
    
    updateConfig(task, configs);
}

void InitState::updateConfig(RTT::TaskContext* task, const std::string& config, const std::string& config2, const std::string& config3)
{
    std::vector< std::string > configs;
    configs.push_back(config);
    configs.push_back(config2);
    configs.push_back(config3);
    
    updateConfig(task, configs);
}

void InitState::registerWithConfig(RTT::TaskContext* task, const std::vector< std::string >& configs)
{
    TaskWithConfig t;
    t.task = task;
    t.config = configs;
    
    allTasks.push_back(t);    
}

void InitState::registerWithConfig(RTT::TaskContext* task, const std::string& config)
{
    std::vector< std::string > configs;
    configs.push_back(config);
    
    registerWithConfig(task, configs);
}

void InitState::registerWithConfig(RTT::TaskContext* task, const std::string& config, const std::string& config2)
{
    std::vector< std::string > configs;
    configs.push_back(config);
    configs.push_back(config2);
    
    registerWithConfig(task, configs);
}

void InitState::registerWithConfig(RTT::TaskContext* task, const std::string& config, const std::string& config2, const std::string& config3)
{
    std::vector< std::string > configs;
    configs.push_back(config);
    configs.push_back(config2);
    configs.push_back(config3);

    registerWithConfig(task, configs);
}


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
