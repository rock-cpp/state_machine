#ifndef STATE_H
#define STATE_H

#include <base/Time.hpp>
#include <vector>
#include <map>
#include <iostream>
#include <sstream>
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
    const int id;
    virtual void enter() = 0;
    virtual void exit() = 0;
    virtual void executeFunction() = 0;
    State* execute();
    Transition *addEdge(const std::string &name, State* next, std::function<bool()> guard);
  
    /**
     * Returns if the state should be deleted, if it is left
     * */
    bool autoDestroy();
    
    /**
     * Sets if the state should be deleted on exit
     * */
    void autoDestroy(bool deleteIt);

    const std::string &getName() const;

    const std::vector<Transition*> &getTransitions() const
    {
        return transitions;
    }
    
protected:
    std::vector<Transition*> transitions;

    std::ostream &msg;

    bool destroyOnExit;
  
private:
    ///Name of the state.  
    std::string name;

  
};

/**
 * A Transition between two states. Every Transition has to have a individual id. This id is used to display the StateMachine in the GUI
 */
class Transition
{
public:
    Transition(const std::string &name, State *prev, State *next, std::function<bool()> guard) :
        prev(prev), next(next), guard(guard), name(name) {};
    ~Transition() {};
    State *prev;
    State *next;
    std::function<bool()> guard;
    const std::string &getName() const
    {
        return name;
    }
private:
    std::string name;
    int id;
};

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
    void init(State* initState);
    void start();

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
    
    int getNewId();
    
    const std::map<State *, std::string> &getAllStates() const
    {
        return states;
    };
    
private:
    std::map<State *, std::string> states;
    std::map<Transition *, std::string> transistions;
    
    std::stringstream debugStream;
    
    State* currentState;
    base::Time executionStep;
    
    int idCounter;
    
    ~StateMachine() {};
    StateMachine();
    StateMachine(StateMachine const&) = delete;
    void operator=(StateMachine const&) = delete;
};

}
#endif // STATE_H
