#include <cstdlib>
#include <ctime>
#include <iostream>
#include <string>

using namespace std;

// Constants
const int WIDTH = 10;
const int HEIGHT = 10;
const int TOP_BOUNDARY = 0;
const int LEFT_BOUNDARY = 0;
const int DEFAULT_LIVES = 3;
const char EMPTY_CELL = 0;
const char EMPTY_DISPLAY = '.';

// Position structure
struct Position {
  int x, y;
  Position(int x = -1, int y = -1) : x(x), y(y) {}
};

// DirectionVector structure
struct DirectionVector {
  int dx, dy;
  DirectionVector(int dx = 0, int dy = 0) : dx(dx), dy(dy) {}
};

// Direction enum
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

// Abstract Base Class: Ship
class Ship {
protected:
  Position pos;
  int lives;
  char symbol;
  string team;
  int battlefield[HEIGHT][WIDTH];

  void calculateTargetPosition(Direction dir, int &targetX, int &targetY) {
    targetX = pos.x;
    targetY = pos.y;

    switch (dir) {
    case UP:
      targetX--;
      break;
    case DOWN:
      targetX++;
      break;
    case LEFT:
      targetY--;
      break;
    case RIGHT:
      targetY++;
      break;
    case UP_LEFT:
      targetX--;
      targetY--;
      break;
    case UP_RIGHT:
      targetX--;
      targetY++;
      break;
    case DOWN_LEFT:
      targetX++;
      targetY--;
      break;
    case DOWN_RIGHT:
      targetX++;
      targetY++;
      break;
    }
  }

  bool isDiagonalDirection(Direction dir) { return (dir > RIGHT); }

  bool isValidDirection(Direction dir, bool allowDiagonal = false) {
    if (!allowDiagonal && (isDiagonalDirection(dir))) {
      cout << symbol << " cannot move diagonally.\n";
      return false;
    }
    return true;
  }

public:
  Ship(char sym, string team)
      : symbol(sym), team(team), lives(DEFAULT_LIVES), pos(-1, -1) {}

  virtual void move(Direction dir) = 0;
  virtual void look() = 0;
  virtual void shoot(Direction dir) = 0;
  virtual void destroy(Direction dir) = 0;

  bool isWithinBoundary(int targetX, int targetY) {
    return targetX >= TOP_BOUNDARY && targetX < HEIGHT &&
           targetY >= LEFT_BOUNDARY && targetY < WIDTH;
  }

  bool isUnoccupied(int targetX, int targetY) {
    return battlefield[targetX][targetY] == EMPTY_CELL;
  }

  bool isSelf(int targetX, int targetY) {
    return targetX != pos.x || targetY != pos.y;
  }

  bool isAlive() { return lives > 0; }

  void takeDamage() {
    if (--lives == 0) {
      cout << symbol << " destroyed!\n";
    }
  }

  void setPosition(int x, int y) {
    pos.x = x;
    pos.y = y;
  }

  int getX() { return pos.x; }
  int getY() { return pos.y; }
  char getSymbol() { return symbol; }
};

// Moving Ship Abstract Class
class MovingShip : virtual public Ship {
public:
  MovingShip(char sym, string team) : Ship(sym, team) {}

  void move(Direction dir) override {
    if (!isValidDirection(dir, false)) {
      return;
    }

    int targetX, targetY;
    calculateTargetPosition(dir, targetX, targetY);

    if (isWithinBoundary(targetX, targetY) && isUnoccupied(targetX, targetY)) {
      battlefield[pos.x][pos.y] = EMPTY_CELL;
      battlefield[targetX][targetY] = symbol;
      pos.x = targetX;
      pos.y = targetY;
      cout << symbol << " moved to (" << pos.x << ", " << pos.y << ").\n";
    } else {
      cout << symbol << " cannot move to (" << targetX << ", " << targetY
           << "). Blocked or out of bounds.\n";
    }
  }
};

// Shooting Ship Abstract Class
class ShootingShip : virtual public Ship {
public:
  ShootingShip(char sym, string team) : Ship(sym, team) {}

  void shoot(Direction dir) override {
    int targetX, targetY;
    calculateTargetPosition(dir, targetX, targetY);

    if (isWithinBoundary(targetX, targetY) && isSelf(targetX, targetY)) {
      if (battlefield[targetX][targetY] != EMPTY_CELL &&
          battlefield[targetX][targetY] != symbol) {
        cout << symbol << " shot and destroyed an enemy at (" << targetX << ", "
             << targetY << ").\n";
        battlefield[targetX][targetY] = EMPTY_CELL;
      } else {
        cout << symbol << " shot at (" << targetX << ", " << targetY
             << ") but missed.\n";
      }
    }
  }
};

// RamShip Class
class RamShip : virtual public Ship {
public:
  RamShip(char sym, string team) : Ship(sym, team) {}

  void destroy(Direction dir) override {
    if (!isValidDirection(dir, false)) {
      return;
    }

    int targetX, targetY;
    calculateTargetPosition(dir, targetX, targetY);

    if (isWithinBoundary(targetX, targetY) && isSelf(targetX, targetY)) {
      if (battlefield[targetX][targetY] != EMPTY_CELL) {
        cout << symbol << " destroyed an enemy at (" << targetX << ", "
             << targetY << ").\n";
        battlefield[targetX][targetY] = symbol;
      } else {
        cout << symbol << " attempted to destroy at (" << targetX << ", "
             << targetY << ") but missed.\n";
      }
    }
  }
};

// SeeingRobot Class
class SeeingRobot : virtual public Ship {
private:
  static const DirectionVector DIRECTIONS[8];

public:
  SeeingRobot(char sym, string team) : Ship(sym, team) {}

  void look() override {
    for (int i = 0; i < 8; i++) {
      int targetX = pos.x + DIRECTIONS[i].dx;
      int targetY = pos.y + DIRECTIONS[i].dy;

      string result;
      if (!isWithinBoundary(targetX, targetY)) {
        result = "Looked outside the battlefield.";
      } else if (isUnoccupied(targetX, targetY)) {
        result = "It's empty.";
      } else {
        result = "Spotted an object: " +
                 string(1, (char)battlefield[targetX][targetY]);
      }

      cout << symbol << " looked at direction " << i << ": " << result << endl;
    }
  }
};

const DirectionVector SeeingRobot::DIRECTIONS[8] = {
    DirectionVector(-1, 0),  // UP
    DirectionVector(1, 0),   // DOWN
    DirectionVector(0, -1),  // LEFT
    DirectionVector(0, 1),   // RIGHT
    DirectionVector(-1, -1), // UP_LEFT
    DirectionVector(-1, 1),  // UP_RIGHT
    DirectionVector(1, -1),  // DOWN_LEFT
    DirectionVector(1, 1)    // DOWN_RIGHT
};

// Battlefield Class
class Battlefield {
private:
  int grid[HEIGHT][WIDTH];

public:
  Battlefield() {
    for (int i = 0; i < HEIGHT; i++)
      for (int j = 0; j < WIDTH; j++)
        grid[i][j] = EMPTY_CELL;
  }

  void placeShip(Ship *ship) {
    Position pos;
    do {
      pos.x = rand() % HEIGHT;
      pos.y = rand() % WIDTH;
    } while (grid[pos.x][pos.y] != EMPTY_CELL);

    grid[pos.x][pos.y] = ship->getSymbol();
    ship->setPosition(pos.x, pos.y);
  }

  void display() {
    for (int i = 0; i < HEIGHT; i++) {
      for (int j = 0; j < WIDTH; j++) {
        cout << (grid[i][j] == EMPTY_CELL ? EMPTY_DISPLAY : (char)grid[i][j])
             << " ";
      }
      cout << endl;
    }
  }

  int *getGrid() { return &grid[0][0]; }
};