#include "ship.cpp"
#include <algorithm>
#include <cstdlib>

class SuperShip : public MovingShip,
                  public ShootingShip,
                  public RamShip,
                  public SeeingRobot {
public:
  SuperShip(char sym = 'S', string team = "Gold")
      : Ship(sym, team), MovingShip(sym, team), ShootingShip(sym, team),
        RamShip(sym, team), SeeingRobot(sym, team) {}

  // Constructor for upgrading from other ships
  SuperShip(const Ship &original)
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