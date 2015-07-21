rock_library(state_machine
    SOURCES 
        Config.cpp
        State.cpp
        Events.cpp
    HEADERS 
        Config.hpp
        State.hpp
        Events.hpp
    DEPS_PKGCONFIG 
        base-types
)

rock_executable(state_machine_bin Main.cpp
    DEPS state_machine)

