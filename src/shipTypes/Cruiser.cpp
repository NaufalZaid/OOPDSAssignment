#include "ship.cpp"
#include <algorithm>
#include <cstdlib>

class Cruiser : public MovingShip,
                public RamShip,
                public SeeingRobot,
                public ShootingShip {
private:
  int shipsDestroyed = 0;

public:
  Cruiser(char sym = 'C', string team = "Red")
      : Ship(sym, team), MovingShip(sym, team), RamShip(sym, team),
        SeeingRobot(sym, team), ShootingShip(sym, team) {}

  void checkForUpgrade() override {
    if (killCount >= 3 && !isUpgraded) {
      isUpgraded = true;
      // Signal for upgrade to Destroyer
    }
  }

  void performTurn() override {
    look();

    // Check 3x3 neighborhood
    for (int dx = -1; dx <= 1; dx++) {
      for (int dy = -1; dy <= 1; dy++) {
        if (dx == 0 && dy == 0)
          continue;

        int targetX = pos.x + dx;
        int targetY = pos.y + dy;

        if (isWithinBoundary(targetX, targetY) &&
            !isUnoccupied(targetX, targetY)) {
          // Found enemy ship, move to destroy it
          Direction dir = calculateDirection(dx, dy);
          destroy(dir);
          return;
        }
      }
    }

    // If no enemies found, move randomly
    Direction moveDir = static_cast<Direction>(rand() % 8);
    move(moveDir);
  }

  void incrementKills() override {
    ++shipsDestroyed;
    checkForUpgrade();
  }

private:
  Direction calculateDirection(int dx, int dy) {
    // Convert dx,dy to Direction enum
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
};
