#include "StateMachineWidget.hpp"
#include <state_machine/Events.hpp>
#include <qgv/qgv.h>
#include <QMainWindow>
#include <qgv/QGVScene.h>
#include <qgv/QGVEdge.h>
#include <qgv/QGVNode.h>
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
    m_scene.applyLayout();
}

void StateMachineWidget::update(const state_machine::serialization::StateMachine& dump)
{
    //delete old contents
    m_scene.clear();
    m_idToState.clear();
    m_idToTransition.clear();
    m_idToSubGraph.clear();
    std::map<unsigned int, const state_machine::serialization::State*>  idToSerState;
    
    for(const state_machine::serialization::State &state: dump.allStates)
    {
        idToSerState[state.id] = &state;
        if (!m_idToSubGraph.count(state.parentId))
        {
            m_idToSubGraph[state.parentId] = m_scene.addSubGraph(QString::fromStdString(state.name + std::to_string(state.id)), true);
        }
    }

    for(const state_machine::serialization::State &state: dump.allStates)
    {
        if (state.id != state.parentId) {
            m_idToState[state.id] = m_idToSubGraph[state.parentId]->addNode(QString::fromStdString(state.name));
        } else {
            m_idToState[state.id] = m_scene.addNode(QString::fromStdString(state.name));
        }
    }

    for(auto &it : m_idToSubGraph)
    {
        it.second->setAttribute(QString::fromStdString("label"), QString::fromStdString(idToSerState[it.first]->name));
    }
    
    for(auto &tr: dump.allTransitions)
    {
        m_idToTransition[tr.id] = m_scene.addEdge(m_idToState[tr.from.id], m_idToState[tr.to.id], QString::fromStdString(tr.name));
    }

    //Layout scene
    m_scene.applyLayout();
}

void StateMachineWidget::wheelEvent(QWheelEvent* e) {
    double z = (e->delta()/120) * 0.1;
    scale(1+z, 1+z);
}