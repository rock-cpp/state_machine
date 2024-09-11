# CMakeLists.txt has to be located in the project folder and cmake has to be
# executed from 'project/build' with 'cmake ../'.
cmake_minimum_required(VERSION 3.1)
project(state_machine VERSION 0.1 DESCRIPTION "A simple state machine library")
find_package(Rock)
rock_init()
rock_feature(NOCURDIR)
rock_standard_layout()
