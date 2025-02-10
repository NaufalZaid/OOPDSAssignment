#include "../main.h"
#include <algorithm>
#include <cstdlib>

class Corvette : public ShootingShip,
                 public RamShip,
                 public SeeingRobot,
                 public MovingShip {
public:
  Corvette(char sym = 'V', string team = "Yellow")
      : Ship(sym, team), ShootingShip(sym, team), SeeingRobot(sym, team),
        RamShip(sym, team), MovingShip(sym, team) {}

  void performTurn() override {
    Direction shootDir = static_cast<Direction>(rand() % 8);
    shoot(shootDir);
  }
};
