#include "EcoSim.hpp"
#include "Animals.hpp"
#include "utils.hpp"
#include <random>

#include <vector>

int main(){

  // Make the board and set up some land types
  Board board = Board(20,20);
  board.addPond(10);
  board.populateLandTypes(LandType::Water, 3);
  board.populateLandTypes(LandType::Food, 10);
  board.populateLandTypes(LandType::Bush, 10);
  //  board.printBoard();
  
  EcoSim simulation(board);

  // Make animals
  std::vector<Animal *> animals;

  float thirstyT = 0.5, energyT = 0.5, hornyT = 0.5, horniness = 0.1, movementInc = 10.;

  std::random_device rd;
  std::mt19937 rand_gen(rd());

  std::normal_distribution<float> thresholds(0.5,0.1);
  std::normal_distribution<float> horninessF(0.06,0.01);
  std::normal_distribution<float> rabbitMove(10.,0.5);  
  
  // Just a couple of rabbits for now
  int nRabbits = 6;
  for (int i = 0; i < nRabbits; i++){
    // Randomise allioles
    do {
      thirstyT = thresholds(rand_gen);
      energyT = thresholds(rand_gen);
      hornyT = thresholds(rand_gen);
      horniness = horninessF(rand_gen);
      movementInc = rabbitMove(rand_gen);
    } while (thirstyT < 0 || energyT < 0 || hornyT < 0 || horniness < 0 || movementInc < 0);
    animals.push_back(new Rabbit(board.getFreeTile(animals),thirstyT,energyT,hornyT,horniness,movementInc));
  }

  simulation.setAnimals(&animals);

  int timeSteps = 10000;
  
  simulation.runSimulation(timeSteps);

  simulation.makePlots("testOut");
  
  return 0;
}
