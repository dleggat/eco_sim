#include "EcoSim.hpp"
#include "Animals.hpp"
#include "utils.hpp"
#include <random>

#include <vector>

char* getCmdOption(char ** begin, char ** end, const std::string & option){
  if (begin == end) return 0;
  std::cout << option << std::endl;
  char ** itr = std::find(begin, end, option);  
  if (itr != end && ++itr != end){
    return *itr;
  }
  return 0;
}

void printHelp(){
  std::cout << "FirstSim usage ./bin/FirstSim [args]" << std::endl;
  std::cout << " Options:" << std::endl;
  std::cout << " -o: output directory for creating plots of the simulation" << std::endl;
  std::cout << " -t: number of timesteps to run the simulation for " << std::endl;
}

int main(int argc, char * argv[]){

  // Parse command lines args
  // Help message
  if (getCmdOption(argv, argv+argc, "-h")){
    printHelp();
    return 0;
  }
  
  char * outDir = getCmdOption(argv, argv+argc, "-o");
  char * timestepsCLI = getCmdOption(argv, argv+argc, "-t");
  
  
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

  int timeSteps = 1000;
  if (timestepsCLI){
    timeSteps = atoi(timestepsCLI);
  }

  simulation.runSimulation(timeSteps);

  if (outDir){
    simulation.makePlots(outDir);
  }
  
  return 0;
}
