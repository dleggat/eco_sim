#pragma once

#include <utility>
#include <iostream>
#include <string>
#include "Board.hpp"
#include <cmath>

class Board;
enum struct LandType;

class Animal
{
public:
  Animal();
  Animal(std::pair<int,int> location);
  std::pair <int, int>  getLocation() { return _location; };
  int getX() { return _location.first; };
  int getY() { return _location.second; };
  std::string getAnimalName() { return _animalName; };
  std::string printAnimal() { return _animal_print; };
  void updateTimestep(Board * board, std::vector<Animal*> * animals);
  bool isHorny() {return this->_state == AnimalState::Horny;};
  
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
  int _movementIncrement;
  int _internalCounter;
  // Amount internal states change
  float _energyDelta;
  float _thirstDelta;
  float _horniness;
  //Properties that we could potentially evolve
  int _sightRange;
  // For monitoring board
  std::string _animal_print;
  std::string _animalName;
  // Where the animal is
  std::pair<int, int> _location;
  // Land the animal cannot move onto
  std::vector<LandType> _forbiddenLand;
  virtual AnimalState defineState() = 0;
  virtual void runBehaviour(Board board, std::vector<Animal*> animals) = 0;

  // Movement methods
  bool existsLegalMoves(Board board);
  void moveOneRandom(Board board, std::vector<Animal*> animals, std::vector<LandType> forbidden);
  void moveAnimal(std::pair<int,int> newLocation);

  // Search methods
  std::pair<int,int> searchFor(Board board, LandType search);
  template <typename F>
  Animal* findClosestAnimal(std::string animalType, std::vector<Animal*> animals,  F* validityFunc = 0);
private:
  bool _checkLife();
};

class Rabbit: public Animal{
public:
  Rabbit();
  Rabbit(std::pair<int,int> start_location);
private:
  AnimalState defineState();
  void runBehaviour(Board board, std::vector<Animal*> animals);
  // Different behaviours
  void _idleBehaviour(Board board,  std::vector<Animal*> animals);
  void _thirstyBehaviour(Board board,  std::vector<Animal*> animals);
  void _hungryBehaviour(Board board,  std::vector<Animal*> animals);
  void _hornyBehaviour(Board board,  std::vector<Animal*> animals);
};
