#include "utils.hpp"

void utils::printLandTile(LandType landtype){
  switch(landtype){  
  case LandType::Ground:
    std::cout << (unsigned char)46 << (unsigned char)46;
    break;
  case LandType::Water:
    std::cout << (unsigned char)126 << (unsigned char)126;
    break;
  case LandType::Bush:
    std::cout << (unsigned char)118 << (unsigned char)118;
    break;
  case LandType::Food:
    std::cout << (unsigned char)94 << (unsigned char)94;
    break;
  default:
    std::cout << "?";
    break;
  }
}; //printLandTile

bool utils::checkAnimalCollision(std::pair<int,int> location, std::vector<Animal*> animals){
  for (auto & animal: animals){
    if (location == animal->getLocation()) return true;
  }
  return false;
};

float utils::angleCalc(std::pair<int,int> x, std::pair<int,int> y){
  return acos(dotProduct(x,y)/(magnitude(x) * magnitude(y)));
};

float utils::dotProduct(std::pair<int,int> x, std::pair<int,int> y){
  return (float)(x.first * y.first + x.second * y.second);
};

float utils::magnitude(std::pair<int,int> x){
  return sqrt((x.first * x.first) + (x.second * x.second));
};

float utils::distanceBetween(std::pair<int,int> x, std::pair<int,int> y){
  return (sqrt((x.first-y.first)*(x.first-y.first) + (x.second-y.second)*(x.second-y.second)));
};
