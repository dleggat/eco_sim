#pragma once

#include <cstddef> 
#include <cstdlib> 
#include "Animals.hpp"
#include "Board.hpp"
#include <algorithm>

class EcoSim{
public:
  EcoSim(Board board);
  ~EcoSim() {};
  void setAnimals(std::vector<Animal*> *animals);
  void runSimulation(int timeSteps);
  void printState();
private:
  Board _board;
  static const int _printTimesteps = 10;
};
