#include "Queue.cpp"
#include "parseFile.cpp"
#include "ship.cpp" // Include the ship-related classes
#include <cstdlib>
#include <ctime>
#include <iostream>

using namespace std;

// Game Manager Class
class GameManager {
private:
  Queue<Ship *> respawnQueue;
  const int MAX_RESPAWNS_PER_TURN = 2;
  const int MAX_RESPAWNS_PER_SHIP = 3;

  void handleRespawns() {
    int respawnsThisTurn = 0;
    while (!respawnQueue.empty() && respawnsThisTurn < MAX_RESPAWNS_PER_TURN) {
      Ship *ship = respawnQueue.Front();
      respawnQueue.pop();
      battlefield.placeShip(ship);
      respawnsThisTurn++;
    }
  }

  bool checkTeamVictory() {
    string survivingTeam = "";
    bool hasWinner = true;

    for (int i = 0; i < shipCount; i++) {
      if (ships[i]->isAlive()) {
        if (survivingTeam.empty()) {
          survivingTeam = ships[i]->getTeam();
        } else if (survivingTeam != ships[i]->getTeam()) {
          hasWinner = false;
          break;
        }
      }
    }

    if (hasWinner && !survivingTeam.empty()) {
      cout << "Team " << survivingTeam << " wins!" << endl;
      return true;
    }
    return false;
  }
  ofstream logFile;

  void initializeLog() { logFile.open("battle_log.txt"); }

  void logTurn(int turnNumber) {
    logFile << "\n--- Turn " << turnNumber << " ---\n";
    // Log battlefield state
    for (int i = 0; i < HEIGHT; i++) {
      for (int j = 0; j < WIDTH; j++) {
        logFile << battlefield.getCell(i, j) << " ";
      }
      logFile << endl;
    }
  }
  void handleShipUpgrade(Ship *ship) {
    Ship *upgradedShip = nullptr;

    if (Battleship *b = dynamic_cast<Battleship *>(ship)) {
      if (b->getKills() >= 4) {
        upgradedShip = new Destroyer(ship->getSymbol(), ship->getTeam());
      }
    }
    // Similar for other ship types

    if (upgradedShip) {
      // Replace old ship with upgraded version
      for (int i = 0; i < shipCount; i++) {
        if (ships[i] == ship) {
          delete ships[i];
          ships[i] = upgradedShip;
          break;
        }
      }
    }
  }
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
    initializeLog();
    for (int i = 0; i < iterations && !checkTeamVictory(); i++) {
      cout << "\n--- Turn " << i + 1 << " ---\n";
      battlefield.display();
      logTurn(i + 1);

      handleRespawns();

      for (int j = 0; j < shipCount; j++) {
        if (ships[j]->isAlive()) {
          ships[j]->performTurn();
          handleShipUpgrade(ships[j]);
        }
      }
    }
    logFile.close();
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