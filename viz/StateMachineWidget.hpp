#pragma once

#include <QGraphicsView>
#include <qgv/QGVScene.h>
#include <state_machine/Events.hpp>
#include <qgv/QGVSubGraph.h>
#include <map>
#include <qevent.h>

class StateMachineWidget : public QGraphicsView
{
    Q_OBJECT
public:
    StateMachineWidget(QWidget* parent = 0);
    using QGraphicsView::update;

public slots:    
    void update(const state_machine::serialization::StateMachine &dump);   
    void update(const state_machine::serialization::Event &event);
    
protected:
    void wheelEvent(QWheelEvent* e);
    
private:
    int m_activeTransition;
    int m_activeState;
    std::map<unsigned int, QGVNode *> m_idToState;
    std::map<unsigned int, QGVEdge *> m_idToTransition;
    std::map<unsigned int, QGVSubGraph *> m_idToSubGraph;

    QGVScene m_scene;
};

