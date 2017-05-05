
rock_vizkit_widget(state_machine_viz
    SOURCES 
        StateMachineWidget.cpp
        StateMachineWidgetPlugin.cpp
    HEADERS 
        StateMachineWidget.hpp
    MOC
        StateMachineWidget.hpp
        StateMachineWidgetPlugin.hpp
    DEPS_PKGCONFIG
        QGVCore
        base-types
)


rock_executable(vizTest vizTest.cpp
    DEPS state_machine_viz)
