#pragma once
#include <functional>
#include <base/Time.hpp>
#include <sstream>
#include <map>
#include <vector>
#include "Events.hpp"

namespace state_machine
{

/**
 *The StateMachine contains all tasks and variables that are needed by multiple states. init() has to be called before start()
 */
class StateMachine
{
  
public:
    
    static StateMachine& getInstance() {
        static StateMachine instance; 
        return instance;
    };
    bool execute();
    
    /**
     * Executes the given state as a substate.
     * Will return true, if the state was in
     * finished state on exit;
     * */
    bool executeSubState(state_machine::State* subState);
    
    void start(state_machine::State* initState);

    /**
     * Callback, that gets called every time execute is called
     * */
    void setExecuteCallback(std::function<void()> loopCallback);
    
    base::Time lastUpdate;
    base::Time timePassed;
    
    std::stringstream &getDebugStream()
    {
        return debugStream;
    };
    
    State &getCurrentState()
    {
        return *currentState;
    };
    
    void registerState(State *state);
    void deRegisterState(State *state);

    void registerTransition(Transition *tr);
    void deregisterTransition(Transition *tr);
    
    unsigned int getNewStateId();
    unsigned int getNewTransitionId();
    
    const std::map<State *, std::string> &getAllStates() const
    {
        return states;
    };
    
    std::vector<serialization::Event> getNewEvents()
    {
        std::vector<serialization::Event> newOnes = events;
        events.clear();
        return newOnes;
    }
    
    void transitionTriggered(Transition *tr);
    
private:
    std::map<State *, std::string> states;
    std::map<Transition *, std::string> transistions;
    std::vector<serialization::Event> events;
    
    std::stringstream debugStream;
    
    State* currentState;
    base::Time executionStep;
    
    void checkPreemption();
    
    std::function<void()> executeCallback;
    
    unsigned int idCounterState;
    unsigned int idCounterTransition;
    
    ~StateMachine() {};
    StateMachine();
    StateMachine(StateMachine const&) = delete;
    void operator=(StateMachine const&) = delete;
};

}
