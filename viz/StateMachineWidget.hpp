#ifndef STATEMACHINEWIDGET_H
#define STATEMACHINEWIDGET_H

#include <QGraphicsView>
#include <qgv/QGVScene.h>
#include <state_machine/Events.hpp>
#include <qgv/QGVSubGraph.h>
#include <map>
#include <qevent.h>

class StateMachineWidget : public QGraphicsView
{
public:
    StateMachineWidget();
    
    void update(const state_machine::serialization::StateMachine &dump);   
    void update(const state_machine::serialization::Event &event);
    
protected:
    void wheelEvent(QWheelEvent* e);
    
private:
    int activeTransition;
    int activeState;
    std::map<unsigned int, QGVNode *> idToState;
    std::map<unsigned int, QGVEdge *> idToTransition;
    std::map<unsigned int, QGVSubGraph *> idToSubGraph;

    QGVScene scene;
};

#endif // STATEMACHINEWIDGET_H
