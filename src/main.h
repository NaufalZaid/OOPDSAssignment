#ifndef MAIN_H
#define MAIN_H

#include <algorithm>
#include <cstdlib>
#include <ctime>
#include <fstream>
#include <iostream>
#include <sstream>
#include <stdexcept>
#include <string>

// Constants
const int WIDTH = 10;
const int HEIGHT = 10;
const int TOP_BOUNDARY = 0;
const int LEFT_BOUNDARY = 0;
const int DEFAULT_LIVES = 3;
const char EMPTY_CELL = 0;
const char EMPTY_DISPLAY = '.';
const int MAX_SHIPS_TOTAL = 10;
const int MAX_TEAMS = 2;
const int MAX_SHIPS_PER_TEAM = 10;

// Forward declarations
class Ship;
class MovingShip;
class ShootingShip;
class RamShip;
class SeeingRobot;
class Battlefield;
class GameManager;
class GameParser;

// Basic structures
struct Position {
  int x, y;
  Position(int x = -1, int y = -1) : x(x), y(y) {}
};

struct DirectionVector {
  int dx, dy;
  DirectionVector(int dx = 0, int dy = 0) : dx(dx), dy(dy) {}
};

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

// Game configuration structures
struct ShipConfig {
  std::string type;
  char symbol;
  int count;
};

struct TeamConfig {
  std::string name;
  int shipTypeCount;
  ShipConfig ships[MAX_SHIPS_PER_TEAM];
};

struct GameConfig {
  int iterations;
  int width;
  int height;
  int battlefield[HEIGHT][WIDTH];
  int teamCount;
  TeamConfig teams[MAX_TEAMS];
};

// Ship class declarations
class Ship {
protected:
  Position pos;
  int lives;
  char symbol;
  std::string team;
  int battlefield[HEIGHT][WIDTH];
  int respawnCount = 0;
  int killCount = 0;
  const int MAX_RESPAWNS = 3;
  bool isUpgraded = false;

  static Ship *shipRegistry[MAX_SHIPS_TOTAL];
  static int shipCount;

  bool canSeeShip(int x, int y) const;
  std::string getTeamForSymbol(char sym) const;
  void calculateTargetPosition(Direction dir, int &targetX, int &targetY);
  bool isDiagonalDirection(Direction dir);
  bool isValidDirection(Direction dir, bool allowDiagonal = false);

public:
  Ship(char sym, std::string teamName = "");
  virtual ~Ship();

  virtual void move(Direction dir) = 0;
  virtual void look() = 0;
  virtual void shoot(Direction dir) = 0;
  virtual void destroy(Direction dir) = 0;
  virtual void performTurn() = 0;
  virtual void incrementKills();

  bool isWithinBoundary(int targetX, int targetY) const;
  bool isUnoccupied(int targetX, int targetY);
  bool isSelf(int targetX, int targetY);
  bool isAlive();
  void takeDamage();
  void setPosition(int x, int y);
  bool canRespawn() const;
  void incrementRespawns();

  int getX();
  int getY();
  char getSymbol() const;
  void setTeam(const std::string &teamName);
  std::string getTeam() const;
  Position getPosition() const;
  int getLives() const;
};

// Ship type base classes
class MovingShip : virtual public Ship {
public:
  MovingShip(char sym, std::string team);
  void move(Direction dir) override;
};

class ShootingShip : virtual public Ship {
public:
  ShootingShip(char sym, std::string team);
  void shoot(Direction dir) override;
};

class RamShip : virtual public Ship {
public:
  RamShip(char sym, std::string team);
  void destroy(Direction dir) override;
};

class SeeingRobot : virtual public Ship {
private:
  static const DirectionVector DIRECTIONS[8];

public:
  SeeingRobot(char sym, std::string team);
  void look() override;
};

// Specific ship classes
class Battleship : public MovingShip,
                   public ShootingShip,
                   public SeeingRobot,
                   public RamShip {
private:
  int shipsDestroyed = 0;
  const int MAX_FIRE_DISTANCE = 5;

public:
  Battleship(char sym = 'B', const std::string &teamName = "Blue");
  void performTurn() override;
  void incrementKills() override;
  void transformToDestroyer();
};

class Cruiser : public MovingShip,
                public RamShip,
                public SeeingRobot,
                public ShootingShip {
private:
  int shipsDestroyed = 0;

public:
  Cruiser(char sym = 'C', std::string team = "Red");
  void performTurn() override;
  void incrementKills() override;
};

class Destroyer : public MovingShip,
                  public RamShip,
                  public SeeingRobot,
                  public ShootingShip {
private:
  int shipsDestroyed = 0;
  const int MAX_FIRE_DISTANCE = 5;

  Direction calculateDirection(int dx, int dy);

public:
  Destroyer(char sym = 'D', const std::string &teamName = "Any");
  Destroyer(const Ship &original);
  void performTurn() override;
  void incrementKills() override;
  void transformToSuperShip();
};

class Frigate : public ShootingShip,
                public RamShip,
                public SeeingRobot,
                public MovingShip {
private:
  const Direction FIRING_SEQUENCE[8];
  int currentFiringIndex = 0;
  int shipsDestroyed = 0;

public:
  Frigate(char sym = 'F', std::string team = "Green");
  void performTurn() override;
  void incrementKills() override;
};

class Corvette : public ShootingShip,
                 public RamShip,
                 public SeeingRobot,
                 public MovingShip {
public:
  Corvette(char sym = 'V', std::string team = "Yellow");
  void performTurn() override;
};

class Amphibious : public MovingShip,
                   public ShootingShip,
                   public RamShip,
                   public SeeingRobot {
private:
  int shipsDestroyed = 0;

public:
  Amphibious(char sym = 'A', std::string team = "Purple");
  void move(Direction dir) override;
  void performTurn() override;
  void incrementKills() override;
};

class SuperShip : public MovingShip,
                  public ShootingShip,
                  public RamShip,
                  public SeeingRobot {
public:
  SuperShip(char sym = 'S', std::string team = "Gold");
  SuperShip(const Ship &original);
  void performTurn() override;
};

// Game management classes
class Battlefield {
private:
  int grid[HEIGHT][WIDTH];

public:
  Battlefield();
  void placeShip(Ship *ship);
  void display();
  int *getGrid();
  int getCell(int x, int y);
  void setGrid(const int newGrid[HEIGHT][WIDTH]);
};

class GameParser {
private:
  GameConfig config;
  bool parseHeader(std::ifstream &file);
  bool parseTeams(std::ifstream &file);

public:
  GameConfig parseFile(const std::string &filename);
  Ship *createShip(const std::string &type, char symbol,
                   const std::string &teamName);
};

template <typename T> class Queue {
private:
  T *data;
  int size;
  int capacity;
  int front;
  int rear;

public:
  Queue(int cap = 5);
  ~Queue();
  void push(T value);
  void pop();
  T Front();
  bool full();
  bool empty();
};

template <typename T> Queue<T>::Queue(int cap) {
  size = 0;
  capacity = cap;
  data = new T[capacity];
  front = rear = -1;
}

template <typename T> Queue<T>::~Queue() { delete[] data; }

template <typename T> void Queue<T>::push(T value) {
  if (full())
    throw std::runtime_error("Queue is full");

  if (empty())
    front = 0;
  rear = (rear + 1) % capacity;
  data[rear] = value;
  size++;
}

template <typename T> void Queue<T>::pop() {
  if (empty())
    throw std::runtime_error("Queue is empty");

  if (front == rear)
    front = rear = -1;
  else
    front = (front + 1) % capacity;
  size--;
}

template <typename T> T Queue<T>::Front() {
  if (empty())
    throw std::runtime_error("Queue is empty");
  return data[front];
}

template <typename T> bool Queue<T>::full() { return size == capacity; }

template <typename T> bool Queue<T>::empty() { return size == 0; }

class GameManager {
private:
  Queue<Ship *> respawnQueue;
  const int MAX_RESPAWNS_PER_TURN = 2;
  const int MAX_RESPAWNS_PER_SHIP = 3;
  std::ofstream logFile;
  Battlefield battlefield;
  Ship *ships[MAX_SHIPS_TOTAL];
  int shipCount;

  void processRespawnQueue();
  void logBattleState();
  void handleRespawns();
  bool checkTeamVictory();
  void initializeLog();
  void logTurn(int turnNumber);
  bool isTeamConflict(Ship *ship1, Ship *ship2) const;

public:
  GameManager();
  ~GameManager();
  static void replaceShip(Ship *oldShip, Ship *newShip);
  void setBattlefield(const int grid[HEIGHT][WIDTH]);
  void addShip(Ship *ship);
  void runSimulation(int iterations);
  void handleCombat(Ship *attacker, Ship *target);
};

#endif // MAIN_H