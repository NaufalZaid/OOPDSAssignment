#ifndef GAMEMANAGER_H
#define GAMEMANAGER_H

#include "Battlefield.h"
#include "Ship.h"
#include <string>
#include <vector>

class GameManager {
private:
  Battlefield battlefield;   // The shared battlefield
  std::vector<Ship *> ships; // All ships in the game

  // Queue (or vector) for storing dead ships that can respawn
  std::vector<Ship *> respawnQueue;

  int maxRespawnsPerTurn; // e.g., 2
  int maxShipRespawns;    // e.g., 3
  int totalIterations;    // e.g., from config

public:
  // Constructor / Destructor
  GameManager();
  ~GameManager();

  // Initialization
  void setBattlefieldTerrain(const int grid[HEIGHT][WIDTH]);
  void addShip(Ship *newShip);

  // Main simulation
  void runSimulation(int iterations);
  void executeTurn(int turnNumber);

  // Respawns
  void enqueueRespawn(Ship *deadShip);
  void processRespawns();

  // Upgrading
  void upgradeShip(Ship *oldShip, const std::string &newType);

  // Win-check
  bool checkVictory() const;

  // Access to battlefield if needed
  Battlefield &getBattlefield() { return battlefield; }
  const Battlefield &getBattlefield() const { return battlefield; }
};

#endif // GAMEMANAGER_H
