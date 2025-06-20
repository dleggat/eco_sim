#pragma once

#include <cstddef>
#include <cstdlib>
#include <iostream>
#include <random>
#include <algorithm>
#include "Animals.hpp"
#include <map>

class Animal;

enum struct LandType{
  Ground,
  Water,
  Bush,
  Food
};

class Board
{
public:
  Board(std::size_t size_x, std::size_t size_y);
  //  ~Board() { free(landTiles); };

  void updateBoardTimestep();
  LandType getTileTypeAt(std::size_t x, std::size_t y) { return landTiles[x * _size_x + y]; };
  LandType getTileTypeAt(std::pair<int,int> location) { return landTiles[location.first * _size_x + location.second]; };
  void printBoard();
  void populateLandTypes(LandType type, int nTiles);
  void addPond(int nTiles);
  // Utilities for getting free tiles
  bool isLegal(std::pair<int,int> location,std::vector<LandType> forbidden);
  std::vector<std::pair<int,int>> getLegalMoves(std::pair<int,int> location,std::vector<LandType> forbidden);
  std::pair<int, int> getRandomFreeTile();
  std::pair<int, int> getRandomAdjacentTile(std::pair<int,int> location,std::vector<LandType> forbidden);
  std::pair<int, int> getFreeTile(std::vector<Animal*> animals);
  // Get moves towards things
  std::pair<int,int> plotMoveTowards(std::pair<int,int> start, std::pair<int,int> end, std::vector<LandType> forbidden, bool moveAway = false);
  int sizeX() {return _size_x;};
  int sizeY() {return _size_y;};
  bool adjacentContains(std::pair<int,int> location, std::vector<LandType> types);
  bool _coordInBounds(std::pair<int,int> coord);
  // Place and remove animal from tiles
  void placeAnimalAt(std::pair<int,int> location, Animal* animal, bool addToMap = false);
  void removeAnimalFrom(std::pair<int,int> location, bool removeFromMap = false);
  void bufferAnimalIDRemoval(int id){_idsToDelete.push_back(id);};
  bool isAnimalRemovalBuffered(int id){return (std::find(_idsToDelete.begin(),_idsToDelete.end(),id) != _idsToDelete.end());};
  void removeFromMap(int id);
  Animal* getAnimalAt(std::pair<int,int> location) { return animalTiles[location.first * _size_x + location.second]; };
  std::vector<Animal*> getAnimals();
  std::map<int,Animal*>* getAnimalMap(){return &_animalMap;};
private:
  std::vector<LandType> landTiles;
  std::vector<Animal*> animalTiles;
  int _size_x, _size_y;
  std::mt19937 rand_gen;
  
  // Random generators
  std::uniform_int_distribution<uint> get_rand_full_x;
  std::uniform_int_distribution<uint> get_rand_full_y;
  std::uniform_int_distribution<int> four_ways;
  std::vector<std::pair<int,int>> moveVector;
  std::vector<std::pair<int,int>> selfAndSurround;

  std::map<int,Animal*> _animalMap;
  // To delete animals we have to make a temp store of ids to be deleted
  std::vector<int> _idsToDelete;
  void _removeBuffered();
};
