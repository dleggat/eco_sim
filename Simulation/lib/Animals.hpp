#pragma once

#include <utility>
#include <iostream>
#include <string>
#include "Board.hpp"
#include <cmath>
#include <map>

class Board;
enum struct LandType;

class Animal
{
public:
  Animal();
  Animal(std::pair<int,int> location);
  Animal(std::pair<int,int> location, float thirstThreshold, float energyThreshold, float hornyThreshold, float horniness, float movementInc);
  virtual ~Animal() = default;
  std::pair <int, int>  getLocation() { return _location; };
  int getX() { return _location.first; };
  int getY() { return _location.second; };
  std::string getAnimalName() { return _animalName; };
  std::string printAnimal() { return _animal_print; };
  void updateTimestep(Board * board);
  bool isHorny() {return this->_state == AnimalState::Horny;};
  int getID() {return _uniqueId;};
  std::map<std::string,float> pollGenes();

  static inline int animalID = 1;
  
  enum struct AnimalState{
    Idle,
    Hungry,
    Thirsty,
    Horny
  };
  
protected:
  float _energy;
  //  float _hunger;
  float _thirst; //What does thirst effect?!
  float _horn;
  // Animal state
  AnimalState _state;
  // The amount of time ticks between movements
  float _movementIncrement;
  int _internalCounter;
  // Amount internal states change
  float _energyDelta;
  float _smallEnergyDelta;
  float _thirstDelta;
  float _horniness;
  float _babyEnergy;
  //Properties that we could potentially evolve
  int _sightRange;
  float _thirstThreshold;
  float _energyThreshold;
  float _hornyThreshold;
  // For use with sight/identification
  std::vector<std::pair<int,int>> _sightGrid;
  // For monitoring board
  std::string _animal_print;
  std::string _animalName;
  // Where the animal is
  std::pair<int, int> _location;
  // Land the animal cannot move onto
  std::vector<LandType> _forbiddenLand;
  virtual AnimalState defineState() = 0;
  virtual void runBehaviour(Board * board) = 0;

  // Behaviour methods. We will define default ones that can be overridden by inheretted classes
  void _idleBehaviour(Board board);
  void _thirstyBehaviour(Board board);
  
  // Movement methods
  bool existsLegalMoves(Board board);
  void moveOneRandom(Board board, std::vector<LandType> forbidden);
  void moveAnimal(std::pair<int,int> newLocation);

  // Search methods
  std::pair<int,int> searchFor(Board board, LandType search);
  template <typename F = bool(Animal*)>
  Animal* findClosestAnimal(Board board, std::string animalType,  F* validityFunc = 0);
  template <typename F>
  static void _mateAnimals(Board * board, Animal * animalOne, Animal * animalTwo, F* babyFunc);

  // Static methods
  static float _mutateAllele(float first, float second, float minimumValue = 0.);
private:
  bool _checkLife();
  const int _uniqueId;

};

class Rabbit: public Animal{
public:
  Rabbit();
  Rabbit(std::pair<int,int> startLocation);
  Rabbit(std::pair<int,int> location, float thirstThreshold, float energyThreshold, float hornyThreshold, float horniness, float movementInc);

  ~Rabbit() {rabbitPopulation--;};
  static inline int rabbitPopulation = 0;
private:
  AnimalState defineState();
  void runBehaviour(Board * board);
  // Different behaviours
  void _hungryBehaviour(Board board);
  void _hornyBehaviour(Board * board);
};

class Fox: public Animal{
public:
  Fox();
  Fox(std::pair<int,int> startLocation);
  Fox(std::pair<int,int> location, float thirstThreshold, float energyThreshold, float hornyThreshold, float horniness, float movementInc);

  ~Fox() {foxPopulation--;};
  static inline int foxPopulation = 0;
private:
  AnimalState defineState();
  void runBehaviour(Board * board);
  //behaviours
  void _hungryBehaviour(Board * board);
  void _hornyBehaviour(Board * board);
};
