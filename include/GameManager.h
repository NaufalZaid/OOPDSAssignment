#ifndef GAMEMANAGER_H
#define GAMEMANAGER_H

#include "Battlefield.h"
#include "Ship.h"
#include <string>
#include <vector>

class GameManager {
private:
  Battlefield battlefield;
  std::vector<Ship *> ships;

  std::vector<Ship *> respawnQueue;
  int maxRespawnsPerTurn;
  int maxShipRespawns;
  int totalIterations;

  // NEW: We'll reuse the same upgradeShip(...) function,
  // but we won't call it from ships directly. We'll call it
  // from handleUpgrades() after the turn ends.

public:
  GameManager();
  ~GameManager();

  void setBattlefieldTerrain(const int grid[HEIGHT][WIDTH]);
  void addShip(Ship *newShip);

  void runSimulation(int iterations);
  void executeTurn(int turnNumber);

  // Respawns
  void enqueueRespawn(Ship *deadShip);
  void processRespawns();

  // Upgrades
  void upgradeShip(Ship *oldShip, const std::string &newType);

  // NEW: after each turn, we'll check all ships for pending upgrades
  void handleUpgrades();

  bool checkVictory() const;

  Battlefield &getBattlefield() { return battlefield; }
  const Battlefield &getBattlefield() const { return battlefield; }
};

#endif
