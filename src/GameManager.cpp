#include "GameManager.h"
#include "ShipTypes.h" // for Destroyer, SuperShip, Corvette, etc.
#include <algorithm>   // for std::find
#include <cstdlib>     // for rand()
#include <iostream>

GameManager::GameManager()
    : maxRespawnsPerTurn(2), maxShipRespawns(3),
      totalIterations(100) // default, can override from config
{}

GameManager::~GameManager() {
  // Clean up dynamically allocated ships
  for (Ship *s : ships) {
    delete s;
  }
  ships.clear();
}

void GameManager::setBattlefieldTerrain(const int grid[HEIGHT][WIDTH]) {
  battlefield.setTerrain(grid);
}

void GameManager::addShip(Ship *newShip) {
  if (!newShip)
    return;

  // Attach the single shared battlefield to the ship
  newShip->setBattlefieldPtr(&battlefield);

  // Try placing the ship randomly
  bool placed = battlefield.placeShipRandomly(newShip);
  if (!placed) {
    std::cout << "Warning: Could not place ship " << newShip->getSymbol()
              << "!\n";
    // The ship is still added to the list (but effectively off-map)
    // Alternatively, handle this as an error or store position as invalid
  }
  ships.push_back(newShip);
}

void GameManager::runSimulation(int iterations) {
  totalIterations = iterations;

  for (int turn = 1; turn <= totalIterations; turn++) {
    std::cout << "\n--- Turn " << turn << " ---\n";
    battlefield.display(std::cout);

    // Process any respawns first
    processRespawns();

    // Execute each ship's turn
    executeTurn(turn);

    // Check victory condition
    if (checkVictory()) {
      std::cout << "Victory condition met! Ending simulation.\n";
      break;
    }
  }

  std::cout << "\nSimulation ended after " << (int)totalIterations
            << " turns.\n";
}

void GameManager::executeTurn(int /*turnNumber*/) {
  // Let each *alive* ship perform its turn
  for (Ship *s : ships) {
    if (s->isAlive()) {
      s->performTurn();
    }
  }

  // After all have moved/shot, handle any deaths
  for (Ship *s : ships) {
    if (!s->isAlive()) {
      // If it can still respawn, enqueue it
      if (s->canRespawn(maxShipRespawns)) {
        enqueueRespawn(s);
      }
    }
  }

  // Remove dead ships from battlefield occupant array
  for (int x = 0; x < HEIGHT; x++) {
    for (int y = 0; y < WIDTH; y++) {
      Ship *occupant = battlefield.getOccupant(x, y);
      if (occupant && !occupant->isAlive()) {
        battlefield.setOccupant(x, y, nullptr);
      }
    }
  }
}

void GameManager::enqueueRespawn(Ship *deadShip) {
  // Add to respawnQueue
  respawnQueue.push_back(deadShip);
}

void GameManager::processRespawns() {
  int respawnsThisTurn = 0;
  // Attempt to respawn from the front
  auto it = respawnQueue.begin();
  while (it != respawnQueue.end() && respawnsThisTurn < maxRespawnsPerTurn) {
    Ship *s = *it;
    bool placed = battlefield.placeShipRandomly(s);
    if (placed) {
      s->incrementRespawnCount();
      it = respawnQueue.erase(it);
      respawnsThisTurn++;
    } else {
      // Could not place, maybe next
      ++it;
    }
  }
}

// The key method for upgrading
void GameManager::upgradeShip(Ship *oldShip, const std::string &newType) {
  // 1) Find oldShip in the ships vector
  auto it = std::find(ships.begin(), ships.end(), oldShip);
  if (it == ships.end()) {
    // Not found
    std::cout << "Error: upgradeShip could not find oldShip.\n";
    return;
  }

  // 2) Create the new derived ship
  Ship *newShip = nullptr;
  if (newType == "Destroyer") {
    // e.g. Destroyer(const Ship& oldShip, GameManager* gm)
    newShip = new Destroyer(*oldShip, this);
  } else if (newType == "SuperShip") {
    newShip = new SuperShip(*oldShip, this);
  } else if (newType == "Corvette") {
    newShip = new Corvette(*oldShip, this);
  } else {
    std::cout << "Unknown upgrade type: " << newType << std::endl;
    return;
  }

  if (!newShip) {
    std::cout << "Failed to create newShip.\n";
    return;
  }

  // 3) Get old position from oldShip
  Position oldPos = oldShip->getPosition();

  // 4) Remove old occupant on battlefield
  if (oldPos.x >= 0 && oldPos.x < HEIGHT && oldPos.y >= 0 && oldPos.y < WIDTH) {
    // If the oldShip was indeed on the map:
    battlefield.setOccupant(oldPos.x, oldPos.y, nullptr);
  }

  // 5) Place newShip in the same position
  battlefield.setOccupant(oldPos.x, oldPos.y, newShip);

  // 6) Replace old ship in the ships vector
  *it = newShip;

  // 7) Delete the old ship
  delete oldShip;

  std::cout << "Ship upgraded to " << newType << " at (" << oldPos.x << ", "
            << oldPos.y << ")\n";
}

bool GameManager::checkVictory() const {
  // If only one team remains, or no ships left
  std::string survivingTeam;
  for (auto s : ships) {
    if (s->isAlive()) {
      if (survivingTeam.empty()) {
        survivingTeam = s->getTeam();
      } else if (survivingTeam != s->getTeam()) {
        // More than one team alive => no victor yet
        return false;
      }
    }
  }

  if (!survivingTeam.empty()) {
    std::cout << "Team " << survivingTeam << " is victorious!\n";
    return true;
  }
  // If no survivingTeam, it's a draw (all dead) - treat as you like
  return false;
}
