rock_library(state_machine
    SOURCES Dummy.cpp
    HEADERS Dummy.hpp)

rock_executable(state_machine_bin Main.cpp
    DEPS state_machine)

