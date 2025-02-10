#include "GameManager.h"
#include "ShipTypes.h"
#include <algorithm>
#include <cstdlib>
#include <iostream>

GameManager::GameManager()
    : maxRespawnsPerTurn(2), maxShipRespawns(3), totalIterations(100) {}

GameManager::~GameManager() {
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
  newShip->setBattlefieldPtr(&battlefield);

  bool placed = battlefield.placeShipRandomly(newShip);
  if (!placed) {
    std::cout << "Warning: Could not place ship " << newShip->getSymbol()
              << "!\n";
  }
  ships.push_back(newShip);
}

void GameManager::runSimulation(int iterations) {
  totalIterations = iterations;
  for (int turn = 1; turn <= totalIterations; turn++) {
    std::cout << "\n--- Turn " << turn << " ---\n";
    battlefield.display(std::cout);

    processRespawns(); // handle queue
    executeTurn(turn); // all ships do their turn
    handleUpgrades();  // new step: apply pending upgrades

    if (checkVictory()) {
      std::cout << "Victory condition met! Ending simulation.\n";
      break;
    }
  }
  std::cout << "\nSimulation ended after " << totalIterations << " turns.\n";
}

void GameManager::executeTurn(int turnNumber) {
  // 1) Each alive ship performs its turn
  for (Ship *s : ships) {
    if (s->isAlive()) {
      s->performTurn();
    }
  }

  // 2) Handle destroyed ships (respawn queue, etc.)
  for (Ship *s : ships) {
    if (!s->isAlive()) {
      if (s->canRespawn(maxShipRespawns)) {
        enqueueRespawn(s);
      }
    }
  }

  // 3) "Belt & Braces" occupant cleaning
  //    If occupant is not alive => set that cell to nullptr
  for (int x = 0; x < HEIGHT; x++) {
    for (int y = 0; y < WIDTH; y++) {
      Ship *occupant = battlefield.getOccupant(x, y);
      // occupant is a pointer; if the occupant is dead, remove it

      if (occupant && !occupant->isAlive()) {
        battlefield.setOccupant(x, y, nullptr);
      }
    }
  }
}

void GameManager::enqueueRespawn(Ship *deadShip) {
  respawnQueue.push_back(deadShip);
}

void GameManager::processRespawns() {
  int respawnsThisTurn = 0;
  auto it = respawnQueue.begin();
  while (it != respawnQueue.end() && respawnsThisTurn < maxRespawnsPerTurn) {
    Ship *s = *it;
    bool placed = battlefield.placeShipRandomly(s);
    if (placed) {
      s->incrementRespawnCount();
      it = respawnQueue.erase(it);
      respawnsThisTurn++;
    } else {
      ++it;
    }
  }
}

// NEW method: after each turn, we check if a ship requested upgrade
void GameManager::handleUpgrades() {
  for (Ship *s : ships) {
    // only do something if it's alive
    if (s->isAlive()) {
      std::string upgradeType = s->getPendingUpgradeType();
      if (!upgradeType.empty()) {
        upgradeShip(s, upgradeType);
        // no need to 'return;' because we are deferring
        // but we can break if you'd like to avoid re-checking the new ship
        // For safety, we might also do `s->clearPendingUpgrade()`
        // inside upgradeShip. But let's do it after:
        s->clearPendingUpgrade();
      }
    }
  }
}

void GameManager::upgradeShip(Ship *oldShip, const std::string &newType) {
  auto it = std::find(ships.begin(), ships.end(), oldShip);
  if (it == ships.end()) {
    std::cout << "Error: upgradeShip could not find oldShip.\n";
    return;
  }

  Ship *newShip = nullptr;
  if (newType == "Destroyer") {
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

  // old position
  Position oldPos = oldShip->getPosition();
  // remove occupant
  if (oldPos.x >= 0 && oldPos.x < HEIGHT && oldPos.y >= 0 && oldPos.y < WIDTH) {
    battlefield.setOccupant(oldPos.x, oldPos.y, nullptr);
  }

  std::cout << "Upgrading ship at (" << oldPos.x << ", " << oldPos.y
            << "): old ship " << oldShip << " -> new ship " << newShip << "\n";

  // place new occupant
  battlefield.setOccupant(oldPos.x, oldPos.y, newShip);

  // replace in ships vector
  *it = newShip;

  // delete old
  delete oldShip;
  battlefield.setOccupant(oldPos.x, oldPos.y, nullptr);

  std::cout << "Ship upgraded to " << newType << " at (" << oldPos.x << ", "
            << oldPos.y << ")\n";
}

bool GameManager::checkVictory() const {
  std::string survivingTeam;
  for (Ship *s : ships) {
    if (s->isAlive()) {
      if (survivingTeam.empty()) {
        survivingTeam = s->getTeam();
      } else if (survivingTeam != s->getTeam()) {
        // more than one team
        return false;
      }
    }
  }
  if (!survivingTeam.empty()) {
    std::cout << "Team " << survivingTeam << " is victorious!\n";
    return true;
  }
  // else no ships alive => draw
  return false;
}
