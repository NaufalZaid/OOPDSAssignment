// Ship.cpp
#include "Ship.h"
#include "Battlefield.h" // Full declaration needed for getOccupant/setOccupant
#include <iostream>

Ship::Ship(const std::string &sym, const std::string &teamName)
    : pos(-1, -1), lives(DEFAULT_LIVES), killCount(0), respawnCount(0),
      symbol(sym), team(teamName), battlefieldPtr(nullptr), pendingUpgrade("") {
  std::cout << "Constructing Ship " << symbol << " (Team " << team << ") at "
            << getPosition().x << "," << getPosition().y << " [" << this
            << "]\n";
}

void Ship::takeDamage(int dmg) {
  std::cout << "Ship " << symbol << " (Team " << team << ") takes " << dmg
            << " damage. Lives before: " << lives << "\n";
  lives -= dmg;
  if (lives <= 0) {
    lives = 0;
    std::cout << symbol << " (Team " << team << ") has been destroyed.\n";

    // Immediately clear this ship from the battlefield occupant array
    if (battlefieldPtr) {
      Position p = getPosition();
      if (p.x >= 0 && p.x < HEIGHT && p.y >= 0 && p.y < WIDTH) {
        Ship *occ = battlefieldPtr->getOccupant(p.x, p.y);
        if (occ == this) {
          std::cout << "Clearing occupant at (" << p.x << "," << p.y << ")\n";
          battlefieldPtr->setOccupant(p.x, p.y, nullptr);
        }
      }
    }
  }
  std::cout << "Ship " << symbol << " now has " << lives << " lives.\n";
}

bool Ship::canRespawn(int maxAllowed) const {
  return respawnCount < maxAllowed;
}
