#include "Animals.hpp"
#include "utils.hpp"

Animal::Animal():
  Animal(std::pair<int,int>(0.,0.))
{
};

Animal::Animal(std::pair<int,int> location):
  _location(location), _energyDelta(0.02), _thirstDelta(0.004), _energy(1.0), _thirst(0.0), _sightRange(3), _state(AnimalState::Idle), _horn(0.), _babyEnergy(0.4), _internalCounter(0), _smallEnergyDelta(0.004), _uniqueId(Animal::animalID++)
{
};

void Animal::updateTimestep(Board * board){
  // Update thirst tracker always
  this->_thirst += _thirstDelta;
  this->_energy -= _smallEnergyDelta;

  //Update internal counter, and run behaviour if we're at the animal's count
  if (++_internalCounter % int(_movementIncrement) != 0) return;
  
  _internalCounter = 0;

  // Remove animal from board whilst we do its behaviour
  board->removeAnimalFrom(getLocation());

  _state = defineState();

  // And run animal specific behaviour based on it
  runBehaviour(board);

  if (!_checkLife()){
    board->bufferAnimalIDRemoval(getID());
    return;

    delete this;
  }
  else {
    board->placeAnimalAt(getLocation(),this);
    return;
  }

  
  
}; //Animal::udpateTimestep

bool Animal::_checkLife(){
  bool isAlive = true;
  if (_thirst > 1.0) {
    //    std::cout << "Thirst!" << std::endl;
    isAlive = false;
  }
  if (_energy < 0.){
    // std::cout << "Hunger!" << std::endl;
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

float Animal::_mutateAllele(float first, float second, float minimumValue){
  float mean = (first + second) / 2.0;
  float stddev = abs(first-second);
  std::normal_distribution newAllele(mean,stddev);
  //This is clearly bad.
  std::random_device rd;
  std::mt19937 rand_gen(rd());
  float returnValue;
  do{
    returnValue = newAllele(rand_gen);
  } while (returnValue < minimumValue);
  return returnValue;
  //return 1.0;
  //do (
};

void Animal::_idleBehaviour(Board board){
  
  moveOneRandom(board,this->_forbiddenLand);
}; //Rabbit::_idleBehaviour

void Animal::_thirstyBehaviour(Board board){
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
  
} //Animal::_thirstyBehaviour

Rabbit::Rabbit():
  Animal(std::pair<int, int>(0.,0.))
{
};

Rabbit::Rabbit(std::pair<int,int> location):
  Rabbit(location,0.5,0.5,0.5,0.1,10)
{
};

Rabbit::Rabbit(std::pair<int,int> location, float thirstThreshold, float energyThreshold, float hornyThreshold, float horniness, float movementInc):
  Animal(location)
  //  _location(location), _movementIncrement(10), _energyDelta(0.02)
{
  this->_animal_print = "db";
  this->_movementIncrement = movementInc;
  this->_forbiddenLand = {LandType::Water};
  this->_sightRange = 6;
  this->_animalName = "rabbit";
  for (int i = -_sightRange; i < _sightRange + 1; i++){
    for (int j = -_sightRange; j < _sightRange + 1; j++){
      //skip outside of the sight range
      float range = sqrt((i*i)+(j*j));
      if (floor(range) > _sightRange) continue;
      this->_sightGrid.push_back(std::pair<int,int>(i,j));
    }
  }
  this->_horniness = horniness;
  this->_thirstThreshold = thirstThreshold;
  this->_energyThreshold = energyThreshold;
  this->_hornyThreshold = hornyThreshold;
  rabbitPopulation++;
};

void Rabbit::runBehaviour(Board * board){
  switch (_state){
  case AnimalState::Idle:
    this->_idleBehaviour(*board);
    break;
  case AnimalState::Thirsty:
    this->_thirstyBehaviour(*board);
    break;
  case AnimalState::Hungry:
    _hungryBehaviour(*board);
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
  if (_thirst > this->_thirstThreshold) return AnimalState::Thirsty;
  if (_energy < this->_energyThreshold) return AnimalState::Hungry;
  // If we aren't hungry or thirsty, the rabbit starts getting horny
  this->_horn+=this->_horniness;
  if (this->_horn > this->_hornyThreshold ) return AnimalState::Horny;
  return AnimalState::Idle;

}; //Rabbit::_defineState

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

void Rabbit::_hornyBehaviour(Board * board){
  auto hornyLambda = [](Animal* animal){ return animal->isHorny(); };
  Animal* closestRabbit = findClosestAnimal(*board, "rabbit", &hornyLambda );
  if (closestRabbit == NULL){
    moveOneRandom(*board,this->_forbiddenLand);
  }
  else{
    std::pair<int,int> targetLocation = closestRabbit->getLocation();
    std::pair<int,int> moveLocation = board->plotMoveTowards(_location,targetLocation,this->_forbiddenLand);
    moveAnimal(moveLocation);
    if (utils::distanceBetween(_location,closestRabbit->getLocation()) < 1.1){
      auto makeBabyRabbitLambda = [](std::pair<int,int> location, float thirstyT, float energyT, float hornyT, float horniness, float movementInc){ return new Rabbit(location,thirstyT,energyT,hornyT,horniness,movementInc); };
      Animal::_mateAnimals(board, (Animal*)this,closestRabbit,&makeBabyRabbitLambda);
    }
    //    moveOneRandom(board,animals,this->_forbiddenLand);
  }
};

template <typename F>
void Animal::_mateAnimals(Board * board, Animal * animalOne, Animal * animalTwo, F* babyFunc){

  float thirstyThreshold = _mutateAllele(animalOne->_thirstThreshold,animalTwo->_thirstThreshold);
  float energyThreshold = _mutateAllele(animalOne->_energyThreshold,animalTwo->_energyThreshold);
  float horninessThreshold = _mutateAllele(animalOne->_hornyThreshold,animalTwo->_hornyThreshold);
  float horniness = _mutateAllele(animalOne->_horniness,animalTwo->_horniness);
  float movementInc = _mutateAllele(animalOne->_movementIncrement,animalTwo->_movementIncrement, 1.0);

  std::pair<int,int> babyLocation = board->getRandomAdjacentTile(animalOne->getLocation(),animalOne->_forbiddenLand);
  // Only pro-create if there's room
  if (babyLocation.first < 0) return;
  
  Animal* babyAnimal = (*babyFunc)(babyLocation,thirstyThreshold,energyThreshold,horninessThreshold,horniness,movementInc);
  board->placeAnimalAt(babyLocation,babyAnimal,true);
  
  //reset animal states
  for (auto animal: {animalOne,animalTwo}){
    animal->_energy -= animal->_babyEnergy;
    animal->_horn = 0.;
    animal->_state = AnimalState::Idle;
  }
  
}; //Rabbit::_mateRabbit

Fox::Fox():
  Animal(std::pair<int, int>(0.,0.))
{
};

Fox::Fox(std::pair<int,int> location):
  Fox(location,0.6,0.6,0.6,0.05,8)
{
};

Fox::Fox(std::pair<int,int> location, float thirstThreshold, float energyThreshold, float hornyThreshold, float horniness, float movementInc):
  Animal(location)
  //  _location(location), _movementIncrement(10), _energyDelta(0.02)
{
  this->_animal_print = "\\/";
  this->_movementIncrement = movementInc;
  this->_forbiddenLand = {LandType::Water,LandType::Bush};
  this->_sightRange = 4;
  this->_animalName = "fox";
  for (int i = -_sightRange; i < _sightRange + 1; i++){
    for (int j = -_sightRange; j < _sightRange + 1; j++){
      //skip outside of the sight range
      float range = sqrt((i*i)+(j*j));
      if (floor(range) > _sightRange) continue;
      this->_sightGrid.push_back(std::pair<int,int>(i,j));
    }
  }
  this->_horniness = horniness;
  this->_thirstThreshold = thirstThreshold;
  this->_energyThreshold = energyThreshold;
  this->_hornyThreshold = hornyThreshold;
  this->_energyDelta = 0.01;
  foxPopulation++;
};

Animal::AnimalState Fox::defineState(){
  // For now, let's just return idle
  if (_energy < this->_energyThreshold) return AnimalState::Hungry;
  if (_thirst > this->_thirstThreshold) return AnimalState::Thirsty;
  // If we aren't hungry or thirsty, the rabbit starts getting horny
  this->_horn+=this->_horniness;
  if (this->_horn > this->_hornyThreshold ) return AnimalState::Horny;
  return AnimalState::Idle;

}; //Fox::_defineState

void Fox::runBehaviour(Board * board){
  switch (_state){
  case AnimalState::Idle:
    this->_idleBehaviour(*board);
    break;
  case AnimalState::Thirsty:
    this->_thirstyBehaviour(*board);
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

void Fox::_hungryBehaviour(Board * board){

  Animal* closestRabbit = findClosestAnimal(*board, "rabbit");
  if (!closestRabbit){
    moveOneRandom(*board, this->_forbiddenLand);
  }
  else{
    std::pair<int,int> targetLocation = closestRabbit->getLocation();
    std::pair<int,int> moveLocation = board->plotMoveTowards(_location,targetLocation,this->_forbiddenLand);
    moveAnimal(moveLocation);
    if (utils::distanceBetween(_location,closestRabbit->getLocation()) < 1.1){
      this->_energy = 1.0;
      board->removeAnimalFrom(closestRabbit->getLocation());
      board->bufferAnimalIDRemoval(closestRabbit->getID());
    }
  }
} // Fox::_hungryBehaviour

void Fox::_hornyBehaviour(Board * board){
  auto hornyLambda = [](Animal* animal){ return animal->isHorny(); };
  Animal* closestFox = findClosestAnimal(*board, "fox", &hornyLambda );
  if (closestFox == NULL){
    moveOneRandom(*board, this->_forbiddenLand);
  }
  else{
    std::pair<int,int> targetLocation = closestFox->getLocation();
    std::pair<int,int> moveLocation = board->plotMoveTowards(_location,targetLocation,this->_forbiddenLand);
    moveAnimal(moveLocation);
    if (utils::distanceBetween(_location,closestFox->getLocation()) < 1.1){
      auto makeFoxCubLambda = [](std::pair<int,int> location, float thirstyT, float energyT, float hornyT, float horniness, float movementInc){ return new Fox(location,thirstyT,energyT,hornyT,horniness,movementInc); };
      Animal::_mateAnimals(board, (Animal*)this,closestFox,&makeFoxCubLambda);
    }
    //    moveOneRandom(board,animals,this->_forbiddenLand);
  }
};

std::map<std::string,float> Animal::pollGenes(){
  return {
    { "Thirst", _thirstThreshold },
    { "Energy", _energyThreshold },
    { "Horny" , _hornyThreshold },
    { "Horniness", _horniness },
    { "Movement", _movementIncrement }
  };
}

