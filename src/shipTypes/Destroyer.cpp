#include "../main.h"
#include <algorithm>
#include <cstdlib>

class Destroyer : public MovingShip,
                  public RamShip,
                  public SeeingRobot,
                  public ShootingShip {
private:
  int shipsDestroyed = 0;
  const int MAX_FIRE_DISTANCE = 5;

  Direction calculateDirection(int dx, int dy) {
    if (dx < 0) {
      if (dy < 0)
        return UP_LEFT;
      if (dy == 0)
        return UP;
      return UP_RIGHT;
    }
    if (dx == 0) {
      if (dy < 0)
        return LEFT;
      return RIGHT;
    }
    if (dy < 0)
      return DOWN_LEFT;
    if (dy == 0)
      return DOWN;
    return DOWN_RIGHT;
  }

public:
  // Constructor for fresh Destroyer
  Destroyer(char sym = 'D', const string &teamName = "Any")
      : Ship(sym), MovingShip(sym, teamName), ShootingShip(sym, teamName),
        RamShip(sym, teamName), SeeingRobot(sym, teamName) {
    setTeam(teamName);
  }

  // Constructor for upgrading from other ships
  Destroyer(const Ship &original)
      : Ship(original.getSymbol()),
        MovingShip(original.getSymbol(), original.getTeam()),
        ShootingShip(original.getSymbol(), original.getTeam()),
        RamShip(original.getSymbol(), original.getTeam()),
        SeeingRobot(original.getSymbol(), original.getTeam()) {
    setTeam(original.getTeam());
    pos = original.getPosition();
    lives = original.getLives();
  }

  void performTurn() override {
    look(); // From SeeingRobot

    // First try to destroy like Cruiser
    bool enemyFound = false;
    for (int dx = -1; dx <= 1 && !enemyFound; dx++) {
      for (int dy = -1; dy <= 1; dy++) {
        if (dx == 0 && dy == 0)
          continue;

        int targetX = pos.x + dx;
        int targetY = pos.y + dy;

        if (isWithinBoundary(targetX, targetY) &&
            !isUnoccupied(targetX, targetY)) {
          Direction dir = calculateDirection(dx, dy);
          destroy(dir);
          enemyFound = true;
          break;
        }
      }
    }

    // If no enemies to destroy, act like Battleship
    if (!enemyFound) {
      // Move once (non-diagonal like Battleship)
      Direction moveDir = static_cast<Direction>(rand() % 4);
      move(moveDir);

      // Shoot twice with distance check
      for (int i = 0; i < 2; i++) {
        Direction shootDir = static_cast<Direction>(rand() % 8);
        int targetX, targetY;
        calculateTargetPosition(shootDir, targetX, targetY);

        if (abs(targetX - pos.x) + abs(targetY - pos.y) <= MAX_FIRE_DISTANCE) {
          shoot(shootDir);
        }
      }
    }

    // Check for upgrade to SuperShip
    if (shipsDestroyed >= 3) {
      // Transform to SuperShip
      transformToSuperShip();
    }
  }

  void incrementKills() override { ++shipsDestroyed; }

  void transformToSuperShip() {
    // Create a new SuperShip object with the same properties
    SuperShip *superShip = new SuperShip(*this);
    // Replace this Destroyer with the new SuperShip in the game manager
    GameManager::replaceShip(this, superShip);
  }
};