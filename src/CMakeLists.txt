rock_library(state_machine
    SOURCES 
        Config.cpp
        State.cpp
    HEADERS 
        Config.hpp
        State.hpp
    DEPS_PKGCONFIG 
        base-types
)

rock_executable(state_machine_bin Main.cpp
    DEPS state_machine)

