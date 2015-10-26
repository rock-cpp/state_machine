find_package( Boost COMPONENTS system filesystem)
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
        yaml-cpp
    DEPS_PLAIN 
        Boost_SYSTEM Boost_FILESYSTEM 

)

rock_executable(state_machine_bin Main.cpp
    DEPS state_machine)

