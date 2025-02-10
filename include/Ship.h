#ifndef SHIP_H
#define SHIP_H

#include "Constants.h"
#include <iostream>
#include <string>

class Battlefield;

// Position struct
struct Position {
  int x, y;
  Position(int px = -1, int py = -1) : x(px), y(py) {}
};

/**
 * Base abstract Ship class
 */
class Ship {
protected:
  Position pos;
  int lives;
  int killCount;
  int respawnCount;

  std::string symbol;
  std::string team;

  Battlefield *battlefieldPtr;

  // NEW: store a pending upgrade request (empty if none)
  std::string pendingUpgrade;

public:
  Ship(const std::string &sym, const std::string &teamName);
  ~Ship() {
    std::cout << "Deleting ship " << symbol << " (Team " << team << ") at "
              << getPosition().x << "," << getPosition().y
              << " pointer: " << this << "\n";
  }

  virtual void performTurn() = 0; // each final derived must implement

  // Common ship methods
  bool isAlive() const { return lives > 0; }
  int getLives() const { return lives; }
  int getKillCount() const { return killCount; }
  int getRespawnCount() const { return respawnCount; }

  void takeDamage(int dmg = 1);
  void incrementKills() { killCount++; }
  bool canRespawn(int maxAllowed = 3) const;
  void incrementRespawnCount() { respawnCount++; }

  // Position, team, symbol
  void setPosition(int x, int y) {
    pos.x = x;
    pos.y = y;
  }
  Position getPosition() const { return pos; }
  std::string getSymbol() const { return symbol; }
  std::string getTeam() const { return team; }

  void setBattlefieldPtr(Battlefield *bf) { battlefieldPtr = bf; }
  Battlefield *getBattlefield() const { return battlefieldPtr; }

  // NEW: for deferred upgrade logic
  void requestUpgrade(const std::string &newType) { pendingUpgrade = newType; }
  std::string getPendingUpgradeType() const { return pendingUpgrade; }
  void clearPendingUpgrade() { pendingUpgrade.clear(); }
};

#endif
