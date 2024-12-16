#include "EcoSim.hpp"
#include "Animals.hpp"
#include "utils.hpp"

#include <vector>

int main(){

  // Make the board and set up some land types
  Board board = Board(16,16);
  board.addPond(10);
  board.populateLandTypes(LandType::Water, 3);
  board.populateLandTypes(LandType::Food, 10);
  board.populateLandTypes(LandType::Bush, 10);
  //  board.printBoard();
  
  EcoSim simulation(board);

  // Make animals
  std::vector<Animal *> animals;
  
  // Just a couple of rabbits for now
  int nRabbits = 6;
  for (int i = 0; i < nRabbits; i++){
    animals.push_back(new Rabbit(board.getFreeTile(animals)));
  }

  simulation.setAnimals(&animals);

  int timeSteps = 1000;
  
  simulation.runSimulation(timeSteps);
  
  return 0;
}
