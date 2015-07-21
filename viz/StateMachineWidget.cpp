#include "StateMachineWidget.hpp"
#include <state_machine/Events.hpp>
#include <qgv/qgv.h>
#include <QMainWindow>
#include <qgv/QGVScene.h>
#include <qgv/QGVEdge.h>
#include <qgv/QGVNode.h>
#include <QApplication>
#include <iostream>

StateMachineWidget::StateMachineWidget(): QGraphicsView(), scene("StateMachine")
{
    setScene(&scene);
    
    scene.setGraphAttribute("splines", "ortho");
    scene.setGraphAttribute("rankdir", "LR");
    //_scene.setGraphAttribute("concentrate", "true"); //Error !
    scene.setGraphAttribute("nodesep", "0.4");

    scene.setNodeAttribute("shape", "box");
    scene.setNodeAttribute("style", "filled");
    scene.setNodeAttribute("fillcolor", "white");
    scene.setNodeAttribute("height", "1.2");
    scene.setEdgeAttribute("minlen", "3");

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
            }
            
            auto s = idToState.find(event.id);
            if(s != idToState.end())
            {
                activeState = event.id;
                idToState[activeState]->setAttribute("color", "blue");
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
            }
            auto trans = idToTransition.find(event.id);
            if(trans != idToTransition.end())
            {
                activeTransition = event.id;
                idToTransition[activeTransition]->setAttribute("color", "red");
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

    //Fit in view
    fitInView(scene.sceneRect(), Qt::KeepAspectRatio);
}

void StateMachineWidget::update(const state_machine::serialization::StateMachine& dump)
{
    //delete old contents
    scene.clear();
    idToState.clear();
    idToTransition.clear();
    
    for(auto &state: dump.allStates)
    {
        idToState[state.id] = scene.addNode(QString::fromStdString(state.name));
    }
    
    for(auto &tr: dump.allTransitions)
    {
        idToTransition[tr.id] = scene.addEdge(idToState[tr.from.id], idToState[tr.to.id], QString::fromStdString(tr.name));
    }
    
    //Layout scene
    scene.applyLayout();

    //Fit in view
    fitInView(scene.sceneRect(), Qt::KeepAspectRatio);
}
