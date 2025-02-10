#include "ShipTypes.h"
#include "Battlefield.h"
#include <cmath> // abs()
#include <cstdlib>
#include <iostream>

// ---------- Utilities ----------
static bool inRangeCityBlock(int x1, int y1, int x2, int y2, int maxDist) {
  return (std::abs(x1 - x2) + std::abs(y1 - y2)) <= maxDist;
}

// =============== BATTLESHIP ===============
Battleship::Battleship(const std::string &symbol, const std::string &team,
                       GameManager *mgr)
    : Ship(symbol, team), manager(mgr) {}

void Battleship::look(int offsetX, int offsetY) {
  // Basic example: reveal a 3x3 or 9-square area around (pos.x + offsetX, pos.y
  // + offsetY). The assignment says "look(x,y)" reveals a 3x3 window, but you
  // can adapt if needed. For brevity, we just show we 'look' - real code might
  // store the info in some internal state.
  std::cout << getSymbol() << " is looking at offset (" << offsetX << ","
            << offsetY << ")\n";
}

void Battleship::move() {
  // The assignment: "Battleship cannot move to a location that contains another
  // ship" So we pick a random direction (up/down/left/right).
  decideAndMove();
}

void Battleship::shoot(int targetX, int targetY) {
  Battlefield *bf = getBattlefield();
  if (!bf)
    return;

  // City block distance <= 5
  Position p = getPosition();
  if (!inRangeCityBlock(p.x, p.y, targetX, targetY, MAX_CITY_BLOCK_DIST)) {
    return;
  }
  // Must also ensure we do not shoot ourselves or a friend
  Ship *target = bf->getOccupant(targetX, targetY);
  if (target && target != this && target->getTeam() != getTeam()) {
    target->takeDamage(1);
    if (!target->isAlive()) {
      incrementKills();
      // If we have >=4 kills => upgrade
      if (getKillCount() >= 4) {
        manager->upgradeShip(this, "Destroyer");
      }
    }
  }
}

void Battleship::performTurn() {
  // 1) look at current pos or any offset you want
  look(0, 0);

  // 2) move once
  move();

  // 3) shoot 2 times at random positions within city block dist=5
  shootTwiceRandomPositions();
}

void Battleship::decideAndMove() {
  Battlefield *bf = getBattlefield();
  if (!bf)
    return;

  // pick one of 4 directions
  int dir = rand() % 4; // 0=up,1=down,2=left,3=right
  Position p = getPosition();
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
  // check boundary
  if (nx < 0 || nx >= HEIGHT || ny < 0 || ny >= WIDTH)
    return;

  // cannot move if occupant is not null
  if (bf->getOccupant(nx, ny) == nullptr) {
    // free, do the move
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
    // random offset dx, dy with city-block <= 5
    // simpler approach: generate a random dx, dy in [-5..5], retry if out of
    // range
    int dx, dy;
    do {
      dx = (rand() % 11) - 5; // -5..+5
      dy = (rand() % 11) - 5;
    } while (std::abs(dx) + std::abs(dy) > 5 || (dx == 0 && dy == 0));

    shoot(p.x + dx, p.y + dy);
  }
}

bool Battleship::withinCityBlockRange(int ox, int oy, int tx, int ty) const {
  return (std::abs(ox - tx) + std::abs(oy - ty)) <= MAX_CITY_BLOCK_DIST;
}

// =============== CRUISER ===============
Cruiser::Cruiser(const std::string &symbol, const std::string &team,
                 GameManager *mgr)
    : Ship(symbol, team), manager(mgr) {}

void Cruiser::look(int offsetX, int offsetY) {
  std::cout << getSymbol() << " (Cruiser) looks at offset (" << offsetX << ","
            << offsetY << ")\n";
}

void Cruiser::move() {
  // Moves to one of the 8 neighbors (3x3 minus self).
  // If that neighbor has an enemy => we "ram()" (destroy).
  moveToPreferredNeighbor();
}

void Cruiser::ram(int targetX, int targetY) {
  Battlefield *bf = getBattlefield();
  if (!bf)
    return;

  Ship *occupant = bf->getOccupant(targetX, targetY);
  if (occupant && occupant->getTeam() != getTeam()) {
    // destroy occupant
    occupant->takeDamage(occupant->getLives());
    incrementKills();
    // move into that cell
    Position p = getPosition();
    bf->setOccupant(p.x, p.y, nullptr);
    bf->setOccupant(targetX, targetY, this);

    // check upgrade
    if (getKillCount() >= 3) {
      manager->upgradeShip(this, "Destroyer");
    }
  }
}

void Cruiser::performTurn() {
  // 1) look around
  look(0, 0);
  // 2) move (which may destroy occupant)
  move();
}

void Cruiser::moveToPreferredNeighbor() {
  Battlefield *bf = getBattlefield();
  Position p = getPosition();

  // Check 8 neighbors, prefer occupant if occupant is enemy
  // If found, call ram(...)
  // If none found, pick a random neighbor to move if free
  // This is a simplified approach

  // 1) gather neighbors
  int bestX = -1, bestY = -1;
  bool foundEnemy = false;

  for (int dx = -1; dx <= 1; dx++) {
    for (int dy = -1; dy <= 1; dy++) {
      if (dx == 0 && dy == 0)
        continue;
      int nx = p.x + dx, ny = p.y + dy;
      if (nx < 0 || nx >= HEIGHT || ny < 0 || ny >= WIDTH)
        continue;

      Ship *occ = bf->getOccupant(nx, ny);
      if (occ && occ->getTeam() != getTeam()) {
        // enemy occupant => ram
        ram(nx, ny);
        return; // done for the turn
      } else if (!occ && !foundEnemy) {
        // track a free neighbor if we haven't found enemy occupant
        // we can choose the first free neighbor or random among them
        bestX = nx;
        bestY = ny;
      }
    }
  }

  // if we get here, no enemy occupant found; move to bestX,bestY if valid
  if (bestX >= 0 && bestY >= 0) {
    bf->setOccupant(p.x, p.y, nullptr);
    bf->setOccupant(bestX, bestY, this);
  }
}

// =============== DESTROYER ===============
Destroyer::Destroyer(const Ship &oldShip, GameManager *mgr)
    : Ship(oldShip.getSymbol(), oldShip.getTeam()), manager(mgr) {
  // Copy lives
  int lostLives = DEFAULT_LIVES - oldShip.getLives();
  if (lostLives > 0) {
    this->takeDamage(
        lostLives); // so if old ship had 2 lives left, new ship also has 2
  }

  // Copy killCount
  for (int i = 0; i < oldShip.getKillCount(); i++) {
    incrementKills();
  }

  // Copy position
  setBattlefieldPtr(oldShip.getBattlefield());
  setPosition(oldShip.getPosition().x, oldShip.getPosition().y);
}

void Destroyer::look(int offsetX, int offsetY) {
  std::cout << getSymbol() << " (Destroyer) looks at offset(" << offsetX << ","
            << offsetY << ")\n";
}

void Destroyer::move() {
  // Move like a Battleship or Cruiser?
  // The assignment says it can do both: shoot like battleship, destroy like
  // cruiser We'll do a simple "move once in cardinal direction" This is just an
  // example
  Position p = getPosition();
  Battlefield *bf = getBattlefield();
  if (!bf)
    return;

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
      bf->getOccupant(nx, ny) == nullptr) {
    bf->setOccupant(p.x, p.y, nullptr);
    bf->setOccupant(nx, ny, this);
  }
}

void Destroyer::shoot(int targetX, int targetY) {
  // same logic as battleship shoot
  Battlefield *bf = getBattlefield();
  if (!bf)
    return;

  // we can impose city-block distance <=5
  Position p = getPosition();
  if (std::abs(p.x - targetX) + std::abs(p.y - targetY) > 5)
    return;

  Ship *occ = bf->getOccupant(targetX, targetY);
  if (occ && occ->getTeam() != getTeam()) {
    occ->takeDamage();
    if (!occ->isAlive()) {
      incrementKills();
      // If kills >= 3 => upgrade to super ship
      if (getKillCount() >= 3) {
        manager->upgradeShip(this, "SuperShip");
      }
    }
  }
}

void Destroyer::ram(int targetX, int targetY) {
  // same as Cruiser's ram
  Battlefield *bf = getBattlefield();
  if (!bf)
    return;
  Ship *occ = bf->getOccupant(targetX, targetY);
  if (occ && occ->getTeam() != getTeam()) {
    occ->takeDamage(occ->getLives());
    incrementKills();
    Position p = getPosition();
    bf->setOccupant(p.x, p.y, nullptr);
    bf->setOccupant(targetX, targetY, this);
    if (getKillCount() >= 3) {
      manager->upgradeShip(this, "SuperShip");
    }
  }
}

void Destroyer::performTurn() {
  // The assignment: "Destroyer" => can do both Cruiser-like ram or
  // Battleship-like shoot 1) look
  look(0, 0);

  // 2) attempt to ram a neighbor (like Cruiser) if found
  bool rammingDone = tryRamNeighbor();

  // 3) if no ram, move like battleship
  if (!rammingDone) {
    move();
  }

  // 4) shoot twice, for instance
  Position p = getPosition();
  for (int i = 0; i < 2; i++) {
    // pick random offset within city-block dist 5
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
  Position p = getPosition();
  if (!bf)
    return false;

  for (int dx = -1; dx <= 1; dx++) {
    for (int dy = -1; dy <= 1; dy++) {
      if (dx == 0 && dy == 0)
        continue;
      int nx = p.x + dx, ny = p.y + dy;
      if (nx < 0 || nx >= HEIGHT || ny < 0 || ny >= WIDTH)
        continue;
      Ship *occ = bf->getOccupant(nx, ny);
      if (occ && occ->getTeam() != getTeam()) {
        ram(nx, ny);
        return true;
      }
    }
  }
  return false;
}

// =============== FRIGATE ===============
const int Frigate::directions[Frigate::SEQ_LEN][2] = {
    {-1, 0}, {-1, 1}, {0, 1}, {1, 1}, {1, 0}, {1, -1}, {0, -1}, {-1, -1}};

Frigate::Frigate(const std::string &symbol, const std::string &team,
                 GameManager *mgr)
    : Ship(symbol, team), manager(mgr), firingIndex(0) {}

void Frigate::shoot(int targetX, int targetY) {
  // shoots immediate neighbor
  Battlefield *bf = getBattlefield();
  if (!bf)
    return;

  Position p = getPosition();
  // must be exactly 1 cell away? The assignment says immediate neighbor
  // (including diagonals).
  if (std::abs(p.x - targetX) <= 1 && std::abs(p.y - targetY) <= 1) {
    Ship *occ = bf->getOccupant(targetX, targetY);
    if (occ && occ->getTeam() != getTeam()) {
      occ->takeDamage();
      if (!occ->isAlive()) {
        incrementKills();
        if (getKillCount() >= 3) {
          manager->upgradeShip(this, "Corvette");
        }
      }
    }
  }
}

void Frigate::performTurn() {
  // does not move, does not look
  // fires in a fixed sequence
  Position p = getPosition();
  int dx = directions[firingIndex][0];
  int dy = directions[firingIndex][1];
  firingIndex = (firingIndex + 1) % SEQ_LEN;

  shoot(p.x + dx, p.y + dy);
}

// =============== CORVETTE ===============
Corvette::Corvette(const Ship &oldShip, GameManager *mgr)
    : Ship(oldShip.getSymbol(), oldShip.getTeam()), // copy symbol/team
      manager(mgr) {
  // Copy the old ship’s battlefield pointer
  setBattlefieldPtr(oldShip.getBattlefield());

  // Match the old ship’s position
  setPosition(oldShip.getPosition().x, oldShip.getPosition().y);

  // Match the old ship’s lives
  // e.g., if old had 2 left, new ship also gets 2
  int lostLives = DEFAULT_LIVES - oldShip.getLives();
  if (lostLives > 0) {
    takeDamage(lostLives);
  }

  // Copy kill count
  for (int i = 0; i < oldShip.getKillCount(); i++) {
    incrementKills();
  }
}

void Corvette::shoot(int targetX, int targetY) {
  Battlefield *bf = getBattlefield();
  if (!bf)
    return;

  Position p = getPosition();
  // same immediate neighbor check
  if (std::abs(p.x - targetX) <= 1 && std::abs(p.y - targetY) <= 1) {
    Ship *occ = bf->getOccupant(targetX, targetY);
    if (occ && occ->getTeam() != getTeam()) {
      occ->takeDamage();
      if (!occ->isAlive()) {
        incrementKills();
        // no further upgrade for corvette in the assignment
      }
    }
  }
}

void Corvette::performTurn() {
  // does not move, does not look
  // shoot a random neighbor
  Position p = getPosition();
  int dx = (rand() % 3) - 1; // -1..1
  int dy = (rand() % 3) - 1;
  // if dx=0,dy=0 => pick again?
  if (dx == 0 && dy == 0) {
    dx = 1; // or do something
  }
  shoot(p.x + dx, p.y + dy);
}

// =============== AMPHIBIOUS ===============
Amphibious::Amphibious(const std::string &symbol, const std::string &team,
                       GameManager *mgr)
    : Ship(symbol, team), manager(mgr) {}

void Amphibious::look(int offsetX, int offsetY) {
  std::cout << getSymbol() << " (Amphibious) looks at offset(" << offsetX << ","
            << offsetY << ")\n";
}

void Amphibious::move() {
  // Moves like a battleship, but can step onto islands
  // That means we ignore the occupant if it’s not empty? Actually, the
  // assignment says: "Amphibious can move in the sea or on islands," but not to
  // a cell with a friendly occupant?
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
    // We do not care if terrain[nx][ny] == 1 or 0 => amphibious can step there
    // But must not step onto a friendly occupant
    Ship *occ = bf->getOccupant(nx, ny);
    if (!occ) {
      // free, move
      bf->setOccupant(p.x, p.y, nullptr);
      bf->setOccupant(nx, ny, this);
    } else if (occ->getTeam() != getTeam()) {
      // If it's an enemy occupant => presumably do nothing or shoot?
      // The assignment doesn't mention amphibious ramming. We'll skip it
    }
  }
}

void Amphibious::shoot(int targetX, int targetY) {
  Battlefield *bf = getBattlefield();
  if (!bf)
    return;
  Position p = getPosition();
  // city-block <= 5
  if ((std::abs(p.x - targetX) + std::abs(p.y - targetY)) <= 5) {
    Ship *occ = bf->getOccupant(targetX, targetY);
    if (occ && occ->getTeam() != getTeam()) {
      occ->takeDamage();
      if (!occ->isAlive()) {
        incrementKills();
        if (getKillCount() >= 4) {
          manager->upgradeShip(this, "SuperShip");
        }
      }
    }
  }
}

void Amphibious::performTurn() {
  // 1) look
  look(0, 0);

  // 2) move
  move();

  // 3) shoot once or twice? The assignment says "behaves like a Battleship."
  // A battleship normally shoots 2 times.
  // We'll do 2 times to match that.
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

// =============== SUPERSHIP ===============
SuperShip::SuperShip(const Ship &oldShip, GameManager *mgr)
    : Ship(oldShip.getSymbol(), oldShip.getTeam()), // copy symbol/team
      manager(mgr) {
  // Point to the same battlefield
  setBattlefieldPtr(oldShip.getBattlefield());

  // Copy position
  setPosition(oldShip.getPosition().x, oldShip.getPosition().y);

  // Match old ship’s remaining lives
  int lostLives = DEFAULT_LIVES - oldShip.getLives();
  if (lostLives > 0) {
    takeDamage(lostLives);
  }

  // Copy kill count
  for (int i = 0; i < oldShip.getKillCount(); i++) {
    incrementKills();
  }
}

void SuperShip::look(int offsetX, int offsetY) {
  std::cout << getSymbol() << " (SuperShip) scanning offset(" << offsetX << ","
            << offsetY << ")\n";
}

void SuperShip::move() {
  // Moves like the Cruiser => which effectively looks for occupant, if enemy =>
  // ram
  moveLikeCruiser();
}

void SuperShip::shoot(int targetX, int targetY) {
  // We can shoot anywhere, no city-block distance limit in the assignment
  Battlefield *bf = getBattlefield();
  if (!bf)
    return;
  Ship *occ = bf->getOccupant(targetX, targetY);
  if (occ && occ->getTeam() != getTeam()) {
    occ->takeDamage();
    if (!occ->isAlive()) {
      incrementKills(); // no further upgrade
    }
  }
}

void SuperShip::ram(int targetX, int targetY) {
  // same approach as Cruiser or Destroyer
  Battlefield *bf = getBattlefield();
  if (!bf)
    return;
  Ship *occ = bf->getOccupant(targetX, targetY);
  if (occ && occ->getTeam() != getTeam()) {
    occ->takeDamage(occ->getLives());
    incrementKills();
    // Move into that position
    Position p = getPosition();
    bf->setOccupant(p.x, p.y, nullptr);
    bf->setOccupant(targetX, targetY, this);
  }
}

void SuperShip::performTurn() {
  // 1) look
  look(0, 0);

  // 2) move (like Cruiser => destroy occupant)
  move();

  // 3) shoot randomly at 3 locations
  shoot3RandomLocations();
}

void SuperShip::moveLikeCruiser() {
  // we check 8 neighbors for an enemy occupant to ram
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
      if (occ && occ->getTeam() != getTeam()) {
        ram(nx, ny);
        return; // done
      }
    }
  }
  // if no enemy occupant found, just pick a random neighbor to move
  // etc...
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
      bf->getOccupant(nx, ny) == nullptr) {
    bf->setOccupant(p.x, p.y, nullptr);
    bf->setOccupant(nx, ny, this);
  }
}

void SuperShip::shoot3RandomLocations() {
  Battlefield *bf = getBattlefield();
  if (!bf)
    return;
  // The assignment says "shoot randomly at 3 locations in the battlefield"
  // We'll pick 3 random cells in [0..HEIGHT-1, 0..WIDTH-1]
  for (int i = 0; i < 3; i++) {
    int rx = rand() % HEIGHT;
    int ry = rand() % WIDTH;
    shoot(rx, ry);
  }
}
