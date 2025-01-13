#include "EcoSim.hpp"
#include "utils.hpp"
#include <unistd.h>

#include <chrono>
#include <thread>
#include <filesystem>

using namespace std::literals::chrono_literals;

EcoSim::EcoSim(Board board):
  _board(board)
{

};

void EcoSim::runSimulation(int timeSteps){
  // Main loop - probably wants long for longer simulations?
  for (int i = 0; i < timeSteps; i++){
    int j = 0;
    for (auto & animal: _getAnimals()){
      animal->updateTimestep(&_board);
    }
    if (i % _printTimesteps == 0){
      printState();
    }
    if (i % _pollTimesteps == 0){
      _pollAnimalState(_getAnimals());
    }
    std::this_thread::sleep_for(10ms);
  }
}; //runSimulation

void EcoSim::printState(){
  for (int i = 0; i < _board.sizeX(); i++){
    for (int j = 0; j < _board.sizeY(); j++){
      if (_board.getAnimalAt(std::pair<int,int>(i,j))){
	std::cout << _board.getAnimalAt(std::pair<int,int>(i,j))->printAnimal();
      }
      else utils::printLandTile(_board.getTileTypeAt(i,j));
      //}
    }
    std::cout << std::endl;
  }
  std::cout << "Rabbits: " << Rabbit::rabbitPopulation << " Foxes: " << Fox::foxPopulation << " " << _board.getAnimals().size() << std::endl;
    
}

void EcoSim::setAnimals(std::vector<Animal*> *animals){
  for (auto & animal: *animals){
    _board.placeAnimalAt(animal->getLocation(),animal);
    //Prime our gene information storage whilst we're here
    if (!_geneInformation.count(animal->getAnimalName())){
      std::map<std::string, std::vector<std::vector<float> > > animalGeneMap;
      _geneInformation.insert({animal->getAnimalName(), animalGeneMap});
      for (auto myPair : animal->pollGenes()){
	std::vector<std::vector<float> > geneVector;
	_geneInformation[animal->getAnimalName()].insert({myPair.first,geneVector});
      }
    }
  }
};

void EcoSim::_pollAnimalState(std::vector<Animal*> animals){

  //Initialise this timestep's vectors
  for (auto & animalPair: _geneInformation){
    for (auto & genePair: animalPair.second){
      std::vector<float> thisTimestepGenes;
      genePair.second.push_back(thisTimestepGenes);
    }
  }

  // Now sample the data
  for (auto & animal: animals){
    auto genes = animal->pollGenes();
    for (auto gene: genes){
      _geneInformation[animal->getAnimalName()][gene.first].back().push_back(gene.second);
    }
  }
}

void EcoSim::makePlots(std::string outputDirectory){

  std::cout << "Entering plotting routine" << std::endl;
  
  // Make our output directory if we don't have it already
  if (!std::filesystem::exists(outputDirectory)) std::filesystem::create_directories(outputDirectory);

  for (auto animalMap: _geneInformation){
    std::cout << animalMap.first << std::endl;
    bool plotPop = true;
    for (auto geneMap: animalMap.second){
      std::cout << geneMap.first << std::endl;
      _processOneGene(outputDirectory, animalMap.first, geneMap.first, geneMap.second, plotPop);
      plotPop = false;
    }
  }
  
}

void EcoSim::_processOneGene(std::string outputDirectory, std::string animal, std::string gene, std::vector<std::vector<float>> vecOfVecs, bool plotPop){

  std::vector<float> means;
  std::vector<float> stdDevs;
  std::vector<int> population;
  
  for (auto timestepVec: vecOfVecs){
    float sum = 0.0;
    for (auto datum: timestepVec){
      sum += datum;
    }
    float mean = sum / timestepVec.size();

    float stdDev = 0.0;
    for (auto datum: timestepVec){
      stdDev += pow(datum - mean, 2);
    }
    means.push_back(mean);
    stdDevs.push_back(stdDev);
    population.push_back(timestepVec.size());
  }
  if (plotPop){
    _plotPopulation(animal+"_population", outputDirectory, population);
  }
  _plotGeneAverage(means, stdDevs, animal+"_"+gene, outputDirectory);
}

void EcoSim::_plotPopulation(std::string plotName, std::string outputDirectory, std::vector<int> population){
  std::vector<double> x = matplot::linspace(0,population.size(),population.size());
  matplot::plot(x,population);
  matplot::title(plotName);
  std::string saveName = outputDirectory + "/" + plotName + ".png";  
  matplot::save(saveName);
}

void EcoSim::_plotGeneAverage(std::vector<float> means, std::vector<float> sigmas, std::string plotName, std::string outputDirectory){
  // Make one plot
  std::vector<double> x = matplot::linspace(0,means.size(),means.size());
  matplot::errorbar(x,means,sigmas);
  matplot::title(plotName);
  std::string saveName = outputDirectory + "/" + plotName + ".png";
  matplot::save(saveName);
}
