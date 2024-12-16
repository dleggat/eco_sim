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
    for (auto & animal: _animals){
      animal->updateTimestep(&_board,&_animals);
    }
    if (i % _printTimesteps == 0){
      printState();
    }
    std::this_thread::sleep_for(10ms);
  }
}; //runSimulation

void EcoSim::printState(){
  bool printed = false;
  for (int i = 0; i < _board.sizeX(); i++){
    for (int j = 0; j < _board.sizeY(); j++){
      for (auto & animal: _animals){
	printed = false;
	if (animal->getLocation() == std::pair<int,int>(i,j)){
	  std::cout << animal->printAnimal();
	  printed = true;
	  break;
	}
      }
      //      for (auto animal: animals){
      if (!printed) utils::printLandTile(_board.getTileTypeAt(i,j));
      //}
    }
    std::cout << std::endl;
  }
  for (int i = 0; i < _board.sizeX(); i++){
    for (int j = 0; j < _board.sizeY(); j++){
      std::pair<int,int> boardCoord(i,j);
      if (_board.getAnimalAt(boardCoord)){
	std::cout << i <<"," << j << " ";
      }
    }
  }
  std::cout << std::endl;
    
}

void EcoSim::setAnimals(std::vector<Animal*> *animals){
  _animals = *animals;
  for (auto & animal: *animals){
    _board.placeAnimalAt(animal->getLocation(),animal);
  }
};
