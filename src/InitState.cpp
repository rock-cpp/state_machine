#include "InitState.hpp"

namespace state_machine 
{
   
TaskWithConfig* InitState::getTaskWithConfig(RTT::TaskContext* task)
{
    for(TaskWithConfig* taskWithConfig : tasksWithConfig) 
    {
        if(taskWithConfig->task->getName() == task->getName()) 
        {
            return taskWithConfig;
        }
    }
    TaskWithConfig* taskWithConfig = new TaskWithConfig();
    taskWithConfig->task = task;
    tasksWithConfig.push_back(taskWithConfig);
    return taskWithConfig;
}
    
void InitState::configure()
{
    for(TaskWithConfig* taskWithConfig : tasksWithConfig) 
    {
        if(!confHelper.applyConfig(taskWithConfig->task, taskWithConfig->configs))
        {
            throw std::runtime_error("InitState::Failed to apply config to task " + taskWithConfig->task->getName());
        }
        
        if(!trHelper->configureTransformer(taskWithConfig->task))
        {
            throw std::runtime_error("InitState::Failed to configure transformer for task " + taskWithConfig->task->getName());
        }

        msg << "InitState::Configuring " << taskWithConfig->task->getName() << std::endl;
        if(!taskWithConfig->task->configure())
        {
            std::string config = "[";
            for(auto conf: taskWithConfig->configs)
            {
                config += conf + ", ";
            }
            config += "]";
            throw std::runtime_error("InitState::Failed to configure task " + taskWithConfig->task->getName() + " with configuration " + config);
        }
        
        msg << "InitState::Configured " << taskWithConfig->task->getName() << std::endl;
    }
}
    
void InitState::start()
{
    for(TaskWithConfig* taskWithConfig : tasksWithConfig) 
    {
        if(!taskWithConfig->task->start())
        {
            throw std::runtime_error("InitState::Failed to start task " + taskWithConfig->task->getName());
        }
        msg << "InitState::Started " << taskWithConfig->task->getName() << std::endl;
    }
}

bool InitState::stateAlreadyConfigured()
{
    for(TaskWithConfig* taskWithConfig : tasksWithConfig) 
    {
        if(taskWithConfig->task->isConfigured() || taskWithConfig->task->isRunning()) 
        {
            return true;
        }
    }
    return false;
}
    
void InitState::executeFunction()
{
    if(stateAlreadyConfigured()) 
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
    else 
    {
        msg << "The state " << name << " already configured, ignoring!" << std::endl;
    }
    finish();
}

void InitState::setTaskName (const std::string taskName)
{
    taskNames.clear();
    taskNames.push_back(taskName);
}

void InitState::setTaskNames (std::vector<std::string> &taskNames)
{
    this->taskNames = taskNames;
}

void InitState::registerWithConfig(RTT::TaskContext* task, const std::vector< std::string >& configs)
{
    TaskWithConfig* taskWithConfig = getTaskWithConfig(task);
    taskWithConfig->task = task;
    taskWithConfig->configs = configs;
}

void InitState::registerWithConfig(RTT::TaskContext* task, const std::string &config)
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

InitState::InitState(const std::string &name, State* success, State* failure, bool doLog, bool sim)
    : State(name, success, failure), doLog(doLog), sim(sim) 
{
    dependencies = new std::vector<InitState*>();
    if(!spawnedTasks) 
    {
        spawnedTasks = new std::vector<std::string>();
    }
}

void InitState::spawnDeployment(const std::string& deploymentName)
{
    if(std::find(spawnedTasks->begin(), spawnedTasks->end(), deploymentName) == spawnedTasks->end()) 
    {
        orocos_cpp::Spawner &spawner(orocos_cpp::Spawner::getInstace());
        spawner.spawnDeployment(deploymentName);
        spawnedTasks->push_back(deploymentName);
    }
}

void InitState::spawnTask(const std::string& taskName)
{
    if(std::find(spawnedTasks->begin(), spawnedTasks->end(), taskName) == spawnedTasks->end()) 
    {
        orocos_cpp::Spawner &spawner(orocos_cpp::Spawner::getInstace());
        spawner.spawnTask(taskName);
        spawnedTasks->push_back(taskName);
    }
}

}
