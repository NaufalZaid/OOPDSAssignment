#ifndef BATTLEFIELD_H
#define BATTLEFIELD_H

#include "Constants.h"
#include <iostream>
#include <string>
#include <vector>

// Forward declaration
class Ship;

/**
 * Battlefield class
 * - Maintains a 2D array or vector to represent the battlefield layout
 *   (0 => water, 1 => island, or it can store occupant references).
 * - Provides methods to place / move ships, check occupancy, display, etc.
 *
 * Implementation will appear in Battlefield.cpp (Set 3).
 */
class Battlefield {
private:
  // We store the map of 0s & 1s from the config file, plus occupant info
  // For occupant, we might keep a pointer to a Ship, or we just store a char.
  // Let's keep a 2D of ints for the map, plus a 2D of Ship* for occupant.
  int terrain[HEIGHT][WIDTH];    // 0=water, 1=island
  Ship *occupant[HEIGHT][WIDTH]; // null if no ship

public:
  Battlefield();
  ~Battlefield() {}

  // Called after reading from config
  void setTerrain(const int newGrid[HEIGHT][WIDTH]);

  // Checking or modifying occupant
  bool isIsland(int x, int y) const;
  bool isOccupied(int x, int y) const;
  Ship *getOccupant(int x, int y) const;
  void setOccupant(int x, int y, Ship *shipPtr);

  // Place a new ship at a random valid location (water & not occupied)
  // Return true if successful, false otherwise
  bool placeShipRandomly(Ship *shipPtr);

  // Utility to display the entire map
  void display(std::ostream &out = std::cout) const;
};

#endif // BATTLEFIELD_H
