#pragma once

#include <cstddef> 
#include <cstdlib> 
#include "Animals.hpp"
#include "Board.hpp"
#include <algorithm>
#include <map>

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
  static const int _pollTimesteps = 100;
  void _pollAnimalState(std::vector<Animal*> animals);
  std::map<std::string, std::map<std::string, std::vector<std::vector<float> > > >  _geneInformation;
};
