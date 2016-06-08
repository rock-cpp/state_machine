#ifndef STATE_H
#define STATE_H

#include <vector>
#include <iostream>
#include <functional>

namespace state_machine
{

//Classes
class State;
class Transition;
class StateMachine;
/**
 * This class represents a single State. Every state has a enter, execute and exit function. The functions are called like the names suggest. Every state has to have a individual id. This id is used 
to display the StateMachine in the GUI
 */
class State 
{
public:
    State(const std::string &name);
    State(const std::string &name, State *success);
    State(const std::string &name, State *success, State *failue);
    
    virtual ~State();
    /**
     * Called only one time at startup / When a task is registered
     * */
    virtual void init() = 0;
    virtual void exit() = 0;
    virtual void executeFunction() = 0;
    /**
     * Return true if interruption by this state is wanted, default returns false
     */
    virtual bool preemptionHook(State* preemptedState);
    Transition *addEdge(const std::string &name, State* next, std::function<bool()> guard);
    void deleteEdge(Transition*);
  
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
    
    /**
     * Returns a State* to the parentState, if state is no substate it returns itself.
     */
    const State* getParentState() const;
    
    /**
     * Sets pointer to parent state
     */
    void setParentState(const State* state);
  
    State* getSuccessState();
    State* getFailureState();
    
    Transition* getSuccessTransition();
    Transition* getFailureTransition();
    
    
protected:
    virtual void enter(const State *lastState) = 0;
    const unsigned int id;
    std::vector<Transition *> transitions;
    std::vector<SubState> subStates;
    std::ostream &msg;

    bool isFinished;
    bool hasFailed;
    
    State *successState;
    State *failureState;
    
    
    bool destroyOnExit;
  
private:
    ///Name of the state.  
    std::string name;
    const State* parentState;
    bool isPreemptable;
};

}
#endif // STATE_H
