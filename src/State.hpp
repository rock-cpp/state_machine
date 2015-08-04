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
    virtual ~State() {};
    virtual void enter(const State *lastState) = 0;
    virtual void exit() = 0;
    virtual void executeFunction() = 0;
    State* execute();
    Transition *addEdge(const std::string &name, State* next, std::function<bool()> guard);
  
    void registerSubState(State *subState);
    
    void executeSubState(state_machine::State* subState);
    
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

    const std::vector<SubState> &getSubStates() const
    {
        return subStates;
    }
    
protected:
    
    
    const unsigned int id;
    std::vector<Transition *> transitions;
    std::vector<SubState> subStates;
    std::ostream &msg;

    bool destroyOnExit;
  
private:
    ///Name of the state.  
    std::string name;

  
};



}
#endif // STATE_H
