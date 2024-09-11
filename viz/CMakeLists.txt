
rock_find_qt4(OPTIONAL)

if(ROCK_QT_VERSION_4)
    rock_vizkit_widget(state_machine_viz
        SOURCES
            StateMachineWidget.cpp
            StateMachineWidgetPlugin.cpp
        HEADERS
            StateMachineWidget.hpp
        MOC
            StateMachineWidget.hpp
            StateMachineWidgetPlugin.hpp
        DEPS
            state_machine
        DEPS_PKGCONFIG
            QGVCore
            base-types
    )


    rock_executable(vizTest vizTest.cpp
        DEPS state_machine_viz)
endif()
