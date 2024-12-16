#include "Animals.hpp"
#include "utils.hpp"

Animal::Animal():
  Animal(std::pair<int,int>(0.,0.))
{
};

Animal::Animal(std::pair<int,int> location):
  _location(location), _energyDelta(0.02), _thirstDelta(0.004), _energy(1.0), _thirst(0.0), _sightRange(3), _state(AnimalState::Idle), _horn(0.)
{
};

void Animal::updateTimestep(Board * board, std::vector<Animal*> * animals){
  // Update thirst tracker always
  this->_thirst += _thirstDelta;

  //Update internal counter, and run behaviour if we're at the animal's count
  if (++_internalCounter % _movementIncrement != 0) return;

  _internalCounter = 0;

  // Remove animal from board whilst we do its behaviour
  board->removeAnimalFrom(getLocation());
  
  _state = defineState();

  // And run animal specific behaviour based on it
  runBehaviour(*board);

  if (!_checkLife()){
    std::cout << "Animal has died!" << std::endl;
    animals->erase(std::remove(animals->begin(),animals->end(), this), animals->end());
  }
  else {
    board->placeAnimalAt(getLocation(),this);
  }

  
  
}; //Animal::udpateTimestep

bool Animal::_checkLife(){
  bool isAlive = true;
  if (_thirst > 1.0) {
    std::cout << "Thirst!" << std::endl;
    isAlive = false;
  }
  if (_energy < 0.){
    std::cout << "Hunger!" << std::endl;
    isAlive = false;
  }
  return isAlive;
};

bool Animal::existsLegalMoves(Board board){
  std::vector<std::pair<int,int>> possibleMoves = board.getLegalMoves(_location,_forbiddenLand);
  if (possibleMoves.size() == 0) return false;
  return true;
};

void Animal::moveOneRandom(Board board, std::vector<LandType> forbidden){
  //Check that we have a legal move, or we'll get stuck forever
  if (!existsLegalMoves(board)) return;

  // Now find our new location
  std::pair<int,int> newLocation = board.getRandomAdjacentTile(_location, forbidden);
  moveAnimal(newLocation);
}; //Animal::moveOneRandom

void Animal::moveAnimal(std::pair<int,int> newLocation){
  // Move animal and subtract energy
  _location = newLocation;
  _energy -= _energyDelta;
}; //Animal::moveToLocation

std::pair<int,int> Animal::searchFor(Board board, LandType searchFor){
  // This will find just the closest land tile of type (within the animals search diameter?)
  // This *can* be overloaded by derived classes if we want animals to sense differently
  float closestRange = 10000000.;
  std::pair<int,int> closestLocation;
  std::pair<int,int> tempLoc;
  for (auto & sightSpot: _sightGrid){
    tempLoc = std::pair<int,int>(_location.first+sightSpot.first, _location.second+sightSpot.second);
    if (!board._coordInBounds(tempLoc)) continue;
    if (board.getTileTypeAt(tempLoc.first, tempLoc.second) != searchFor) continue;
    float range = utils::distanceBetween(_location,tempLoc);
    if (range < closestRange){
      closestLocation = tempLoc;
      closestRange = range;
    }
  }
  if (closestRange < 1000000){
    return closestLocation;
  }
  return std::pair<int,int>(-1,-1);

}; // Animal::searchFor

template <typename F>
Animal* Animal::findClosestAnimal(Board board, std::string animalType,  F* validityFunc){
  // Basic closest neighbour search. I know there are better ways to do this, but who has
  // time to makes trees and do pruning?
  Animal* returnAnimal = NULL;
  float closestAnimal = 1000000.;
  std::pair<int,int> closestLocation;
  std::pair<int,int> tempLoc;
  for (auto & sightSpot: _sightGrid){
    tempLoc = std::pair<int,int>(_location.first+sightSpot.first, _location.second+sightSpot.second);
    if (!board._coordInBounds(tempLoc)) continue;
    Animal * tmpAnimal = board.getAnimalAt(tempLoc);
    if (!tmpAnimal || tmpAnimal->getAnimalName() != animalType) continue;
    if (validityFunc){
      if (!(*validityFunc)(tmpAnimal)) continue;
    }    
    float distance = utils::distanceBetween(_location,tmpAnimal->getLocation());
    if (distance < closestAnimal){
      closestAnimal = distance;
      returnAnimal = tmpAnimal;
    }
  }
  return returnAnimal;
}; //Animal::findClosestAnimal

Rabbit::Rabbit():
  Animal(std::pair<int, int>(0.,0.))
{
};

Rabbit::Rabbit(std::pair<int,int> location):
  Animal(location)
  //  _location(location), _movementIncrement(10), _energyDelta(0.02)
{
  this->_animal_print = "db";
  this->_movementIncrement = 10;
  this->_forbiddenLand = {LandType::Water};
  this->_sightRange = 6;
  this->_animalName = "rabbit";
  this->_horniness = 0.1;
  for (int i = -_sightRange; i < _sightRange + 1; i++){
    for (int j = -_sightRange; j < _sightRange + 1; j++){
      //skip outside of the sight range
      float range = sqrt((i*i)+(j*j));
      if (floor(range) > _sightRange) continue;
      this->_sightGrid.push_back(std::pair<int,int>(i,j));
    }
  }

};

void Rabbit::runBehaviour(Board board){
  std::pair<int,int> moveLocation;
  switch (_state){
  case AnimalState::Idle:
    _idleBehaviour(board);
    break;
  case AnimalState::Thirsty:
    _thirstyBehaviour(board);
    break;
  case AnimalState::Hungry:
    _hungryBehaviour(board);
    break;
  case AnimalState::Horny:
    _hornyBehaviour(board);
    break;
  default:
    break;
  }
}; //Rabbit::runBehaviour

Animal::AnimalState Rabbit::defineState(){
  // For now, let's just return idle
  if (_thirst > 0.5) return AnimalState::Thirsty;
  if (_energy < 0.5) return AnimalState::Hungry;
  // If we aren't hungry or thirsty, the rabbit starts getting horny
  this->_horn+=this->_horniness;
  if (this->_horn > 0.5) return AnimalState::Horny;
  return AnimalState::Idle;

}; //Rabbit::_defineState

void Rabbit::_idleBehaviour(Board board){
  
  moveOneRandom(board,this->_forbiddenLand);
}; //Rabbit::_idleBehaviour

void Rabbit::_thirstyBehaviour(Board board){
  std::pair<int,int> nearestWater = searchFor(board, LandType::Water);
  if (nearestWater.first < 0){
    moveOneRandom(board,this->_forbiddenLand);
  }
  else{
    std::pair<int,int> moveLocation = board.plotMoveTowards(_location,nearestWater,this->_forbiddenLand);
    moveAnimal(moveLocation);
  }
  if (board.adjacentContains(_location,{LandType::Water})){
    this->_thirst = 0.;
  }
  
};

void Rabbit::_hungryBehaviour(Board board){
  std::pair<int,int> nearestFood = searchFor(board, LandType::Food);
  if (nearestFood.first < 0){
    moveOneRandom(board,this->_forbiddenLand);
  }
  else{
    std::pair<int,int> moveLocation = board.plotMoveTowards(_location,nearestFood,this->_forbiddenLand);
    moveAnimal(moveLocation);
  }
  if (board.adjacentContains(_location,{LandType::Food})){
    this->_energy = 1.;
  }
};

void Rabbit::_hornyBehaviour(Board board){
  auto hornyLambda = [](Animal* animal){ return animal->isHorny(); };
  Animal* closestRabbit = findClosestAnimal(board, "rabbit", &hornyLambda );
  if (closestRabbit == NULL
      || utils::distanceBetween(closestRabbit->getLocation(), _location) > _sightRange
      ){
    moveOneRandom(board,this->_forbiddenLand);
  }
  else{
    //    std::pair<int,int> targetLocation = findClosestAnimal->getLocation();
    //    moveOneRandom(board,animals,this->_forbiddenLand);
  }
};
