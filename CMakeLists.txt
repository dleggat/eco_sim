cmake_minimum_required(VERSION 3.16)
project(eco_sim
  VERSION 0.0.1
  LANGUAGES CXX
)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# You can set this flag to `-g` when developing to enable debugging
set(CMAKE_CXX_FLAGS "-g")

add_subdirectory(app)
add_subdirectory(Simulation)
