rock_library(state_machine
    SOURCES 
        Config.cpp
        State.cpp
        StateMachine.cpp
        Transition.cpp
        Events.cpp
    HEADERS 
        Config.hpp
        State.hpp
        StateMachine.hpp;
        Transition.hpp
        Events.hpp
    DEPS_PKGCONFIG 
        base-types
)

rock_executable(state_machine_bin Main.cpp
    DEPS state_machine)

