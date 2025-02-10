#include "main.h"

class GameManager {
private:
  Queue<Ship *> respawnQueue;
  const int MAX_RESPAWNS_PER_TURN = 2;
  const int MAX_RESPAWNS_PER_SHIP = 3;
  ofstream logFile;

  void replaceShip(Ship *oldShip, Ship *newShip) {
    for (int i = 0; i < shipCount; i++) {
      if (ships[i] == oldShip) {
        ships[i] = newShip;
        delete oldShip;
        break;
      }
    }
  }

  void processRespawnQueue() {
    int respawnsThisTurn = 0;
    while (!respawnQueue.empty() && respawnsThisTurn < MAX_RESPAWNS_PER_TURN) {
      Ship *ship = respawnQueue.Front();
      if (ship->canRespawn()) {
        ship->incrementRespawns();
        battlefield.placeShip(ship);
        respawnsThisTurn++;
      }
      respawnQueue.pop();
    }
  }

  void logBattleState() {
    logFile << "\n=== Battlefield State ===\n";
    battlefield.display(logFile);
    logShipStatus();
  }

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
  static void replaceShip(Ship *oldShip, Ship *newShip) {
    for (int i = 0; i < shipCount; i++) {
      if (ships[i] == oldShip) {
        ships[i] = newShip;
        delete oldShip;
        break;
      }
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