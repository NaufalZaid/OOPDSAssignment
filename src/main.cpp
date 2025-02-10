#include "GameManager.h"
#include "ShipTypes.h"
#include "parseFile.h"
#include <iostream>

int main(int argc, char *argv[]) {
  if (argc < 2) {
    std::cerr << "Usage: " << argv[0] << " <game_file.txt>\n";
    return 1;
  }

  try {
    // 1) Parse the config
    GameParser parser;
    GameConfig config = parser.parseFile(argv[1]);

    // 2) Create a GameManager
    GameManager manager;
    //    Set the battlefield terrain
    manager.setBattlefieldTerrain(config.terrainGrid);

    // 3) Create and add ships
    //    For each ShipInfo in config.allShips
    for (auto &info : config.allShips) {
      // For each ship in the count
      for (int i = 0; i < info.count; i++) {
        // Build a unique symbol, e.g. "*1", "*2", etc.
        std::string uniqueSymbol = info.symbol;
        uniqueSymbol += std::to_string(i + 1);

        Ship *newShip = nullptr;

        // Decide which ship type to instantiate
        if (info.type == "Battleship") {
          newShip = new Battleship(uniqueSymbol, info.team, &manager);
        } else if (info.type == "Cruiser") {
          newShip = new Cruiser(uniqueSymbol, info.team, &manager);
        } else if (info.type == "Frigate") {
          newShip = new Frigate(uniqueSymbol, info.team, &manager);
        } else if (info.type == "Amphibious") {
          newShip = new Amphibious(uniqueSymbol, info.team, &manager);
        } else if (info.type == "Destroyer") {
          newShip = new Destroyer(uniqueSymbol, info.team, &manager);
        } else if (info.type == "Corvette") {
          newShip = new Corvette(uniqueSymbol, info.team, &manager);
        } else if (info.type == "SuperShip") {
          newShip = new SuperShip(uniqueSymbol, info.team, &manager);
        } else {
          std::cerr << "Unknown ship type: " << info.type << std::endl;
        }

        // Add the ship if it was successfully created
        if (newShip) {
          manager.addShip(newShip);
        }
      } // end for count
    }

    // 4) Run the simulation with config.iterations
    manager.runSimulation(config.iterations);

  } catch (const std::exception &ex) {
    std::cerr << "Error: " << ex.what() << std::endl;
    return 1;
  }

  return 0;
}
