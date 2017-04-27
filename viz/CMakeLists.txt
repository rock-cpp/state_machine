
rock_vizkit_widget(StateMachineVisualization
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


