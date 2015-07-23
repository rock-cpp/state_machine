#ifndef STATEMACHINEWIDGET_H
#define STATEMACHINEWIDGET_H

#include <QGraphicsView>
#include <qgv/QGVScene.h>
#include <state_machine/Events.hpp>
#include <map>

class StateMachineView : public QGraphicsView
{
public:
    StateMachineView();
    
public slots:
    void update(const state_machine::serialization::StateMachine &dump);
    void update(const state_machine::serialization::Event &event);
    
private:
    int activeTransition;
    int activeState;
    std::map<unsigned int, QGVNode *> idToState;
    std::map<unsigned int, QGVEdge *> idToTransition;

    QGVScene scene;
};

#endif // STATEMACHINEWIDGET_H
