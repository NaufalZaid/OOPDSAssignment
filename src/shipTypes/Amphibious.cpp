#include "../main.h"
#include <algorithm>
#include <cstdlib>
#include <cstring>

class Amphibious : public MovingShip,
                   public ShootingShip,
                   public RamShip,
                   public SeeingRobot {
private:
  int shipsDestroyed = 0;

public:
  Amphibious(char sym = 'A', string team = "Purple")
      : Ship(sym, team), MovingShip(sym, team), ShootingShip(sym, team),
        RamShip(sym, team), SeeingRobot(sym, team) {}

  void move(Direction dir) override {
    // Can move regardless of terrain
    int targetX = pos.x;
    int targetY = pos.y;
    calculateTargetPosition(dir, targetX, targetY);

    if (isWithinBoundary(targetX, targetY)) {
      battlefield[pos.x][pos.y] = EMPTY_CELL;
      pos.x = targetX;
      pos.y = targetY;
      battlefield[pos.x][pos.y] = symbol;
    }
  }

  void performTurn() override {
    // Similar to Battleship but can move anywhere
    look();
    Direction moveDir = static_cast<Direction>(rand() % 4);
    move(moveDir);

    Direction shootDir = static_cast<Direction>(rand() % 8);
    shoot(shootDir);
  }

  void incrementKills() override {
    if (++shipsDestroyed >= 4) {
      // Upgrade to SuperShip
      // Implementation of upgrade mechanism
    }
  }
};
