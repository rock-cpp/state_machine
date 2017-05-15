#pragma once

#include <QGraphicsView>
#include <qgv/QGVScene.h>
#include <state_machine/Events.hpp>
#include <map>
#include <qevent.h>
#include <graph_analysis/gui/BaseGraphView/BaseGraphView.hpp>

class StateMachineWidget : public QGraphicsView
{
    Q_OBJECT
public:
    StateMachineWidget(QWidget* parent = 0);
    using QGraphicsView::update;

public slots:    
    void update(const state_machine::serialization::StateMachine &dump);   
    void updateDump(const state_machine::serialization::StateMachine &dump)
    {
        update(dump);
    };
    void update(const state_machine::serialization::Event &event);
    void updateEvents(const state_machine::serialization::Event &event)
    {
        update(event);
    };
    
protected:
    void wheelEvent(QWheelEvent* e);
    
    void setDefaultAttributes();
    
    void removeState(const state_machine::serialization::State &state);
    void removeParentState(const state_machine::serialization::State &state);
    
private:
    int m_activeTransition;
    int m_activeState;
    std::map<unsigned int, QGVNode *> m_idToQGVNode;
    std::map<unsigned int, QGVEdge *> m_idToQGVEdge;
    std::map<unsigned int, QGVSubGraph *> m_idToQGVSubGraph;

    std::map<unsigned int, state_machine::serialization::State> m_idToParentState;
    std::map<unsigned int, state_machine::serialization::State> m_idToChildState;
    std::map<unsigned int, state_machine::serialization::Transition> m_idToTransitions;
    
    QGVScene m_scene;
};

