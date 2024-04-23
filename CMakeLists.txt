cmake_minimum_required(VERSION 3.22)
project(TPs)

set(CMAKE_CXX_STANDARD 17)

find_package(MPI REQUIRED)

add_executable(TP2exercise1 TP-2/exercise-1/main.cpp)
add_executable(TP2exercise2 TP-2/exercise-2/main.cpp)
add_executable(TP3exercise3 TP-3/exercise-3/main.cpp)
add_executable(TP3exercise4 TP-3/exercise-4/main.cpp)
add_executable(TP3exercise5 TP-3/exercise-5/main.cpp)

target_link_libraries(TP3exercise3 PRIVATE MPI::MPI_CXX)
target_link_libraries(TP3exercise4 PRIVATE MPI::MPI_CXX)
target_link_libraries(TP3exercise5 PRIVATE MPI::MPI_CXX)