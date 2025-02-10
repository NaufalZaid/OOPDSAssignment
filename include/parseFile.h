#ifndef PARSEFILE_H
#define PARSEFILE_H

#include "GameManager.h"
#include <string>
#include <vector>

// A structure to hold config data if desired
struct GameConfig {
  int iterations;
  int width;
  int height;

  // For battlefield
  int terrainGrid[10][10]; // or dynamically allocated if width/height vary

  // For teams and ships
  struct ShipInfo {
    std::string type;   // e.g. "Battleship"
    std::string symbol; // e.g. "*"
    int count;          // e.g. 5
    std::string team;   // e.g. "A"
  };

  // Collection of all ships to create
  std::vector<ShipInfo> allShips;
};

/**
 * A simple parser class that reads the given file and returns a GameConfig
 */
class GameParser {
public:
  GameParser() = default;
  ~GameParser() = default;

  // Parse the file and fill a GameConfig structure
  GameConfig parseFile(const std::string &filename);
};

#endif // PARSEFILE_H
