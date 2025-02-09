#include "ship.cpp"
#include <algorithm>
#include <cstdlib>

class Battleship : public MovingShip, public ShootingShip {
private:
  int shipsDestroyed = 0;
  const int MAX_FIRE_DISTANCE = 5;

public:
  Battleship(char sym = 'B', string team = "Blue")
      : Ship(sym, team), MovingShip(sym, team), ShootingShip(sym, team) {}

  void performTurn() {
    look();

    // Move once
    Direction moveDir = static_cast<Direction>(rand() % 4); // Non-diagonal
    move(moveDir);

    // Shoot twice
    for (int i = 0; i < 2; i++) {
      Direction shootDir = static_cast<Direction>(rand() % 8);
      int targetX, targetY;
      calculateTargetPosition(shootDir, targetX, targetY);

      // Check fire distance
      if (abs(targetX - pos.x) + abs(targetY - pos.y) <= MAX_FIRE_DISTANCE) {
        shoot(shootDir);
      }
    }
  }

  void incrementKills() {
    if (++shipsDestroyed >= 4) {
      // Upgrade to Destroyer
      // Implementation of upgrade mechanism
    }
  }
};

class Cruiser : public MovingShip, public RamShip {
private:
  int shipsDestroyed = 0;

public:
  Cruiser(char sym = 'C', string team = "Red")
      : Ship(sym, team), MovingShip(sym, team), RamShip(sym, team) {}

  void performTurn() {
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

  void incrementKills() {
    if (++shipsDestroyed >= 3) {
      // Upgrade to Destroyer
      // Implementation of upgrade mechanism
    }
  }

private:
  Direction calculateDirection(int dx, int dy) {
    // Convert dx,dy to Direction enum
    // Implementation
    return UP; // placeholder
  }
};

class Frigate : public ShootingShip {
private:
  int currentDirection = 0;
  int shipsDestroyed = 0;
  static const Direction FIRING_SEQUENCE[8];

public:
  Frigate(char sym = 'F', string team = "Green")
      : Ship(sym, team), ShootingShip(sym, team) {}

  void performTurn() {
    shoot(FIRING_SEQUENCE[currentDirection]);
    currentDirection = (currentDirection + 1) % 8;
  }

  void incrementKills() {
    if (++shipsDestroyed >= 3) {
      // Upgrade to Corvette
      // Implementation of upgrade mechanism
    }
  }
};

class Corvette : public ShootingShip {
public:
  Corvette(char sym = 'V', string team = "Yellow")
      : Ship(sym, team), ShootingShip(sym, team) {}

  void performTurn() {
    Direction shootDir = static_cast<Direction>(rand() % 8);
    shoot(shootDir);
  }
};

class Amphibious : public MovingShip, public ShootingShip {
private:
  int shipsDestroyed = 0;

public:
  Amphibious(char sym = 'A', string team = "Purple")
      : Ship(sym, team), MovingShip(sym, team), ShootingShip(sym, team) {}

  void performTurn() {
    // Similar to Battleship but can move anywhere
    look();
    Direction moveDir = static_cast<Direction>(rand() % 4);
    move(moveDir);

    Direction shootDir = static_cast<Direction>(rand() % 8);
    shoot(shootDir);
  }

  void incrementKills() {
    if (++shipsDestroyed >= 4) {
      // Upgrade to SuperShip
      // Implementation of upgrade mechanism
    }
  }
};

class SuperShip : public MovingShip, public ShootingShip, public RamShip {
public:
  SuperShip(char sym = 'S', string team = "Gold")
      : Ship(sym, team), MovingShip(sym, team), ShootingShip(sym, team),
        RamShip(sym, team) {}

  void performTurn() {
    look();

    // Move and destroy like Cruiser
    Direction moveDir = static_cast<Direction>(rand() % 8);
    destroy(moveDir);

    // Shoot at 3 random locations
    for (int i = 0; i < 3; i++) {
      Direction shootDir = static_cast<Direction>(rand() % 8);
      shoot(shootDir);
    }
  }
};

// Define static member
const Direction Frigate::FIRING_SEQUENCE[8] = {
    UP, UP_RIGHT, RIGHT, DOWN_RIGHT, DOWN, DOWN_LEFT, LEFT, UP_LEFT};