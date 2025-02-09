#include "ship.cpp"
#include <algorithm>
#include <cstdlib>

class Frigate : public ShootingShip,
                public RamShip,
                public SeeingRobot,
                public MovingShip {
private:
  const Direction Frigate::FIRING_SEQUENCE[8] = {
      UP, UP_RIGHT, RIGHT, DOWN_RIGHT, DOWN, DOWN_LEFT, LEFT, UP_LEFT};
  int currentFiringIndex = 0;
  int shipsDestroyed = 0;

public:
  Frigate(char sym = 'F', string team = "Green")
      : Ship(sym, team), ShootingShip(sym, team), SeeingRobot(sym, team),
        RamShip(sym, team), MovingShip(sym, team) {}

  void performTurn() override {
    shoot(FIRING_SEQUENCE[currentFiringIndex]);
    currentFiringIndex = (currentFiringIndex + 1) % 8;
  }

  void incrementKills() override {
    if (++shipsDestroyed >= 3) {
      // Upgrade to Corvette
      // Implementation of upgrade mechanism
    }
  }
};
