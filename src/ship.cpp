#include "Ship.h"
#include <iostream>

Ship::Ship(const std::string &sym, const std::string &teamName)
    : pos(-1, -1), lives(DEFAULT_LIVES), killCount(0), respawnCount(0),
      symbol(sym), team(teamName), battlefieldPtr(nullptr) {}

void Ship::takeDamage(int dmg) {
  lives -= dmg;
  if (lives <= 0) {
    lives = 0;
    std::cout << symbol << " (Team " << team << ") has been destroyed.\n";
  }
}

bool Ship::canRespawn(int maxAllowed) const {
  return respawnCount < maxAllowed;
}
