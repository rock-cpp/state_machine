#pragma once
#include "State.hpp"

namespace state_machine 
{

class NetworkState : public state_machine::State 
{
private:
    std::vector<std::string> *preemptingTasks;
    bool preemptionWanted;
    
public:
    NetworkState(const std::string& name);
    void preempt(bool wanted) { preemptionWanted = wanted ; };
    bool preemptionHook(State* preemptedState);
};

}