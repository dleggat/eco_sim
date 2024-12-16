#pragma once

#include <iostream>
#include "Animals.hpp"
#include <cmath>

namespace utils{
  void printLandTile(LandType landtype);
  bool checkAnimalCollision(std::pair<int,int> location, std::vector<Animal*> animals);
  float dotProduct(std::pair<int,int> x, std::pair<int,int> y);
  float angleCalc(std::pair<int,int> x, std::pair<int,int> y);
  float magnitude(std::pair<int,int> x);
  float distanceBetween(std::pair<int,int> x, std::pair<int,int> y);
};
