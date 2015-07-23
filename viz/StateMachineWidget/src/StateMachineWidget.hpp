#ifndef TESTWIDGET_HPP
#define TESTWIDGET_HPP

#include <QtGui>
#include "StateMachineView.hpp"

class StateMachineWidget : public QWidget
{
    Q_OBJECT

public:
    StateMachineWidget(QWidget *parent = 0);
    virtual ~StateMachineWidget();

public slots:
    void update(const state_machine::serialization::StateMachine &dump);
    void update(const state_machine::serialization::Event &event);

private:
    StateMachineView *view;
};

#endif /* TESTWIDGET_HPP */
