add_library(SimulationLib STATIC EcoSim.cpp Animals.cpp Board.cpp utils.cpp)
find_package(Matplot++ REQUIRED)
target_include_directories(SimulationLib PUBLIC "${CMAKE_SOURCE_DIR}/Simulation/lib" Matplot++::matplot)
