#include "../main.h"
#include <algorithm>
#include <cstdlib>

class Battleship : public MovingShip,
                   public ShootingShip,
                   public SeeingRobot,
                   public RamShip {
private:
  int shipsDestroyed = 0;
  const int MAX_FIRE_DISTANCE = 5;

public:
  Battleship(char sym = 'B', const string &teamName = "Blue")
      : Ship(sym), MovingShip(sym, teamName), ShootingShip(sym, teamName),
        SeeingRobot(sym, teamName), RamShip(sym, teamName) {
    setTeam(teamName);
  }

  bool canMoveToOccupiedSpace = false;

  void performTurn() override {
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

    // Check for upgrade to Destroyer
    if (shipsDestroyed >= 4) {
      // Transform to Destroyer
      transformToDestroyer();
    }
  }

  void incrementKills() override { ++shipsDestroyed; }

  int getKills() const { return shipsDestroyed; }

  void transformToDestroyer() {
    // Create a new Destroyer object with the same properties
    Destroyer *destroyer = new Destroyer(*this);
    // Replace this Battleship with the new Destroyer in the game manager
    GameManager::replaceShip(this, destroyer);
  }
};