#pragma once
#include <string>
#include <functional>

namespace state_machine
{

class State;
    
/**
 * A Transition between two states. Every Transition has to have a individual id. This id is used to display the StateMachine in the GUI
 */
class Transition
{
public:
    Transition(const std::string &name, State *prev, State *next, std::function<bool()> guard);
    ~Transition();
    State *prev;
    State *next;
    std::function<bool()> guard;
    const std::string &getName() const
    {
        return name;
    }

    const unsigned int getId() const
    {
        return id;
    }

private:
    std::string name;
    const unsigned int id;
};

}

