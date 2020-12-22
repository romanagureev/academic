cmake_minimum_required(VERSION 3.12)
project(TSP)

set(CMAKE_CXX_STANDARD 17)

add_executable(TSP main.cpp Tests/TestsGenerator.cpp PlaneGraph.h Point.h Graph.h PlaneGraph.cpp)