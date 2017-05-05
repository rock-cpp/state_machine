#include <QApplication>
#include <state_machine/StateMachineWidget.hpp>

int main(int argc, char** argv)
{
    QApplication app(argc, argv);
    
    StateMachineWidget widget;
    
    widget.show();

    state_machine::serialization::StateMachine dump;
    state_machine::serialization::State state1;
    state1.name = "state1";
    state1.parentId = 1;
    state1.id = 1;
    dump.allStates.push_back(state1);

    widget.update(dump);
    
    state_machine::serialization::State state2;
    state2.name = "Foo";
    state2.parentId = 1;
    state2.id = 2;
    
    dump.allStates.push_back(state2);
    widget.update(dump);
    
    state_machine::serialization::Transition tr;
    tr.from = state1;
    tr.to = state2;
    tr.name = "FooTrans";
    tr.id = 0;
    
    dump.allTransitions.push_back(tr);
    
    widget.update(dump);

    dump.allStates.resize(1);
    dump.allTransitions.clear();
    
    widget.update(dump);

    
    app.exec();
    

        
    return 0;
}
  