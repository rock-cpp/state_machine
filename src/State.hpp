#pragma once

#include <vector>
#include <iostream>
#include <functional>
#include <orocos_cpp/ConfigurationHelper.hpp>
#include <orocos_cpp/TransformerHelper.hpp>

namespace state_machine
{

class State;
class Transition;
class StateMachine;

class TaskWithConfig
{
public:
    RTT::TaskContext *task;
    std::string taskName;
    std::vector<std::string> configs;
};

/**
 * This class represents a single State. Every state has a enter, execute and exit function. The functions are called like the names suggest. Every state has to have a individual id. This id is used 
to display the StateMachine in the GUI
 */
class State 
{
public:
    State(const std::string &name) : State(name, nullptr, nullptr) {};
    State(const std::string &name, State *success) : State(name, success, nullptr) {};
    State(const std::string &name, State *success, State *failure);
    
    virtual ~State() {};
    virtual void exit() = 0;
    virtual void executeFunction() = 0;
    virtual void enter(const State *lastState) = 0;
    
    Transition *addEdge(const std::string &name, State* next, std::function<bool()> guard);
    void deleteEdge(Transition*);
    
    virtual void generateId(std::string id);
  
    virtual void enterExt(const State *lastState);
    
    /**
     * Registeres a substate. 
     * This function must be called prior to calling
     * executeSubState for a given state.
     * 
     * Returns the transition from the current state to the substate
     * */
    Transition* registerSubState(State *subState);
    
    /**
     * Removes a registered substate.
     */
    void deRegisterSubState(State *subState);
    
    /**
     * Executes the given state, until an transition,
     * to 'this' state occures. The function will
     * block. Will return true, if state was in finished state
     * on the transition back to 'this' state.
     * 
     * Note, the ExecuteCallback from the statemachine will be called
     * */
    bool executeSubState(state_machine::State* subState);

    /**
     * Convenienc variant of above function
     * */
    bool executeSubState(state_machine::State &subState);

    /**
     * Returns if the state should be deleted, if it is left
     * */
    bool autoDestroy();
    
    /**
     * Sets if the state should be deleted on exit
     * */
    void autoDestroy(bool deleteIt);

    const std::string &getName() const;
    
    const unsigned int getId() const
    {
        return id;
    }

    const std::vector<Transition*> &getTransitions() const
    {
        return transitions;
    }
    
    class SubState
    {
    public:
        State *state;
        Transition *toSubState; 
    };

    const std::vector<SubState> &getSubStates() const;
    
    /**
     * Checks, if any of the transitions of this state triggered.
     * Returns the first registered transition that triggered,
     * or a nullptr.
     * */
    Transition *checkTransitions() const;
    
    /**
     * Marks this state as failed and activates the failure transition
     * */
    void fail();
    
    bool failed() const;
    
    /**
     * Marks this state as finished and activates the success transition
     * */
    void finish();

    bool finished() const;
    
    const State* getParentState() const { return parentState; } ;
    void setParentState(const State* state) { parentState = state; };
   
  
    State* getSuccessState();
    State* getFailureState();
    
    Transition* getSuccessTransition();
    Transition* getFailureTransition();
    
    
protected:
    
    int id;
    std::vector<Transition *> transitions;
    std::vector<SubState> subStates;
    std::ostream &msg;

    bool isFinished;
    bool hasFailed;
    
    State *successState;
    State *failureState;
    
    
    bool destroyOnExit;
    std::string name;
    const State* parentState;

};

}
