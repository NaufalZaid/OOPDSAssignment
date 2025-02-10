#include "ShipTypes.h"
#include "Battlefield.h"
#include <cmath>
#include <cstdlib>
#include <iostream>

// A utility, if needed, to check city-block distance
static bool inRangeCityBlock(int x1, int y1, int x2, int y2, int maxDist) {
  return (std::abs(x1 - x2) + std::abs(y1 - y2)) <= maxDist;
}

/* ==================== BATTLESHIP ==================== */

Battleship::Battleship(const std::string &symbol, const std::string &team,
                       GameManager *mgr)
    : Ship(symbol, team), manager(mgr) {}

void Battleship::look(int offsetX, int offsetY) {
  std::cout << getSymbol() << " is looking at offset(" << offsetX << ","
            << offsetY << ")\n";
}

void Battleship::move() { decideAndMove(); }

void Battleship::shoot(int targetX, int targetY) {
  Battlefield *bf = getBattlefield();
  if (!bf)
    return;

  // bounds check
  if (targetX < 0 || targetX >= HEIGHT || targetY < 0 || targetY >= WIDTH) {
    return;
  }

  // city-block distance <= 5
  Position p = getPosition();
  if (!inRangeCityBlock(p.x, p.y, targetX, targetY, MAX_CITY_BLOCK_DIST)) {
    return;
  }

  Ship *target = bf->getOccupant(targetX, targetY);
  if (!target)
    return;
  if (!target->isAlive())
    return;
  if (target == this)
    return;

  // different team => damage
  if (target->getTeam() != getTeam()) {
    target->takeDamage(1);
    if (!target->isAlive()) {
      incrementKills();
      // if kills >=4 => upgrade
      if (getKillCount() >= 4) {
        requestUpgrade("Destroyer");
        return;
      }
    }
  }
}

void Battleship::performTurn() {
  look(0, 0);
  move();
  shootTwiceRandomPositions();
}

void Battleship::decideAndMove() {
  Battlefield *bf = getBattlefield();
  if (!bf)
    return;

  Position p = getPosition();
  int dir = rand() % 4; // 0=up,1=down,2=left,3=right
  int nx = p.x, ny = p.y;
  switch (dir) {
  case 0:
    nx--;
    break;
  case 1:
    nx++;
    break;
  case 2:
    ny--;
    break;
  case 3:
    ny++;
    break;
  }
  if (nx < 0 || nx >= HEIGHT || ny < 0 || ny >= WIDTH) {
    return;
  }
  if (!bf->getOccupant(nx, ny)) {
    bf->setOccupant(p.x, p.y, nullptr);
    bf->setOccupant(nx, ny, this);
  }
}

void Battleship::shootTwiceRandomPositions() {
  Battlefield *bf = getBattlefield();
  if (!bf)
    return;

  Position p = getPosition();
  for (int i = 0; i < 2; i++) {
    int dx, dy;
    do {
      dx = (rand() % 11) - 5; // -5..5
      dy = (rand() % 11) - 5;
    } while (std::abs(dx) + std::abs(dy) > 5 || (dx == 0 && dy == 0));
    shoot(p.x + dx, p.y + dy);
  }
}

bool Battleship::withinCityBlockRange(int ox, int oy, int tx, int ty) const {
  return (std::abs(ox - tx) + std::abs(oy - ty)) <= MAX_CITY_BLOCK_DIST;
}

/* ==================== CRUISER ==================== */

Cruiser::Cruiser(const std::string &symbol, const std::string &team,
                 GameManager *mgr)
    : Ship(symbol, team), manager(mgr) {}

void Cruiser::look(int offsetX, int offsetY) {
  std::cout << getSymbol() << " (Cruiser) looks at offset(" << offsetX << ","
            << offsetY << ")\n";
}

void Cruiser::move() { moveToPreferredNeighbor(); }

void Cruiser::ram(int targetX, int targetY) {
  Battlefield *bf = getBattlefield();
  if (!bf)
    return;

  // bounds check
  if (targetX < 0 || targetX >= HEIGHT || targetY < 0 || targetY >= WIDTH) {
    return;
  }

  Ship *occupant = bf->getOccupant(targetX, targetY);
  if (!occupant)
    return;
  if (!occupant->isAlive())
    return;
  if (occupant == this)
    return;

  if (occupant->getTeam() != getTeam()) {
    occupant->takeDamage(occupant->getLives());
    incrementKills();
    // move in
    Position p = getPosition();
    bf->setOccupant(p.x, p.y, nullptr);
    bf->setOccupant(targetX, targetY, this);
    // upgrade if kills >=3
    if (getKillCount() >= 3) {
      requestUpgrade("Destroyer");
      return;
    }
  }
}

void Cruiser::performTurn() {
  look(0, 0);
  move();
}

void Cruiser::moveToPreferredNeighbor() {
  Battlefield *bf = getBattlefield();
  if (!bf)
    return;

  Position p = getPosition();
  int bestX = -1, bestY = -1;
  for (int dx = -1; dx <= 1; dx++) {
    for (int dy = -1; dy <= 1; dy++) {
      if (dx == 0 && dy == 0)
        continue;
      int nx = p.x + dx, ny = p.y + dy;
      if (nx < 0 || nx >= HEIGHT || ny < 0 || ny >= WIDTH)
        continue;

      Ship *occ = bf->getOccupant(nx, ny);
      if (occ && occ->isAlive() && occ != this && occ->getTeam() != getTeam()) {
        ram(nx, ny);
        return;
      } else if (!occ && bestX < 0) {
        bestX = nx;
        bestY = ny;
      }
    }
  }
  if (bestX >= 0 && bestY >= 0) {
    bf->setOccupant(p.x, p.y, nullptr);
    bf->setOccupant(bestX, bestY, this);
  }
}

/* ==================== DESTROYER ==================== */

// (A) fresh creation
Destroyer::Destroyer(const std::string &symbol, const std::string &team,
                     GameManager *mgr)
    : Ship(symbol, team), manager(mgr) {
  // no upgrade logic. this is a brand-new ship
  // position = (-1,-1) until placed
  std::cout << "Constructed fresh Destroyer " << symbol << " for team " << team
            << "\n";
}

// (B) upgrade from old ship
Destroyer::Destroyer(const Ship &oldShip, GameManager *mgr)
    : Ship(oldShip.getSymbol(), oldShip.getTeam()), manager(mgr) {
  int lostLives = DEFAULT_LIVES - oldShip.getLives();
  if (lostLives > 0) {
    takeDamage(lostLives);
  }
  for (int i = 0; i < oldShip.getKillCount(); i++) {
    incrementKills();
  }
  setBattlefieldPtr(oldShip.getBattlefield());
  setPosition(oldShip.getPosition().x, oldShip.getPosition().y);

  std::cout << "Upgrading to Destroyer from " << oldShip.getSymbol()
            << " (team " << oldShip.getTeam() << ")\n";
}

void Destroyer::look(int offX, int offY) {
  std::cout << getSymbol() << " (Destroyer) looks at offset(" << offX << ","
            << offY << ")\n";
}

void Destroyer::move() {
  Battlefield *bf = getBattlefield();
  if (!bf)
    return;

  Position p = getPosition();
  int dir = rand() % 4;
  int nx = p.x, ny = p.y;
  switch (dir) {
  case 0:
    nx--;
    break;
  case 1:
    nx++;
    break;
  case 2:
    ny--;
    break;
  case 3:
    ny++;
    break;
  }
  if (nx >= 0 && nx < HEIGHT && ny >= 0 && ny < WIDTH &&
      !bf->getOccupant(nx, ny)) {
    bf->setOccupant(p.x, p.y, nullptr);
    bf->setOccupant(nx, ny, this);
  }
}
void Destroyer::shoot(int targetX, int targetY) {
  Battlefield *bf = getBattlefield();
  if (!bf)
    return;

  // NEW: Check bounds
  if (targetX < 0 || targetX >= HEIGHT || targetY < 0 || targetY >= WIDTH)
    return;

  Position p = getPosition();
  if (std::abs(p.x - targetX) + std::abs(p.y - targetY) > 5)
    return;

  Ship *occ = bf->getOccupant(targetX, targetY);

  if (!occ)
    return;
  if (!occ->isAlive())
    return;
  if (occ == this)
    return;

  if (occ->getTeam() != getTeam()) {
    occ->takeDamage();
    if (!occ->isAlive()) {
      incrementKills();
      if (getKillCount() >= 3) {
        requestUpgrade("SuperShip");
        return;
      }
    }
  }
}

void Destroyer::ram(int targetX, int targetY) {
  Battlefield *bf = getBattlefield();
  if (!bf)
    return;

  // NEW: Check bounds
  if (targetX < 0 || targetX >= HEIGHT || targetY < 0 || targetY >= WIDTH)
    return;

  Ship *occ = bf->getOccupant(targetX, targetY);

  if (!occ)
    return;
  if (!occ->isAlive())
    return;
  if (occ == this)
    return;

  if (occ->getTeam() != getTeam()) {
    occ->takeDamage(occ->getLives());
    incrementKills();
    Position p = getPosition();
    bf->setOccupant(p.x, p.y, nullptr);
    bf->setOccupant(targetX, targetY, this);
    if (getKillCount() >= 3) {
      requestUpgrade("SuperShip");
      return;
    }
  }
}

void Destroyer::performTurn() {
  look(0, 0);
  bool didRam = tryRamNeighbor();
  if (!didRam) {
    move();
  }
  Position p = getPosition();
  for (int i = 0; i < 2; i++) {
    int dx, dy;
    do {
      dx = (rand() % 11) - 5;
      dy = (rand() % 11) - 5;
    } while (std::abs(dx) + std::abs(dy) > 5 || (dx == 0 && dy == 0));
    shoot(p.x + dx, p.y + dy);
  }
}

bool Destroyer::tryRamNeighbor() {
  Battlefield *bf = getBattlefield();
  if (!bf)
    return false;

  Position p = getPosition();
  for (int dx = -1; dx <= 1; dx++) {
    for (int dy = -1; dy <= 1; dy++) {
      if (dx == 0 && dy == 0)
        continue;
      int nx = p.x + dx, ny = p.y + dy;
      if (nx < 0 || nx >= HEIGHT || ny < 0 || ny >= WIDTH)
        continue;
      Ship *occ = bf->getOccupant(nx, ny);
      if (occ && occ->isAlive() && occ != this && occ->getTeam() != getTeam()) {
        ram(nx, ny);
        return true;
      }
    }
  }
  return false;
}

/* ==================== FRIGATE ==================== */

const int Frigate::directions[Frigate::SEQ_LEN][2] = {
    {-1, 0}, {-1, 1}, {0, 1}, {1, 1}, {1, 0}, {1, -1}, {0, -1}, {-1, -1}};

Frigate::Frigate(const std::string &symbol, const std::string &team,
                 GameManager *mgr)
    : ShootingShip(), Ship(symbol, team), manager(mgr), firingIndex(0) {}

void Frigate::shoot(int tx, int ty) {
  Battlefield *bf = getBattlefield();
  if (!bf)
    return;

  // bounds check
  if (tx < 0 || tx >= HEIGHT || ty < 0 || ty >= WIDTH)
    return;

  Position p = getPosition();
  if (std::abs(p.x - tx) <= 1 && std::abs(p.y - ty) <= 1) {
    Ship *occ = bf->getOccupant(tx, ty);
    if (!occ)
      return;
    if (!occ->isAlive())
      return;
    if (occ == this)
      return;

    if (occ->getTeam() != getTeam()) {
      occ->takeDamage();
      if (!occ->isAlive()) {
        incrementKills();
        if (getKillCount() >= 3) {
          requestUpgrade("Corvette");
          return;
        }
      }
    }
  }
}

void Frigate::performTurn() {
  Position p = getPosition();
  int dx = directions[firingIndex][0];
  int dy = directions[firingIndex][1];
  firingIndex = (firingIndex + 1) % SEQ_LEN;
  shoot(p.x + dx, p.y + dy);
}

/* ==================== CORVETTE ==================== */

// (A) fresh creation
Corvette::Corvette(const std::string &symbol, const std::string &team,
                   GameManager *mgr)
    : Ship(symbol, team), manager(mgr) {
  std::cout << "Constructed fresh Corvette " << symbol << " for team " << team
            << "\n";
}

// (B) upgrade
Corvette::Corvette(const Ship &oldShip, GameManager *mgr)
    : Ship(oldShip.getSymbol(), oldShip.getTeam()), manager(mgr) {
  setBattlefieldPtr(oldShip.getBattlefield());
  setPosition(oldShip.getPosition().x, oldShip.getPosition().y);
  int lostLives = DEFAULT_LIVES - oldShip.getLives();
  if (lostLives > 0) {
    takeDamage(lostLives);
  }
  for (int i = 0; i < oldShip.getKillCount(); i++) {
    incrementKills();
  }
  std::cout << "Upgrading to Corvette from " << oldShip.getSymbol() << " (team "
            << oldShip.getTeam() << ")\n";
}

void Corvette::shoot(int tx, int ty) {
  Battlefield *bf = getBattlefield();
  if (!bf)
    return;

  // bounds
  if (tx < 0 || tx >= HEIGHT || ty < 0 || ty >= WIDTH)
    return;

  Position p = getPosition();
  if (std::abs(p.x - tx) <= 1 && std::abs(p.y - ty) <= 1) {
    Ship *occ = bf->getOccupant(tx, ty);
    if (!occ)
      return;
    if (!occ->isAlive())
      return;
    if (occ == this)
      return;

    if (occ->getTeam() != getTeam()) {
      occ->takeDamage();
      if (!occ->isAlive()) {
        incrementKills();
      }
    }
  }
}

void Corvette::performTurn() {
  Position p = getPosition();
  int dx = (rand() % 3) - 1;
  int dy = (rand() % 3) - 1;
  if (dx == 0 && dy == 0) {
    dx = 1;
  }
  shoot(p.x + dx, p.y + dy);
}

/* ==================== AMPHIBIOUS ==================== */

Amphibious::Amphibious(const std::string &symbol, const std::string &team,
                       GameManager *mgr)
    : Ship(symbol, team), manager(mgr) {}

void Amphibious::look(int offsetX, int offsetY) {
  std::cout << getSymbol() << " (Amphibious) looks at offset (" << offsetX
            << "," << offsetY << ")\n";
}

void Amphibious::move() {
  Battlefield *bf = getBattlefield();
  if (!bf)
    return;

  Position p = getPosition();
  int dir = rand() % 4;
  int nx = p.x, ny = p.y;
  switch (dir) {
  case 0:
    nx--;
    break;
  case 1:
    nx++;
    break;
  case 2:
    ny--;
    break;
  case 3:
    ny++;
    break;
  }
  if (nx >= 0 && nx < HEIGHT && ny >= 0 && ny < WIDTH) {
    Ship *occ = bf->getOccupant(nx, ny);
    if (!occ) {
      bf->setOccupant(p.x, p.y, nullptr);
      bf->setOccupant(nx, ny, this);
    }
  }
}

void Amphibious::shoot(int targetX, int targetY) {
  Battlefield *bf = getBattlefield();
  if (!bf)
    return;

  // NEW: Check bounds
  if (targetX < 0 || targetX >= HEIGHT || targetY < 0 || targetY >= WIDTH)
    return;

  Position p = getPosition();
  if ((std::abs(p.x - targetX) + std::abs(p.y - targetY)) <= 5) {
    Ship *target = bf->getOccupant(targetX, targetY);
    if (!target)
      return;
    if (!target->isAlive())
      return;
    if (target == this)
      return;

    if (target->getTeam() != getTeam()) {
      target->takeDamage();
      if (!target->isAlive()) {
        incrementKills();
        if (getKillCount() >= 4) {
          requestUpgrade("SuperShip");
          return;
        }
      }
    }
  }
}

void Amphibious::performTurn() {
  look(0, 0);
  move();
  Position p = getPosition();
  for (int i = 0; i < 2; i++) {
    int dx, dy;
    do {
      dx = (rand() % 11) - 5;
      dy = (rand() % 11) - 5;
    } while (std::abs(dx) + std::abs(dy) > 5 || (dx == 0 && dy == 0));
    shoot(p.x + dx, p.y + dy);
  }
}

/* ==================== SUPERSHIP ==================== */

// (A) fresh creation
SuperShip::SuperShip(const std::string &symbol, const std::string &team,
                     GameManager *mgr)
    : Ship(symbol, team), manager(mgr) {
  std::cout << "Constructed fresh SuperShip " << symbol << " for team " << team
            << "\n";
}

// (B) upgrade
SuperShip::SuperShip(const Ship &oldShip, GameManager *mgr)
    : Ship(oldShip.getSymbol(), oldShip.getTeam()), manager(mgr) {
  setBattlefieldPtr(oldShip.getBattlefield());
  setPosition(oldShip.getPosition().x, oldShip.getPosition().y);
  int lostLives = DEFAULT_LIVES - oldShip.getLives();
  if (lostLives > 0) {
    takeDamage(lostLives);
  }
  for (int i = 0; i < oldShip.getKillCount(); i++) {
    incrementKills();
  }
  std::cout << "Upgrading to SuperShip from " << oldShip.getSymbol()
            << ", team=" << oldShip.getTeam() << "\n";
}

void SuperShip::look(int offsetX, int offsetY) {
  std::cout << getSymbol() << " (SuperShip) scanning offset (" << offsetX << ","
            << offsetY << ")\n";
}

void SuperShip::move() { moveLikeCruiser(); }

void SuperShip::shoot(int targetX, int targetY) {
  Battlefield *bf = getBattlefield();
  if (!bf)
    return;

  // NEW: Check bounds
  if (targetX < 0 || targetX >= HEIGHT || targetY < 0 || targetY >= WIDTH)
    return;

  Ship *occ = bf->getOccupant(targetX, targetY);
  if (!occ)
    return;
  if (!occ->isAlive())
    return;
  if (occ == this)
    return;

  if (occ->getTeam() != getTeam()) {
    occ->takeDamage();
    if (!occ->isAlive()) {
      incrementKills();
    }
  }
}

void SuperShip::ram(int targetX, int targetY) {
  Battlefield *bf = getBattlefield();
  if (!bf)
    return;

  // NEW: Check bounds
  if (targetX < 0 || targetX >= HEIGHT || targetY < 0 || targetY >= WIDTH)
    return;

  Ship *occ = bf->getOccupant(targetX, targetY);
  if (!occ)
    return;
  if (!occ->isAlive())
    return;
  if (occ == this)
    return;

  if (occ->getTeam() != getTeam()) {
    occ->takeDamage(occ->getLives());
    incrementKills();
    Position p = getPosition();
    bf->setOccupant(p.x, p.y, nullptr);
    bf->setOccupant(targetX, targetY, this);
  }
}

void SuperShip::performTurn() {
  look(0, 0);
  move();
  shoot3RandomLocations();
}

void SuperShip::moveLikeCruiser() {
  Battlefield *bf = getBattlefield();
  if (!bf)
    return;
  Position p = getPosition();

  for (int dx = -1; dx <= 1; dx++) {
    for (int dy = -1; dy <= 1; dy++) {
      if (dx == 0 && dy == 0)
        continue;
      int nx = p.x + dx, ny = p.y + dy;
      if (nx < 0 || nx >= HEIGHT || ny < 0 || ny >= WIDTH)
        continue;
      Ship *occ = bf->getOccupant(nx, ny);
      if (occ && occ->isAlive() && occ != this && occ->getTeam() != getTeam()) {
        ram(nx, ny);
        return;
      }
    }
  }
  int dir = rand() % 8;
  int nx = p.x, ny = p.y;
  switch (dir) {
  case 0:
    nx--;
    break;
  case 1:
    nx++;
    break;
  case 2:
    ny--;
    break;
  case 3:
    ny++;
    break;
  case 4:
    nx--;
    ny--;
    break;
  case 5:
    nx--;
    ny++;
    break;
  case 6:
    nx++;
    ny--;
    break;
  case 7:
    nx++;
    ny++;
    break;
  }
  if (nx >= 0 && nx < HEIGHT && ny >= 0 && ny < WIDTH &&
      !bf->getOccupant(nx, ny)) {
    bf->setOccupant(p.x, p.y, nullptr);
    bf->setOccupant(nx, ny, this);
  }
}

void SuperShip::shoot3RandomLocations() {
  Battlefield *bf = getBattlefield();
  if (!bf)
    return;
  for (int i = 0; i < 3; i++) {
    int rx = rand() % HEIGHT;
    int ry = rand() % WIDTH;
    shoot(rx, ry);
  }
}
