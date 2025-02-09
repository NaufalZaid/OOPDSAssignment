// filepath: /OOPDS_Assignment/OOPDS_Assignment/src/main.cpp
#include "ship.cpp" // Include the ship-related classes
#include <cstdlib>
#include <ctime>
#include <iostream>
#include <parseFile.cpp>

using namespace std;

// Game Manager Class
class GameManager {
private:
  Battlefield battlefield;
  Ship *ships[MAX_SHIPS_TOTAL]; // Fixed-size array of ships
  int shipCount;
  bool isTeamConflict(Ship *ship1, Ship *ship2) const {
    return ship1->getTeam() == ship2->getTeam();
  }

public:
  GameManager() : shipCount(0) {
    for (int i = 0; i < MAX_SHIPS_TOTAL; i++) {
      ships[i] = nullptr;
    }
  }

  void setBattlefield(const int grid[HEIGHT][WIDTH]) {
    battlefield.setGrid(grid);
  }

  void addShip(Ship *ship) {
    if (shipCount < MAX_SHIPS_TOTAL) {
      ships[shipCount++] = ship;
      battlefield.placeShip(ship);
    }
  }

  void runSimulation(int iterations) {
    for (int i = 0; i < iterations; i++) {
      cout << "\n--- Turn " << i + 1 << " ---\n";
      battlefield.display();

      for (int j = 0; j < shipCount; j++) {
        cout << "\n" << ships[j]->getSymbol() << "'s Turn:\n";

        // Randomly select directions for each action
        Direction randomDir =
            static_cast<Direction>(rand() % 4); // UP, DOWN, LEFT, RIGHT

        ships[j]->look();
        ships[j]->move(randomDir);

        // Shooting can also use diagonal directions
        Direction randomShootDir =
            static_cast<Direction>(rand() % 8); // Includes diagonals
        ships[j]->shoot(randomShootDir);
      }
    }
  }

  void handleCombat(Ship *attacker, Ship *target) {
    if (!isTeamConflict(attacker, target)) {
      target->takeDamage();
      if (!target->isAlive()) {
        cout << attacker->getSymbol() << " from team " << attacker->getTeam()
             << " destroyed " << target->getSymbol() << " from team "
             << target->getTeam() << endl;
      }
    }
  }

  ~GameManager() {
    for (int i = 0; i < shipCount; i++) {
      delete ships[i];
    }
  }
};

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