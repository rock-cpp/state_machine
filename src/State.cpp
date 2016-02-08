#include "State.hpp"
#include "StateMachine.hpp"
#include "Transition.hpp"


namespace state_machine
{
    
bool InitState::configure()
{
    bool success = true;
    
    if(!confHelper.applyConfig(taskWithConfig->task, taskWithConfig->config))
    {
        success = false;
    }
    
    if(!trHelper->configureTransformer(taskWithConfig->task))
    {
        throw std::runtime_error("Init::Failed to configure transformer for task " + taskWithConfig->task->getName());
    }

    std::cout << "Init::Configuring " << taskWithConfig->task->getName() << std::endl;
    if(!taskWithConfig->task->configure())
    {
        std::string config = "[";
        for(auto conf: taskWithConfig->config)
        {
            config += conf + ", ";
        }
        config += "]";
        throw std::runtime_error("Init::Failed to configure task " + taskWithConfig->task->getName() + " with configuration " + config);
    }
    
    std::cout << "Init::Configured " << taskWithConfig->task->getName() << std::endl;
    return success;
}
    
void InitState::start()
{
    if(!taskWithConfig->task->start())
    {
        throw std::runtime_error("Init::Failed to start task " + taskWithConfig->task->getName());
    }
    std::cout << "Init::Started " << taskWithConfig->task->getName() << std::endl;
}
    
void InitState::executeFunction()
{
    setup();
    configure();
    initDependencies();
    for(InitState* initState : *dependencies) 
    {
        if(!executeSubState(initState)) 
        {
            throw std::runtime_error("InitState::Failed to execute state " + initState->getName());
        }
    }
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
    if(taskWithConfig->task == task)
    {
        taskWithConfig->config = configs;
        return;
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
    taskWithConfig = new TaskWithConfig();
    taskWithConfig->task = task;
    taskWithConfig->config = configs; 
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
