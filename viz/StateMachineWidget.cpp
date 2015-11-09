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

StateMachineWidget::StateMachineWidget(): QGraphicsView(), scene("StateMachine")
{
    setScene(&scene);

    scene.setGraphAttribute("splines", "ortho");
    scene.setGraphAttribute("rankdir", "LR");
    //_scene.setGraphAttribute("concentrate", "true"); //Error !
    scene.setGraphAttribute("nodesep", "0.4");
    scene.setGraphAttribute("style", "filled");
    scene.setGraphAttribute("fillcolor", "#a99fa5");

    scene.setNodeAttribute("shape", "box");
    scene.setNodeAttribute("style", "filled");
    scene.setNodeAttribute("fillcolor", "#806969");
    scene.setNodeAttribute("height", "1.2");

    scene.setEdgeAttribute("minlen", "3");
    scene.setEdgeAttribute("pad", "0.2000");

    activeState = -1;
    activeTransition = -1;
}


void StateMachineWidget::update(const state_machine::serialization::Event &event)
{
    switch(event.type)
    {
    case state_machine::serialization::StateChanged:
    {
        if(activeState >= 0)
        {
            idToState[activeState]->setAttribute("color", "black");
            idToState[activeState]->setAttribute("fillcolor", "#806969");
        }

        auto s = idToState.find(event.id);
        if(s != idToState.end())
        {
            activeState = event.id;
            idToState[activeState]->setAttribute("color", "black");
            idToState[activeState]->setAttribute("fillcolor", "yellow");
            idToState[activeState]->setFocus();

        }
        else
        {
            std::cout << "Error state " << event.id << " not found" << std::endl;
        }
    }
    break;
    case state_machine::serialization::TransitionTriggered:
    {
        if(activeTransition >= 0)
        {
            idToTransition[activeTransition]->setAttribute("color", "black");
            idToTransition[activeTransition]->setAttribute("arrowsize", "1.2");
        }
        auto trans = idToTransition.find(event.id);
        if(trans != idToTransition.end())
        {
            activeTransition = event.id;
            idToTransition[activeTransition]->setAttribute("color", "red");
            idToTransition[activeTransition]->setAttribute("arrowsize", "2.0");
        }
        else
        {
            std::cout << "Error unknown transition " << event.id << std::endl;
        }
    }
    break;
    }

    //Layout scene
    scene.applyLayout();
}

void StateMachineWidget::update(const state_machine::serialization::StateMachine& dump)
{
    //delete old contents
    scene.clear();
    idToState.clear();
    idToTransition.clear();
    idToSubGraph.clear();

    for(const state_machine::serialization::State &state: dump.allStates)
    {
        if (!idToSubGraph.count(state.parentId))
        {
            idToSubGraph[state.parentId] = scene.addSubGraph(QString::fromStdString(state.name + std::to_string(state.id)), true);
            idToSubGraph[state.parentId]->setAttribute(QString::fromStdString("label"), QString::fromStdString(state.name));
        }
    }

    for(const state_machine::serialization::State &state: dump.allStates)
    {
        if (state.id != state.parentId) {
            idToState[state.id] = idToSubGraph[state.parentId]->addNode(QString::fromStdString(state.name));
        } else {
            idToState[state.id] = scene.addNode(QString::fromStdString(state.name));
        }

    }

    for(auto &tr: dump.allTransitions)
    {
        idToTransition[tr.id] = scene.addEdge(idToState[tr.from.id], idToState[tr.to.id], QString::fromStdString(tr.name));
    }

    //Layout scene
    scene.applyLayout();
}

void StateMachineWidget::wheelEvent(QWheelEvent* e) {
    double z = (e->delta()/120) * 0.1;
    scale(1+z, 1+z);
}