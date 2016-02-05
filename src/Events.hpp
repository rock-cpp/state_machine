#pragma once

#include <string>
#include <vector>

namespace state_machine
{
    class State;
    class Transition;
    class StateMachine;
namespace serialization
{
struct State 
{
    State(const ::state_machine::State &state);
    State() : id(-1), parentId(-1) {};
    std::string name;
    int id;
    int parentId;
};

struct Transition
{
    Transition() : id(-1) {};
    Transition(const ::state_machine::Transition &trans);
    std::string name;
    State from;
    State to;
    unsigned int id;
};

struct StateMachine
{
    StateMachine() {};
    StateMachine(const ::state_machine::StateMachine &sm);
    std::vector<State> allStates;
    std::vector<Transition> allTransitions;
};

enum EVENT_TYPE
{
    TransitionTriggered,
    StateChanged,
};

struct Event
{
    EVENT_TYPE type;
    unsigned int id;    
};

}

}