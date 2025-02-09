// src/GameManager.h
#ifndef GAME_MANAGER_H
#define GAME_MANAGER_H

#include "Queue.h"
#include "ship.h"
#include <fstream>

struct ShipConfig {
  std::string type;
  char symbol;
  int count;
};

struct TeamConfig {
  std::string name;
  int shipTypeCount;
  ShipConfig ships[10]; // Using MAX_SHIPS_PER_TEAM
};

struct GameConfig {
  int iterations;
  int width;
  int height;
  int battlefield[10][10]; // Using HEIGHT and WIDTH
  int teamCount;
  TeamConfig teams[2]; // Using MAX_TEAMS
};

class GameParser {
public:
  GameConfig parseFile(const std::string &filename);
  Ship *createShip(const std::string &type, char symbol,
                   const std::string &teamName);

private:
  bool parseHeader(std::ifstream &file);
  bool parseTeams(std::ifstream &file);
  GameConfig config;
};

class GameManager {
public:
  GameManager();
  ~GameManager();

  void setBattlefield(const int grid[10][10]);
  void addShip(Ship *ship);
  void runSimulation(int iterations);
  static void replaceShip(Ship *oldShip, Ship *newShip);

private:
  Queue<Ship *> respawnQueue;
  Ship *ships[10]; // Using MAX_SHIPS_TOTAL
  int shipCount;
  Battlefield battlefield;
  std::ofstream logFile;

  void handleUpgrades();
  void processRespawnQueue();
  void logBattleState();
  void handleRespawns();
  bool checkTeamVictory();
  void initializeLog();
  void logTurn(int turnNumber);
  void handleShipUpgrade(Ship *ship);
  bool isTeamConflict(Ship *ship1, Ship *ship2) const;
  void handleCombat(Ship *attacker, Ship *target);
};

#endif // GAME_MANAGER_H