add_executable(FirstSim FirstSim.cpp)
find_package(Matplot++ REQUIRED)
target_link_libraries(FirstSim PUBLIC SimulationLib Matplot++::matplot)
