cmake_minimum_required(VERSION 3.24)
project(ParallelProgrammingMPI)

set(CMAKE_CXX_STANDARD 17)

# Add MPI Package to Project
find_package(MPI REQUIRED)

add_executable(ParallelProgrammingMPI main.cpp)
# Add libraries for code completion and compiling
target_link_libraries(ParallelProgrammingMPI PUBLIC MPI::MPI_CXX)
