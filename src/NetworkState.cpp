#include "NetworkState.hpp"

namespace state_machine 
{

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

}