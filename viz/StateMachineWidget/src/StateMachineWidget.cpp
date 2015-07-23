#include "StateMachineWidget.hpp"

StateMachineWidget::StateMachineWidget(QWidget *parent)
    : QWidget(parent)
{

    view = new StateMachineView();

    QVBoxLayout *vbox = new QVBoxLayout(this);
    vbox->addWidget(view);

    setLayout(vbox);
    show();
}

StateMachineWidget::~StateMachineWidget()
{
}

void StateMachineWidget::update(const state_machine::serialization::StateMachine &dump)
{
    view->update(dump);
}

void StateMachineWidget::update(const state_machine::serialization::Event &event)
{
    view->update(event);
}
