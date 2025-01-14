#include "Board.hpp"
#include "utils.hpp"

Board::Board(size_t size_x, size_t size_y):
  _size_x(size_x), _size_y(size_y)
{

  //Initialise all to ground
  for (int i = 0; i < _size_x * _size_y; i++){
    landTiles.push_back(LandType::Ground);
    animalTiles.push_back(NULL);
  }

  // Initialise random generators
  std::random_device rd;
  rand_gen = std::mt19937(rd());

  get_rand_full_x = std::uniform_int_distribution<uint>(0,_size_x-1);
  get_rand_full_x = std::uniform_int_distribution<uint>(0,_size_y-1);
  four_ways = std::uniform_int_distribution<int>(0,3);

  moveVector = {
    std::pair<int,int>(-1,0),
    std::pair<int,int>(1,0),
    std::pair<int,int>(0,-1),
    std::pair<int,int>(0,1)
  };

  selfAndSurround = moveVector;
  selfAndSurround.push_back(std::pair<int,int>(0,0));
  
}; // constructor

void Board::printBoard(){

  for (int i = 0; i < _size_x; i++){
    for (int j = 0; j < _size_y; j++){
      utils::printLandTile(landTiles[i * _size_x + j]);
    }
    std::cout << std::endl;
  }
  
}; //printBoard

bool Board::isLegal(std::pair<int,int> location,std::vector<LandType> forbidden){
  return (_coordInBounds(location)
	  && std::find(forbidden.begin(), forbidden.end(), getTileTypeAt(location.first, location.second))
	  == forbidden.end()
	  && !getAnimalAt(location)
	  );
};

bool Board::adjacentContains(std::pair<int,int> location, std::vector<LandType> types){
  // Includes the square we're standing on
  bool containsSearch = false;
  for (auto & move: selfAndSurround){
    std::pair<int,int> newLoc = std::pair<int,int>(location.first+move.first,location.second+move.second);
    if (!_coordInBounds(newLoc)) continue;
    if (std::find(types.begin(), types.end(), getTileTypeAt(newLoc.first,newLoc.second)) != types.end()){
      containsSearch = true;
      break;
    }
  }
  return containsSearch;
};

std::vector<std::pair<int,int>> Board::getLegalMoves(std::pair<int,int> location,std::vector<LandType> forbidden){
  std::vector<std::pair<int,int>> legalMoves;
  for (auto & move: moveVector){
    std::pair<int,int> newLoc = std::pair<int,int>(location.first+move.first,location.second+move.second);
    if (isLegal(newLoc,forbidden)) legalMoves.push_back(newLoc);
  }
  return legalMoves;
}; //Board::getLegalMoves

std::pair<int, int> Board::getRandomAdjacentTile(std::pair<int,int> location,std::vector<LandType> forbidden){
  // Get a tile next to the current location, excluding land types in the forbidden list
  std::vector<std::pair<int,int>> legalMoves = getLegalMoves(location,forbidden);
  if (legalMoves.size() == 0) return std::pair<int,int>(-1,-1);
  int selection;
  std::random_device rd;
  rand_gen = std::mt19937(rd());
  do {
    selection = four_ways(rand_gen);
  } while (selection >= legalMoves.size());
  return legalMoves[selection];
  
}; // Board::getRandomAdjacentTile

std::pair<int, int> Board::getRandomFreeTile(){

  std::uniform_int_distribution<uint> get_rand_x(0,_size_x-1);
  std::uniform_int_distribution<uint> get_rand_y(0,_size_y-1);
  
  int tile_x,tile_y;
  do {
    tile_x = get_rand_x(rand_gen);
    tile_y = get_rand_y(rand_gen);
  } while (landTiles[tile_x * _size_x + tile_y] != LandType::Ground);

  return std::pair<int, int>(tile_x, tile_y);
}; // Board::getRandomFreeTile

void Board::populateLandTypes(LandType type, int nTiles){

  std::pair<int, int> tile_coords;
  for (int i = 0; i < nTiles; i++){
    tile_coords = getRandomFreeTile();
    landTiles[tile_coords.first * _size_x + tile_coords.second] = type;
  }
}; //populateLandTypes

bool Board::_coordInBounds(std::pair<int,int> coord){
  return ( coord.first >= 0 && coord.first < _size_x
	   && coord.second >= 0 && coord.second < _size_y);
}; //coordInBounds

void Board::placeAnimalAt(std::pair<int,int> location, Animal* animal, bool addToMap){
  animalTiles[location.first * _size_x + location.second] = animal;
  if (addToMap){
    _animalMap[animal->getID()] = animal;
  }
}; //Board::placeAnimalAt()

void Board::removeAnimalFrom(std::pair<int,int> location, bool removeFromMap){
  if (removeFromMap){
    _animalMap.erase(animalTiles[location.first * _size_x + location.second]->getID());
  }
  animalTiles[location.first * _size_x + location.second] = NULL;
}

void Board::removeFromMap(int id){
  if (_animalMap.count(id) == 1){
    std::cout << "removing " << id << std::endl;
    _animalMap.erase(id);
  }
  else {
    std::cout << "Animal count error for " << id << " counts " << _animalMap.count(id) << std::endl;
  }
  std::cout << _animalMap.count(id) << std::endl;
}

void Board::addPond(int nTiles){

  std::vector<std::pair<int,int> > pondTiles;
  pondTiles.push_back(getRandomFreeTile());
  //  landTiles[pond_start.first * _size_x + pond_start.second] = LandType::Water;
  
  
  std::pair<int,int> tileCoords;
  
  for (int i = 1; i < nTiles; i++){
    do {
      std::uniform_int_distribution<int> getRandTile(0,pondTiles.size()-1);
      tileCoords = pondTiles[getRandTile(rand_gen)];
      switch (four_ways(rand_gen))
	{
	case 0:
	  tileCoords.first -= 1;
	  break;
	case 1:
	  tileCoords.first += 1;
	  break;
	case 2:
	  tileCoords.second -= 1;
	  break;
	case 3:
	  tileCoords.second += 1;
	  break;
	default:
	  break;
	}
    } while (landTiles[tileCoords.first * _size_x + tileCoords.second] != LandType::Ground
	     || std::find(pondTiles.begin(), pondTiles.end(), tileCoords) != pondTiles.end()
	     || !_coordInBounds(tileCoords));
    pondTiles.push_back(tileCoords);
  }
  for (auto & tile: pondTiles){
    landTiles[tile.first * _size_x + tile.second]  = LandType::Water;
  }
}; //addPond

std::pair<int, int> Board::getFreeTile(std::vector<Animal*> animals){

  std::pair<int,int> location;
  do {
    location = getRandomFreeTile();
  } while (utils::checkAnimalCollision(location,animals));

  return location;
}; //getFreeTile                                                                 

std::pair<int,int> Board::plotMoveTowards(std::pair<int,int> start, std::pair<int,int> end, std::vector<LandType> forbidden){
  if (start == end) return start;
  std::vector<std::pair<int,int>> legalMoves = getLegalMoves(start,forbidden);
  std::pair<int,int> vectorDirection = std::pair<int,int>(end.first - start.first, end.second - start.second);
  // If there are no legal moves, stay still
  if (legalMoves.size() == 0) return start;
  std::pair<int,int> bestDirection;
  float minimumAngle = 100.;
  for (auto & move: legalMoves){
    std::pair<int,int> moveVector(move.first - start.first, move.second - start.second);
    float angle = utils::angleCalc(vectorDirection,moveVector);
    if (angle < minimumAngle){
      minimumAngle = angle;
      bestDirection = move;
    }
  }
  return bestDirection;
};

std::vector<Animal*> Board::getAnimals(){
  std::vector<Animal*> animals;
  for (auto & animalTile: animalTiles){
    if (animalTile) animals.push_back(animalTile);
  }
  return animals;
}

void Board::_removeBuffered(){
  for (auto id: _idsToDelete){
    delete _animalMap[id];
    _animalMap.erase(id);
  }
}

void Board::updateBoardTimestep(){
  // remove the buffered animals
  _removeBuffered();
  // plausibly this is also where we could change where food if we decide to do that
}
