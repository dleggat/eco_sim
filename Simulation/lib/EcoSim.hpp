#pragma once

#include <cstddef> 
#include <cstdlib> 
#include "Animals.hpp"
#include "Board.hpp"
#include <algorithm>
#include <map>
#include <cmath>

#include <matplot/matplot.h>

class EcoSim{
public:
  EcoSim(Board board);
  ~EcoSim() {};
  void setAnimals(std::vector<Animal*> *animals);
  void runSimulation(int timeSteps);
  void printState();
  void makePlots(std::string outputDirectory);
private:
  Board _board;
  static const int _printTimesteps = 10;
  static const int _pollTimesteps = 100;
  void _pollAnimalState(std::vector<Animal*> animals);
  void _processOneGene(std::string outputDirectory, std::string animal, std::string gene, std::vector<std::vector<float>> vecOfVecs, bool plotPop);
  void _plotGeneAverage(std::vector<float> averages, std::vector<float> sigmas, std::string plotName, std::string outputDirectory);
  void _plotPopulation(std::string plotName, std::string outputDirectory, std::vector<int> population);
  std::map<std::string, std::map<std::string, std::vector<std::vector<float> > > >  _geneInformation;
  inline std::vector<Animal*> _getAnimals() {return _board.getAnimals();};
};
