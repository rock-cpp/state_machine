#pragma once
#include "State.hpp"

namespace state_machine 
{
    
class InitState : public State 
{
private:
    orocos_cpp::ConfigurationHelper confHelper;
    orocos_cpp::TransformerHelper* trHelper;
    bool configure();
    void start();
    
protected:
    bool doLog; 
    bool sim;
    TaskWithConfig* taskWithConfig;
    std::vector<InitState*> *dependencies;
    std::string taskName;
    
    virtual bool setup() = 0;
    virtual void connect() = 0;
    virtual void initDependencies() = 0;
    
    void updateConfig(RTT::TaskContext *task, const std::vector<std::string> &configs);
    void updateConfig(RTT::TaskContext *task, const std::string &config, const std::string &config2);   
    void updateConfig(RTT::TaskContext *task, const std::string &config, const std::string &config2, const std::string &config3);
    void registerWithConfig(RTT::TaskContext *task, const std::vector<std::string> &configs);
    void registerWithConfig(RTT::TaskContext *task, const std::string &config = "default");
    void registerWithConfig(RTT::TaskContext *task, const std::string &config, const std::string &config2);
    void registerWithConfig(RTT::TaskContext *task, const std::string &config, const std::string &config2, const std::string &config3);
    
public:
    InitState(const std::string& name, const std::string &taskName, State* success, State* failure, bool doLog, bool sim);
    TaskWithConfig* getTaskWithConfig() { return taskWithConfig; };
    virtual void enter(const State *lastState) {};
    virtual void exit() {};
    virtual void executeFunction();
    void setConfigNames(const std::string &configName);
    void setConfigNames(const std::string &configName1, const std::string &configName2);
    void setConfigNames(const std::string &configName1, const std::string &configName2, const std::string &configName3);
    void setConfigNames(std::vector<std::string> &configNames);
};

}