find_package( Boost COMPONENTS system filesystem)
SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DOROCOS_TARGET=gnulinux" )
rock_library(state_machine
    SOURCES 
        Config.cpp
        State.cpp
        InitState.cpp
        NetworkState.cpp
        StateMachine.cpp
        Transition.cpp
        Events.cpp
        RPCCall.cpp
    HEADERS 
        Config.hpp
        State.hpp
        StateMachine.hpp;
        Transition.hpp
        Events.hpp
        RPCCall.hpp
        InitState.hpp
        NetworkState.hpp
    DEPS_PKGCONFIG 
        base-types
        yaml-cpp
    DEPS_PLAIN 
        Boost_SYSTEM Boost_FILESYSTEM 

)

rock_executable(state_machine_bin Main.cpp
    DEPS state_machine)

