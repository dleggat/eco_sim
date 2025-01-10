#include "EcoSim.hpp"
#include "utils.hpp"
#include <unistd.h>

#include <chrono>
#include <thread>

using namespace std::literals::chrono_literals;

EcoSim::EcoSim(Board board):
  _board(board)
{

};

void EcoSim::runSimulation(int timeSteps){
  // Main loop - probably wants long for longer simulations?
  for (int i = 0; i < timeSteps; i++){
    int j = 0;
    for (auto & animal: _board.getAnimals()){
      animal->updateTimestep(&_board);
    }
    if (i % _printTimesteps == 0){
      printState();
    }
    if (i % _pollTimesteps == 0){
      _pollAnimalState(_board.getAnimals());
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
  std::cout << "Rabbits: " << Rabbit::rabbitPopulation << " " << _board.getAnimals().size() << std::endl;
    
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
