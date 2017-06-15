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
#include <functional>

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
        if(m_activeState >= 0 && m_idToQGVNode.count(m_activeState))
        {
            m_idToQGVNode[m_activeState]->setAttribute("color", "black");
            m_idToQGVNode[m_activeState]->setAttribute("fillcolor", "#806969");
        }

        auto s = m_idToQGVNode.find(event.id);
        if(s != m_idToQGVNode.end())
        {
            m_activeState = event.id;
            m_idToQGVNode[m_activeState]->setAttribute("color", "black");
            m_idToQGVNode[m_activeState]->setAttribute("fillcolor", "yellow");
            m_idToQGVNode[m_activeState]->setFocus();

        }
        else
        {
            std::cout << "Error state " << event.id << " not found" << std::endl;
        }
    }
    break;
    case state_machine::serialization::TransitionTriggered:
    {
        if(m_activeTransition >= 0 && m_idToQGVEdge.count(m_activeTransition))
        {
            m_idToQGVEdge[m_activeTransition]->setAttribute("color", "black");
            m_idToQGVEdge[m_activeTransition]->setAttribute("arrowsize", "1.2");
        }
        auto trans = m_idToQGVEdge.find(event.id);
        if(trans != m_idToQGVEdge.end())
        {
            m_activeTransition = event.id;
            m_idToQGVEdge[m_activeTransition]->setAttribute("color", "red");
            m_idToQGVEdge[m_activeTransition]->setAttribute("arrowsize", "2.0");
        }                    
        else
        {
            std::cout << "Error unknown transition " << event.id << std::endl;
        }
    }
    break;
    }

    //Layout scene
    m_scene.applyLayout();
}

void StateMachineWidget::removeState(const state_machine::serialization::State& state)
{
    std::cout << "Removing state" << state.name << "(" << state.id << ")" << std::endl;
    
    //first check if we are connected to some edge
    for(auto it = m_idToTransitions.begin();it !=  m_idToTransitions.end();)
    {
        const auto &tr(it->second);
        if(tr.from.id == state.id || tr.to.id == state.id)
        {
            std::cout << "Removing transition " << tr.name << " from " << tr.from.name << "(" << tr.from.id << ") to " << tr.to.name << "(" << tr.to.id << ")" << std::endl;
            m_scene.deleteEdge(m_idToQGVEdge[tr.id]);
            m_idToQGVEdge.erase(tr.id);
            it = m_idToTransitions.erase(it);
        }
        else
        {
            it++;
        }
    }
    
    m_scene.deleteNode(m_idToQGVNode[state.id]);
    m_idToQGVNode.erase(state.id);    
    m_idToChildState.erase(state.id);
    
}

void StateMachineWidget::removeParentState(const state_machine::serialization::State& state)
{
    std::cout << "Removing sub graph" << state.name << "(" << state.id << ")" << std::endl;
    //remove all attached states
    for (auto it: m_idToParentState)
    {
        std::cout << "check " << it.second.name << ", id=" << std::to_string(it.second.id) << ", parent=" << std::to_string(it.second.parentId) << std::endl;
        if (it.second.id != state.id && it.second.parentId == state.id)
        {
            std::cout << "Removing parent state " << it.second.name << std::endl;
            removeParentState(it.second);
        }
    }
    
    for (auto it: m_idToChildState)
    {
        if (it.second.parentId == state.id && it.second.parentId != it.second.id)
        {
            std::cout << "Removing child state " << it.second.name << std::endl;
            removeState(it.second);
        }
    }
    
    //Delete subgraph
    m_scene.deleteSubGraph(m_idToQGVSubGraph[state.id]);
    m_idToQGVSubGraph.erase(state.id);
    m_idToParentState.erase(state.id);
    
    //delete state itself
    removeState(state);
}

bool checkUpdate(const std::map<unsigned int, state_machine::serialization::State> &idToState, 
                 const std::vector<state_machine::serialization::State> &newStates,
                 std::function<void (const state_machine::serialization::State &)> removeFunc,
                 std::function<void (const state_machine::serialization::State *)> addFunc
                )
{
    std::map<unsigned int, state_machine::serialization::State> deletionList = idToState;
    std::vector<const state_machine::serialization::State *> addList;
    
    bool changed = false;
    
    for(const state_machine::serialization::State &state: newStates)
    {
        bool add = true;
        if(idToState.count(state.id))
        {
            const state_machine::serialization::State &oldState(idToState.at(state.id));
            if(state == oldState)
            {
                //state is registered, remove from the deletion list
                deletionList.erase(state.id);
                add = false;
            }
            //else state is different, keep it in the deletion list for deletion and readd the new state
        }

        if(add)
        {
            addList.push_back(&state);
        }
    }
    
    //delete all states, that are not metioned any more in the new dump
    for(const auto &it: deletionList)
    {
        changed = true;
        removeFunc(it.second);
    }

    for(const state_machine::serialization::State *state: addList)
    {
        changed = true;
        addFunc(state);
    }
    
    return changed;
}

void StateMachineWidget::update(const state_machine::serialization::StateMachine& dump)
{
    std::cout << "Got Dump" << std::endl;
    bool changed = false;
    
    std::vector<state_machine::serialization::State> parentStates;
    std::vector<state_machine::serialization::State> childStates;

    for(const state_machine::serialization::State &state: dump.allStates)
    {
        // check if state is its on parent OR parent of an onther state
        if (state.parentId == state.id
            || std::find_if(dump.allStates.begin(), dump.allStates.end(), [&state] (const state_machine::serialization::State& s) { return s.parentId == state.id; }) !=  dump.allStates.end())
        {
            parentStates.push_back(state);
            std::cout << "add parent state " << state.name << " id=" << std::to_string(state.id) << ", parent=" << std::to_string(state.parentId) << std::endl;
        }
        else
        {
            childStates.push_back(state);
        }
    }
    
    changed |= checkUpdate(m_idToParentState, parentStates, std::bind(&StateMachineWidget::removeParentState, this, std::placeholders::_1), [&](const state_machine::serialization::State *state){
        std::cout << "Adding subGraph " << state->name << std::endl;
        
        if (!m_idToQGVSubGraph.count(state->id))
        {
            QGVSubGraph *subGraph = m_scene.addSubGraph(QString::fromStdString(state->name + std::to_string(state->id)), true);
            
            subGraph->setAttribute(QString::fromStdString("pin"), QString::fromStdString("true"));
            subGraph->setAttribute(QString::fromStdString("label"), QString::fromStdString(state->name));
            m_idToQGVSubGraph[state->id] = subGraph;
            
            // case: state is parent of an other node
            if (state->parentId != state->id)
            {
                m_idToQGVNode[state->id] = subGraph->addNode(QString::fromStdString(state->name));
            }
            // case: state is parent of itself
            else
            {
                m_idToQGVNode[state->id] = m_scene.addNode(QString::fromStdString(state->name));
            }
            
            m_idToParentState[state->id] = *state;
        }
        else
        {
            throw std::runtime_error("Parent State to add exists");
        }

    });

    changed |= checkUpdate(m_idToChildState, childStates, std::bind(&StateMachineWidget::removeState, this, std::placeholders::_1), [&](const state_machine::serialization::State *state){ 
        std::cout << "Adding state " << state->name << std::endl;
        
        if (state->id != state->parentId) {
            if(m_idToQGVNode.count(state->id))
            {
                throw std::runtime_error("State to add exists");                
            }
            m_idToQGVNode[state->id] = m_idToQGVSubGraph[state->parentId]->addNode(QString::fromStdString(state->name));
            m_idToChildState[state->id] = *state;
            
        } else {
            throw std::runtime_error("Expected child state, got parent");
        }
    });

    std::map<unsigned int, state_machine::serialization::Transition> deleteList = m_idToTransitions;
    
    for(auto &tr: dump.allTransitions)
    {
        auto it = m_idToTransitions.find(tr.id);
        if(it != m_idToTransitions.end())
        {
            if(it->second == tr)
            {
                deleteList.erase(tr.id);
            }
        }
        else
        {
            //new transition, add it
            std::cout << "Adding transition " << tr.name << "("<< tr.id << ")" << std::endl;
            changed = true;
            
            if(!m_idToQGVNode.count(tr.from.id))
            {
                std::cout << "Error, transition source not there (" << tr.from.id << ")"  << std::endl;
            }
                
            if(!m_idToQGVNode.count(tr.to.id))
            {
                std::cout << "Error, transition goal not there (" << tr.to.id << ")"  << std::endl;
            }
            
            m_idToQGVEdge[tr.id] = m_scene.addEdge(m_idToQGVNode[tr.from.id], m_idToQGVNode[tr.to.id], QString::fromStdString(tr.name));
            m_idToTransitions[tr.id] = tr;
        }
    }
    
    for(auto &it: deleteList)
    {
        if(!m_idToQGVEdge.count(it.second.id))
        {
            throw std::runtime_error("Transition to delete does not exist");
        }
        std::cout << "Deleting transition " << it.second.name << "("<< it.second.id << ")" << std::endl;
        m_scene.deleteEdge(m_idToQGVEdge.at(it.second.id));
        m_idToTransitions.erase(it.first);
        changed = true;
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