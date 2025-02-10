#ifndef SHIP_H
#define SHIP_H

#include "Constants.h"
#include <string>

// Forward declaration to avoid circular includes
class Battlefield;

/**
 * Simple struct to hold X,Y coordinates in the battlefield.
 * - x corresponds to row (0..HEIGHT-1)
 * - y corresponds to column (0..WIDTH-1)
 */
struct Position {
  int x;
  int y;
  Position(int px = -1, int py = -1) : x(px), y(py) {}
};

/**
 * Base abstract class for all Ships.
 * - Stores position, lives, symbol (string), and team name.
 * - Provides common interface for movement, shooting, etc.
 * - In this simplified example, the actual logic may be in derived classes.
 */
class Ship {
protected:
  Position pos;     // Current position on the battlefield
  int lives;        // Hit-points/lives
  int killCount;    // Number of enemy ships destroyed
  int respawnCount; // How many times this ship has respawned

  std::string symbol; // E.g., "*1", ">2", etc. (can be multiple chars)
  std::string team;   // Team name (e.g., "A", "B")

  // Pointer to the shared Battlefield (not owned by Ship)
  Battlefield *battlefieldPtr;

public:
  Ship(const std::string &sym, const std::string &teamName);
  virtual ~Ship() {}

  // Core abstract actions
  virtual void performTurn() = 0; // Each derived type implements its own logic

  // Common ship methods
  bool isAlive() const { return lives > 0; }
  int getLives() const { return lives; }
  int getKillCount() const { return killCount; }
  int getRespawnCount() const { return respawnCount; }

  void takeDamage(int dmg = 1); // Reduces lives
  void incrementKills() { killCount++; }
  bool canRespawn(int maxAllowed = 3) const;
  void incrementRespawnCount() { respawnCount++; }

  // Position / Team / Symbol
  void setPosition(int x, int y) {
    pos.x = x;
    pos.y = y;
  }
  Position getPosition() const { return pos; }
  std::string getSymbol() const { return symbol; }
  std::string getTeam() const { return team; }

  void setBattlefieldPtr(Battlefield *bf) { battlefieldPtr = bf; }
  Battlefield *getBattlefield() const { return battlefieldPtr; }

  // Helpers for movement or shooting checks
  bool isWithinBoundary(int x, int y) const {
    return (x >= 0 && x < HEIGHT && y >= 0 && y < WIDTH);
  }
};

#endif // SHIP_H
