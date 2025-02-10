#include "Battlefield.h"
#include "Ship.h"
#include <cstdlib> // For rand(), srand()
#include <ctime>   // For time()
#include <iostream>

// Constructor
Battlefield::Battlefield() {
  // Initialize terrain and occupant arrays
  for (int x = 0; x < HEIGHT; x++) {
    for (int y = 0; y < WIDTH; y++) {
      terrain[x][y] = 0;        // default: water
      occupant[x][y] = nullptr; // no ship
    }
  }
  // Initialize random seed once (optional, can do in main)
  std::srand((unsigned)std::time(nullptr));
}

void Battlefield::setTerrain(const int newGrid[HEIGHT][WIDTH]) {
  for (int x = 0; x < HEIGHT; x++) {
    for (int y = 0; y < WIDTH; y++) {
      terrain[x][y] = newGrid[x][y];
      occupant[x][y] = nullptr; // reset occupant
    }
  }
}

bool Battlefield::isIsland(int x, int y) const {
  // Make sure x,y are in bounds if used externally
  return (terrain[x][y] == 1);
}

bool Battlefield::isOccupied(int x, int y) const {
  return (occupant[x][y] != nullptr);
}

Ship *Battlefield::getOccupant(int x, int y) const { return occupant[x][y]; }

void Battlefield::setOccupant(int x, int y, Ship *shipPtr) {
  std::cout << "setOccupant(" << x << "," << y << ") -> " << shipPtr;
  if (shipPtr) {
    std::cout << " (symbol: " << shipPtr->getSymbol()
              << ", team: " << shipPtr->getTeam() << ")";
  }
  std::cout << std::endl;
  occupant[x][y] = shipPtr;
  if (shipPtr) {
    shipPtr->setPosition(x, y);
  }
}

bool Battlefield::placeShipRandomly(Ship *shipPtr) {
  // Attempt a certain number of random placements
  // to find free water cell
  const int MAX_TRIES = 100;
  for (int i = 0; i < MAX_TRIES; i++) {
    int rx = std::rand() % HEIGHT;
    int ry = std::rand() % WIDTH;

    // If it's water (0) and unoccupied, place here
    if (!isIsland(rx, ry) && !isOccupied(rx, ry)) {
      setOccupant(rx, ry, shipPtr);
      return true;
    }
  }
  return false; // Could not place
}

void Battlefield::display(std::ostream &out) const {
  out << "   ";
  for (int col = 0; col < WIDTH; col++) {
    out << col << " ";
  }
  out << "\n";

  for (int x = 0; x < HEIGHT; x++) {
    out << x << ": ";
    for (int y = 0; y < WIDTH; y++) {
      if (terrain[x][y] == 1) {
        // Island cell
        out << "# ";
      } else if (occupant[x][y] == nullptr) {
        // Water cell, no ship
        out << EMPTY_DISPLAY << " ";
      } else {
        // There's a ship here, display a single char from its symbol or any
        // logic For multi-char symbol, you might show the first char or
        // something else
        std::string sym = occupant[x][y]->getSymbol();
        out << (sym.empty() ? '?' : sym[0]) << " ";
      }
    }
    out << "\n";
  }
}
