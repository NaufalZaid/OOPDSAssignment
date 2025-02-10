#ifndef SHIPTYPES_H
#define SHIPTYPES_H

#include "GameManager.h" // for upgradeShip calls, if needed
#include "MovingShip.h"
#include "RamShip.h"
#include "SeeingRobot.h"
#include "Ship.h"
#include "ShootingShip.h"
#include <string>

/**
 * Battleship:
 *  - Inherits: MovingShip, ShootingShip, SeeingRobot
 *  - No RamShip (until upgraded to Destroyer).
 *  - City-block distance <= 5 for shooting.
 *  - If killCount >= 4 => becomes Destroyer.
 */
class Battleship : public MovingShip, public ShootingShip, public SeeingRobot {
public:
  Battleship(const std::string &symbol, const std::string &team,
             GameManager *manager);

  // Overridden capabilities
  virtual void look(int offsetX, int offsetY) override;  // from SeeingRobot
  virtual void move() override;                          // from MovingShip
  virtual void shoot(int targetX, int targetY) override; // from ShootingShip

  // Turn logic
  virtual void performTurn() override; // calls look->move->shoot

private:
  GameManager *manager;
  static const int MAX_CITY_BLOCK_DIST = 5;

  // Helper to pick a random valid move direction
  // (cannot move onto cell with another ship)
  void decideAndMove();

  // Helper to shoot random valid positions within city-block dist
  void shootTwiceRandomPositions();

  // checks if |dx|+|dy| <= 5
  bool withinCityBlockRange(int originX, int originY, int targetX,
                            int targetY) const;
};

/**
 * Cruiser:
 *  - Inherits: MovingShip, SeeingRobot, RamShip
 *  - No ShootingShip initially.
 *  - Looks at 3x3, moves to a neighbor. If that neighbor is occupied by enemy
 * => destroy it.
 *  - If killCount >= 3 => becomes Destroyer.
 */
class Cruiser : public MovingShip, public SeeingRobot, public RamShip {
public:
  Cruiser(const std::string &symbol, const std::string &team,
          GameManager *manager);

  // Overridden capabilities
  virtual void look(int offsetX, int offsetY) override;
  virtual void move() override;
  virtual void ram(int targetX, int targetY) override;

  // Turn logic
  virtual void performTurn() override;

private:
  GameManager *manager;
  void moveToPreferredNeighbor(); // tries to find an enemy in the 3x3
};

/**
 * Destroyer:
 *  - Inherits: MovingShip, ShootingShip, SeeingRobot, RamShip
 *  - Shoots like Battleship, rams like Cruiser
 *  - If killCount >= 3 => becomes SuperShip
 */
class Destroyer : public MovingShip,
                  public ShootingShip,
                  public SeeingRobot,
                  public RamShip {
public:
  Destroyer(const Ship &oldShip, GameManager *mgr);

  // Overridden capabilities
  virtual void look(int offsetX, int offsetY) override;
  virtual void move() override;
  virtual void shoot(int targetX, int targetY) override;
  virtual void ram(int targetX, int targetY) override;

  // Turn logic
  virtual void performTurn() override;

private:
  GameManager *manager;
  bool tryRamNeighbor(); // check 3x3 for enemy to ram
};

/**
 * Frigate:
 *  - Inherits: ShootingShip only
 *  - Does not move, does not look.
 *  - Shoots one immediate neighbor each turn in a fixed sequence (UP ->
 * UP_RIGHT -> ...).
 *  - If killCount >= 3 => becomes Corvette
 */
class Frigate : public ShootingShip {
public:
  Frigate(const std::string &symbol, const std::string &team,
          GameManager *manager);

  // Overridden capabilities
  virtual void shoot(int targetX, int targetY) override; // used internally only
  // We do NOT define move() or look() or ram() => doesn't have them

  virtual void performTurn() override;

private:
  GameManager *manager;
  int firingIndex; // tracks which neighbor to shoot next

  // A fixed sequence of (dx, dy) in clockwise order
  static const int SEQ_LEN = 8;
  static const int directions[SEQ_LEN][2];
};

/**
 * Corvette:
 *  - Inherits: ShootingShip
 *  - Does not move or look
 *  - Shoots one random neighbor each turn
 */
class Corvette : public ShootingShip {
public:
  Corvette(const Ship &oldShip, GameManager *mgr);

  virtual void shoot(int targetX, int targetY) override;
  virtual void performTurn() override;

private:
  GameManager *manager;
};

/**
 * Amphibious:
 *  - Inherits: MovingShip, ShootingShip, SeeingRobot
 *  - Moves like a Battleship, but CAN step onto islands (terrain=1).
 *  - If killCount >= 4 => becomes SuperShip
 */
class Amphibious : public MovingShip, public ShootingShip, public SeeingRobot {
public:
  Amphibious(const std::string &symbol, const std::string &team,
             GameManager *manager);

  virtual void look(int offsetX, int offsetY) override;
  virtual void move() override;
  virtual void shoot(int targetX, int targetY) override;

  virtual void performTurn() override;

private:
  GameManager *manager;
};

/**
 * SuperShip:
 *  - Inherits: MovingShip, ShootingShip, SeeingRobot, RamShip
 *  - Moves like the Cruiser, destroying any occupant in its path.
 *  - Shoots randomly at 3 locations each turn (anywhere in the battlefield).
 */
class SuperShip : public MovingShip,
                  public ShootingShip,
                  public SeeingRobot,
                  public RamShip {
public:
  SuperShip(const Ship &oldShip, GameManager *mgr);

  virtual void look(int offsetX, int offsetY) override;
  virtual void move() override;
  virtual void shoot(int targetX, int targetY) override;
  virtual void ram(int targetX, int targetY) override;

  virtual void performTurn() override;

private:
  GameManager *manager;

  void moveLikeCruiser();
  void shoot3RandomLocations();
};

#endif // SHIPTYPES_H
