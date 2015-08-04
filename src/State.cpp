#include "State.hpp"
#include "StateMachine.hpp"
#include "Transition.hpp"

namespace state_machine
{



State::State(const std::string &name_) : id(StateMachine::getInstance().getNewStateId()), msg(StateMachine::getInstance().getDebugStream()), destroyOnExit(false), name(name_)
{
    StateMachine::getInstance().registerState(this);
}

const std::string& State::getName() const
{
    return name;
}

bool State::autoDestroy()
{
    return destroyOnExit;
}

void State::autoDestroy(bool deleteIt)
{
    destroyOnExit = deleteIt;
}

void State::registerSubState(State* subState)
{
    State::SubState sub;
    sub.state = subState;
    sub.toSubState = addEdge("To" + subState->getName(), subState, [](){return false;});
    subStates.push_back(sub);
}


State* State::execute() 
{
//     for(auto iter = helper->allTaskStatesReader.begin(); iter != helper->allTaskStatesReader.end(); ++iter)
//     {
//       if(iter->second->read(helper->allTaskStates[iter->first]) == RTT::NoData) {
//         helper->allTaskStates[iter->first] = iter->first->getTaskState();
//       }
//     }
//     helper->sendTaskStatus();
    for (Transition* transition : transitions) {
        if (transition->guard()){
            
            StateMachine::getInstance().transitionTriggered(transition);
            
            msg << "Transition " << transition->getName() << " triggered. Leaving state " << getName() << " and entering state " << transition->next->getName() << std::endl;
            this->exit();
            transition->next->enter(this);
    //         this->helper->message("$:" + std::to_string(transition->id) + ":" + std::to_string(transition->next->id));

            //also call execute at least once
            transition->next->executeFunction();
            
            return transition->next;
        }
    }
    
    executeFunction();
    return this;
}

void State::executeSubState(State *subState)
{
    StateMachine::getInstance().executeSubState(subState);
}

Transition *State::addEdge(const std::string &name, State* next, std::function<bool()> guard) 
{
    Transition *tr = new Transition(name, this, next, guard);
    this->transitions.push_back(tr);
    return tr;
}

}