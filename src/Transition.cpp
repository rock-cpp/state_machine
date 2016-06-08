#include "Transition.hpp"
#include "StateMachine.hpp"

namespace state_machine
{

Transition::Transition(const std::string &name, State *prev, State *next, std::function<bool()> guard) :
        prev(prev), next(next), guard(guard), name(name), id(StateMachine::getInstance().getNewTransitionId())
{

}

Transition::~Transition()
{

}


}