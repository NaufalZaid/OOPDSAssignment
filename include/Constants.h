#ifndef CONSTANTS_H
#define CONSTANTS_H

// You can move these into a config or read them from file if you prefer dynamic
// sizing
static const int WIDTH = 10;
static const int HEIGHT = 10;

// Symbol to represent an empty cell in the battlefield array
static const char EMPTY_CELL = 0;
static const char EMPTY_DISPLAY = '.';

// For bounding movement
static const int TOP_BOUNDARY = 0;
static const int LEFT_BOUNDARY = 0;

// Default lives per ship
static const int DEFAULT_LIVES = 3;

// Maximum number of total ships (if you want a strict limit). For flexible
// approach, use std::vector
static const int MAX_SHIPS_TOTAL = 50;

// Directions
enum Direction {
  UP,
  DOWN,
  LEFT,
  RIGHT,
  UP_LEFT,
  UP_RIGHT,
  DOWN_LEFT,
  DOWN_RIGHT
};

#endif // CONSTANTS_H
