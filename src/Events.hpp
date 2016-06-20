#pragma once
#include <limits>
#include <string>
#include <vector>

namespace state_machine
{
    class State;
    class Transition;
    class StateMachine;
namespace serialization
{
struct Serializable
{
    Serializable() : id(std::numeric_limits<unsigned int>::max()) {}
    unsigned int id;
    bool isValid() const;
};

struct State : Serializable
{
    State(const ::state_machine::State &state);
    State() : Serializable(), parentId(-1) {}
    std::string name;
    unsigned int parentId;
};

struct Transition : Serializable
{
    Transition() : Serializable() {}
    Transition(const ::state_machine::Transition &trans);
    std::string name;
    State from;
    State to;
};

struct StateMachine
{
    StateMachine() {}
    StateMachine(const ::state_machine::StateMachine &sm);
    std::vector<State> allStates;
    std::vector<Transition> allTransitions;
};

enum EVENT_TYPE
{
    TransitionTriggered,
    StateChanged,
};

struct Event : Serializable
{
    Event() : Serializable() {}
    EVENT_TYPE type;
};

}
}
