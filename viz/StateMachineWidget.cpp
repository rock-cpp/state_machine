#include "StateMachineWidget.hpp"
#include <state_machine/Events.hpp>
#include <qgv/qgv.h>
#include <QMainWindow>
#include <qgv/QGVScene.h>
#include <qgv/QGVEdge.h>
#include <qgv/QGVNode.h>
#include <qgv/QGVSubGraph.h>
#include <QApplication>
#include <iostream>
#include <boost/concept_check.hpp>
#include <time.h>
#include <stdlib.h>

StateMachineWidget::StateMachineWidget(QWidget* parent)
  : QGraphicsView(parent)
  , m_scene("StateMachine")
{
    setScene(&m_scene);

    setDefaultAttributes();
}

void StateMachineWidget::setDefaultAttributes()
{
    m_scene.setGraphAttribute("splines", "ortho");
    m_scene.setGraphAttribute("rankdir", "LR");
    //m_scene.setGraphAttribute("concentrate", "true"); //Error !
    m_scene.setGraphAttribute("nodesep", "0.4");
    m_scene.setGraphAttribute("style", "filled");
    m_scene.setGraphAttribute("fillcolor", "#a99fa5");
    
    m_scene.setNodeAttribute("shape", "box");
    m_scene.setNodeAttribute("style", "filled");
    m_scene.setNodeAttribute("fillcolor", "#806969");
    m_scene.setNodeAttribute("height", "1.2");
    
    m_scene.setEdgeAttribute("minlen", "3");
    m_scene.setEdgeAttribute("pad", "0.2000");

    m_activeState = -1;
    m_activeTransition = -1;
}

void StateMachineWidget::update(const state_machine::serialization::Event &event)
{
    switch(event.type)
    {
    case state_machine::serialization::StateChanged:
    {
        if(m_activeState >= 0)
        {
            m_idToState[m_activeState]->setAttribute("color", "black");
            m_idToState[m_activeState]->setAttribute("fillcolor", "#806969");
        }

        auto s = m_idToState.find(event.id);
        if(s != m_idToState.end())
        {
            m_activeState = event.id;
            m_idToState[m_activeState]->setAttribute("color", "black");
            m_idToState[m_activeState]->setAttribute("fillcolor", "yellow");
            m_idToState[m_activeState]->setFocus();

        }
        else
        {
            std::cout << "Error state " << event.id << " not found" << std::endl;
        }
    }
    break;
    case state_machine::serialization::TransitionTriggered:
    {
        if(m_activeTransition >= 0)
        {
            m_idToTransition[m_activeTransition]->setAttribute("color", "black");
            m_idToTransition[m_activeTransition]->setAttribute("arrowsize", "1.2");
        }
        auto trans = m_idToTransition.find(event.id);
        if(trans != m_idToTransition.end())
        {
            m_activeTransition = event.id;
            m_idToTransition[m_activeTransition]->setAttribute("color", "red");
            m_idToTransition[m_activeTransition]->setAttribute("arrowsize", "2.0");
        }                    
        else
        {
            std::cout << "Error unknown transition " << event.id << std::endl;
        }
    }
    break;
    }

    //Layout scene
//     m_scene.applyLayout();
}

void StateMachineWidget::removeState(const state_machine::serialization::State& state)
{
    std::cout << "Removing state" << state.name << "(" << state.id << ")" << std::endl;
    
    //first check if we are connected to some edge
    for(auto it = m_transitions.begin();it !=  m_transitions.end();)
    {
        if(it->from.id == state.id || it->to.id == state.id)
        {
            std::cout << "Removing transition " << it->name << " from " << it->from.name << "(" << it->from.id << ") to " << it->to.name << "(" << it->to.id << ")" << std::endl;
            m_scene.deleteEdge(m_idToTransition[it->id]);
            m_idToTransition.erase(it->id);
            it = m_transitions.erase(it);
        }
        else
        {
            it++;
        }
    }
    
    m_scene.deleteNode(m_idToState[state.id]);
    m_idToState.erase(state.id);    
    m_idToSState.erase(state.id);
    
}

void StateMachineWidget::removeSubState(const state_machine::serialization::State& state)
{
    std::cout << "Removing sub graph" << state.name << "(" << state.id << ")" << std::endl;
    //remove all attached states
    for(auto it: m_idToSState)
    {
        if(it.second.parentId == state.id && it.second.parentId != it.second.id)
        {
            std::cout << "Removing child state " << it.second.name << std::endl;
            removeState(it.second);
        }
    }
    
    //Delete subgraph
    m_scene.deleteSubGraph(m_idToSubGraph[state.id]);
    m_idToSubGraph.erase(state.id);
    
    //delete state itself
    removeState(state);
}


void StateMachineWidget::update(const state_machine::serialization::StateMachine& dump)
{
    std::cout << "Got Dump" << std::endl;
    bool changed = false;
    
    std::map<unsigned int, state_machine::serialization::State> idToSStateCpy = m_idToSState;
    std::map<unsigned int, const state_machine::serialization::State*>  idToSerState;
    
    std::vector<const state_machine::serialization::State *> toAddSub;
    std::vector<const state_machine::serialization::State *> toAdd;

    //first we check for all substates
    for(const state_machine::serialization::State &state: dump.allStates)
    {
        idToSerState[state.id] = &state;

        if(state.parentId != state.id)
            continue;
        
        bool add = true;
        if(m_idToSState.count(state.id))
        {
            const state_machine::serialization::State &oldState(m_idToSState[state.id]);
            if(state.name != oldState.name || state.parentId != oldState.parentId)
            {
                removeSubState(oldState);
            }
            else
            {
                add = false;
            }
            
            //state is registered, remove from the deletion list
            idToSStateCpy.erase(state.id);
        }

        if(add)
        {
            toAddSub.push_back(&state);
        }
    }

    //now that all states of the substates might have been deleted, we check the normal states
    for(const state_machine::serialization::State &state: dump.allStates)
    {
        if(state.parentId == state.id)
            continue;
        
        bool add = true;
        if(m_idToSState.count(state.id))
        {
            const state_machine::serialization::State &oldState(m_idToSState[state.id]);
            if(state.name != oldState.name || state.parentId != oldState.parentId)
            {
                removeState(oldState);
            }
            else
            {
                add = false;
            }
            
            //state is registered, remove from the deletion list
            idToSStateCpy.erase(state.id);
        }

        if(add)
        {
            toAdd.push_back(&state);
        }
    }
    
    //delete all states, that are not metioned any more in the new dump
    for(const auto &it: idToSStateCpy)
    {
        removeState(it.second);
    }
    
    
    for(const state_machine::serialization::State *state: toAddSub)
    {
        changed = true;
        std::cout << "Adding subGraph " << state->name << std::endl;
        
        if (!m_idToSubGraph.count(state->parentId))
        {
            if(state->id != state->parentId)
            {
                throw std::runtime_error("Expected parent state, got child state");
            }
            QGVSubGraph *subGraph = m_scene.addSubGraph(QString::fromStdString(state->name + std::to_string(state->id)), true);
            subGraph->setAttribute(QString::fromStdString("pin"), QString::fromStdString("true"));
            subGraph->setAttribute(QString::fromStdString("label"), QString::fromStdString(state->name));
            m_idToSubGraph[state->parentId] = subGraph;
            m_idToState[state->id] = m_scene.addNode(QString::fromStdString(state->name));
            m_idToSState[state->id] = *state;
        }
        else
        {
            throw std::runtime_error("Parent State to add exists");
        }
    }

    for(const state_machine::serialization::State *state: toAdd)
    {
        changed = true;
        std::cout << "Adding state " << state->name << std::endl;
        
        if (state->id != state->parentId) {
            if(m_idToState.count(state->id))
            {
                throw std::runtime_error("State to add exists");                
            }
            std::cout << " to parent " << m_idToSState[state->parentId].name << std::endl;
            m_idToState[state->id] = m_idToSubGraph[state->parentId]->addNode(QString::fromStdString(state->name));
            m_idToSState[state->id] = *state;
            
        } else {
            throw std::runtime_error("Expected child state, got parent");
        }
    }

    std::map<unsigned int, QGVEdge *> idToTransitionCpy;

    
    for(auto &tr: dump.allTransitions)
    {
        if(!m_idToTransition.count(tr.id))
        {
            std::cout << "Adding transition " << tr.name << std::endl;
            changed = true;

            if(!m_idToState.count(tr.from.id))
            {
                std::cout <<"Fishy transition, from id " << tr.from.id << " does not exist" << std::endl;
                continue;
            }
            if(!m_idToState.count(tr.to.id))
            {
                std::cout <<"Fishy transition, to id " << tr.to.id << " does not exist" << std::endl;
                continue;
            }
            
            m_idToTransition[tr.id] = m_scene.addEdge(m_idToState[tr.from.id], m_idToState[tr.to.id], QString::fromStdString(tr.name));
            m_transitions.push_back(tr);
        }
    }

    if(changed)
    {
        //Layout scene
        m_scene.applyLayout();
    }
}

void StateMachineWidget::wheelEvent(QWheelEvent* e) {
    double z = (e->delta()/120) * 0.1;
    scale(1+z, 1+z);
}