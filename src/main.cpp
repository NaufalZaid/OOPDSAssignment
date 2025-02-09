#include "GameManager.cpp"
#include <cstdlib>
#include <ctime>
#include <fstream>
#include <iostream>

using namespace std;

// Main Function
int main(int argc, char *argv[]) {
  if (argc != 2) {
    cout << "Usage: " << argv[0] << " <game_file.txt>" << endl;
    return 1;
  }

  try {
    GameParser parser;
    GameConfig config = parser.parseFile(argv[1]);

    GameManager game;
    game.setBattlefield(config.battlefield);

    // Create and add ships for each team
    for (int t = 0; t < config.teamCount; t++) {
      TeamConfig &team = config.teams[t];
      for (int s = 0; s < team.shipTypeCount; s++) {
        ShipConfig &shipConfig = team.ships[s];
        for (int i = 0; i < shipConfig.count; i++) {
          char uniqueSymbol = shipConfig.symbol;
          if (shipConfig.count > 1) {
            // Append number to symbol for multiple ships
            uniqueSymbol = shipConfig.symbol + to_string(i + 1)[0];
          }
          Ship *ship =
              parser.createShip(shipConfig.type, uniqueSymbol, team.name);
          game.addShip(ship);
        }
      }
    }

    game.runSimulation(config.iterations);
  } catch (const exception &e) {
    cerr << "Error: " << e.what() << endl;
    return 1;
  }

  return 0;
}