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
  }
};
