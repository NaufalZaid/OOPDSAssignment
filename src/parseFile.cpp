#include "parseFile.h"
#include <cctype> // std::isdigit
#include <fstream>
#include <iostream>
#include <sstream>
#include <stdexcept>

// Helper: create a symbol like "*1", "*2" if you need to differentiate each
// ship
static std::string buildSymbol(const std::string &baseSymbol, int index) {
  // e.g., baseSymbol="*", index=1 => "*1"
  // If your assignment literally wants '*1', '*2', then do:
  return baseSymbol + std::to_string(index);
}

GameConfig GameParser::parseFile(const std::string &filename) {
  std::ifstream fin(filename);
  if (!fin.is_open()) {
    throw std::runtime_error("Cannot open file: " + filename);
  }

  GameConfig config;
  // Initialize some defaults
  config.iterations = 100;
  config.width = 10;
  config.height = 10;
  // zero out terrain grid by default
  for (int i = 0; i < 10; i++) {
    for (int j = 0; j < 10; j++) {
      config.terrainGrid[i][j] = 0;
    }
  }

  // Read lines
  std::string line;
  while (std::getline(fin, line)) {
    if (line.empty()) {
      continue; // skip empty lines
    }

    std::istringstream iss(line);
    std::string token;
    iss >> token;

    if (token == "iterations") {
      iss >> config.iterations;
    } else if (token == "width") {
      iss >> config.width;
    } else if (token == "height") {
      iss >> config.height;
    } else if (token == "Team") {
      // Example: "Team A 4" => teamName="A", shipTypeCount=4
      std::string teamName;
      int shipTypeCount = 0;
      iss >> teamName >> shipTypeCount;

      // For the next 'shipTypeCount' lines, read type, symbol, count
      for (int i = 0; i < shipTypeCount; i++) {
        std::string shipType, symbol;
        int count = 0;
        if (!std::getline(fin, line)) {
          throw std::runtime_error(
              "Unexpected end while reading ships for team " + teamName);
        }
        std::istringstream shipsIss(line);
        shipsIss >> shipType >> symbol >> count;

        // Example: "Battleship * 5"
        // We'll add these to config.allShips
        GameConfig::ShipInfo info;
        info.type = shipType; // "Battleship"
        info.symbol = symbol; // "*"
        info.count = count;   // 5
        info.team = teamName; // "A"
        config.allShips.push_back(info);
      }
    } else if (std::isdigit(token[0]) || token == "0" || token == "1") {
      // This likely indicates the start of the battlefield grid lines
      // Because your example has 10 lines of "0 0 0 1..." etc.
      // We already have 'token' which should be the first int in row 0
      // We'll parse 'height' rows, each containing 'width' integers
      // Here we assume config.height=10, config.width=10
      fin.seekg(-static_cast<int>(line.size()) - 1, std::ios::cur);
      // Move file pointer back so we can re-read the line fully
      for (int r = 0; r < config.height; r++) {
        if (!std::getline(fin, line)) {
          throw std::runtime_error("Not enough lines for battlefield grid.");
        }
        std::istringstream gridIss(line);
        for (int c = 0; c < config.width; c++) {
          int val;
          gridIss >> val;
          config.terrainGrid[r][c] = val;
        }
      }
    } else {
      // Possibly unknown token; ignore or handle
      // std::cout << "Unknown token: " << token << std::endl;
    }
  } // end while

  fin.close();
  return config;
}
