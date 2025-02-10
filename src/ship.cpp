// Ship.cpp
#include "Ship.h"
#include "Battlefield.h" // So we can call getOccupant/setOccupant
#include <iostream>

Ship::Ship(const std::string &sym, const std::string &teamName)
    : pos(-1, -1), lives(DEFAULT_LIVES), killCount(0), respawnCount(0),
      symbol(sym), team(teamName), battlefieldPtr(nullptr), pendingUpgrade("") {
}

void Ship::takeDamage(int dmg) {
  lives -= dmg;
  if (lives <= 0) {
    lives = 0;
    std::cout << symbol << " (Team " << team << ") has been destroyed.\n";

    // Immediately remove from the battlefield occupant array
    // so that no other ship references this pointer mid-turn.
    if (battlefieldPtr) {
      Position p = getPosition();
      if (p.x >= 0 && p.x < HEIGHT && p.y >= 0 && p.y < WIDTH) {
        // Only clear occupant if it's actually this ship in that cell
        Ship *occupant = battlefieldPtr->getOccupant(p.x, p.y);
        if (occupant == this) {
          battlefieldPtr->setOccupant(p.x, p.y, nullptr);
        }
      }
    }
  }
}

bool Ship::canRespawn(int maxAllowed) const {
  return respawnCount < maxAllowed;
}
