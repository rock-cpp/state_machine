#pragma once
#include "State.hpp"
#include <orocos_cpp/Spawner.hpp>

namespace state_machine 
{
    
class InitState : public State 
{
private:
    orocos_cpp::ConfigurationHelper confHelper;
    orocos_cpp::TransformerHelper* trHelper;
    void configure();
    void start();
    
protected:
    bool doLog; 
    bool sim;
    std::vector<std::string> taskNames;
    std::vector<TaskWithConfig*> tasksWithConfig;
    std::vector<InitState*> *dependencies;
    
    virtual bool setup() = 0;
    virtual void connect() = 0;
    virtual void initDependencies() = 0;
    void setTaskName(const std::string taskName);
    void setTaskNames(std::vector<std::string> &taskName);
    static std::vector<std::string> *spawnedTasks;
        
    void spawnDeployment(const std::string &deploymentName);
    void spawnTask(const std::string &taskName);
    
public:
    InitState(const std::string& name, State* success, State* failure, bool doLog, bool sim);
    std::vector<TaskWithConfig*>* getTasksWithConfig() { return &tasksWithConfig; };
    TaskWithConfig* getTaskWithConfig(RTT::TaskContext *task);
    virtual void enter(const State *lastState) {};
    virtual void exit() {};
    virtual void executeFunction();
    void registerWithConfig(RTT::TaskContext *task, const std::vector<std::string> &configs);
    void registerWithConfig(RTT::TaskContext *task, const std::string &config = "default");
    void registerWithConfig(RTT::TaskContext *task, const std::string &config, const std::string &config2);
    void registerWithConfig(RTT::TaskContext *task, const std::string &config, const std::string &config2, const std::string &config3);
};

}