#include "InitState.hpp"

namespace state_machine {
   
bool InitState::configure()
{
    bool success = true;
    
    if(!confHelper.applyConfig(taskWithConfig->task, taskWithConfig->configs))
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
        for(auto conf: taskWithConfig->configs)
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
    if(setup()) 
    {
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
        std::cout << getName() << "::" << taskWithConfig->task->getName() << " already configured, ignoring!" << std::endl;
    }
    finish();
}

void InitState::setConfigNames(const std::string& configName)
{
   taskWithConfig->configs.clear();
   taskWithConfig->configs.push_back(configName);
}

void InitState::setConfigNames(const std::string& configName1, const std::string& configName2)
{
    taskWithConfig->configs.clear();
    taskWithConfig->configs.push_back(configName1);
    taskWithConfig->configs.push_back(configName2);
}

void InitState::setConfigNames(const std::string& configName1, const std::string& configName2, const std::string& configName3)
{
    taskWithConfig->configs.clear();
    taskWithConfig->configs.push_back(configName1);
    taskWithConfig->configs.push_back(configName2);
    taskWithConfig->configs.push_back(configName3);
}

void InitState::updateConfig(RTT::TaskContext* task, const std::vector< std::string > &configs)
{
    if(taskWithConfig->task == task)
    {
        taskWithConfig->configs = configs;
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

InitState::InitState(const std::string& name, const std::string &taskName, State* success, State* failure, bool doLog, bool sim)
    : State(name, success, failure), doLog(doLog), sim(sim) 
{
    dependencies = new std::vector<InitState*>();
    std::vector<std::string> defaultConfig;
    defaultConfig.push_back("default");
    taskWithConfig = new TaskWithConfig();
    taskWithConfig->configs = defaultConfig;
}

void InitState::setConfigNames(std::vector< std::string >& configNames)
{
    taskWithConfig->configs = configNames;
}

}

