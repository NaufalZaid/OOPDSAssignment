#ifndef SHIPTYPES_H
#define SHIPTYPES_H

#include "GameManager.h"
#include "MovingShip.h"
#include "RamShip.h"
#include "SeeingRobot.h"
#include "Ship.h"
#include "ShootingShip.h"
#include <string>

/**
 * Battleship:
 *  - Inherits: MovingShip, ShootingShip, SeeingRobot
 *  - City-block distance <= 5 for shooting
 *  - If killCount >= 4 => becomes Destroyer
 */
class Battleship : public MovingShip, public ShootingShip, public SeeingRobot {
private:
  GameManager *manager;
  static const int MAX_CITY_BLOCK_DIST = 5;

  void decideAndMove();             // move once
  void shootTwiceRandomPositions(); // shoot 2 times
  bool withinCityBlockRange(int originX, int originY, int targetX,
                            int targetY) const;

public:
  Battleship(const std::string &symbol, const std::string &team,
             GameManager *manager);

  // Overridden from base classes:
  virtual void look(int offsetX, int offsetY) override;
  virtual void move() override;
  virtual void shoot(int targetX, int targetY) override;
  virtual void performTurn() override;
};

/**
 * Cruiser:
 *  - Inherits: MovingShip, SeeingRobot, RamShip
 *  - Looks at 3x3, moves to neighbor, if occupant => ram
 *  - If killCount >= 3 => becomes Destroyer
 */
class Cruiser : public MovingShip, public SeeingRobot, public RamShip {
private:
  GameManager *manager;
  void moveToPreferredNeighbor();

public:
  Cruiser(const std::string &symbol, const std::string &team,
          GameManager *manager);

  virtual void look(int offsetX, int offsetY) override;
  virtual void move() override;
  virtual void ram(int targetX, int targetY) override;
  virtual void performTurn() override;
};

/**
 * Destroyer:
 *  - Inherits: MovingShip, ShootingShip, SeeingRobot, RamShip
 *  - Can be created from scratch OR upgraded from a Battleship or Cruiser
 */
class Destroyer : public MovingShip,
                  public ShootingShip,
                  public SeeingRobot,
                  public RamShip {
private:
  GameManager *manager;
  bool tryRamNeighbor();

public:
  // Fresh creation constructor (if the config wants a direct Destroyer)
  Destroyer(const std::string &symbol, const std::string &team,
            GameManager *mgr);

  // Upgrade constructor (Battleship or Cruiser => Destroyer)
  Destroyer(const Ship &oldShip, GameManager *mgr);

  virtual void look(int offsetX, int offsetY) override;
  virtual void move() override;
  virtual void shoot(int targetX, int targetY) override;
  virtual void ram(int targetX, int targetY) override;
  virtual void performTurn() override;
};

/**
 * Frigate:
 *  - Inherits: ShootingShip
 *  - Shoots one immediate neighbor each turn in a fixed sequence
 *  - If killCount >= 3 => becomes Corvette
 */
class Frigate : public ShootingShip {
private:
  GameManager *manager;
  int firingIndex;

  static const int SEQ_LEN = 8;
  static const int directions[SEQ_LEN][2];

public:
  Frigate(const std::string &symbol, const std::string &team,
          GameManager *manager);

  virtual void shoot(int targetX, int targetY) override;
  virtual void performTurn() override;
};

/**
 * Corvette:
 *  - Inherits: ShootingShip
 *  - Does not move/look
 *  - Shoots a random neighbor each turn
 *  - Created from scratch OR upgraded from Frigate
 */
class Corvette : public ShootingShip {
private:
  GameManager *manager;

public:
  // fresh creation constructor
  Corvette(const std::string &symbol, const std::string &team,
           GameManager *mgr);

  // upgrade constructor (Frigate => Corvette)
  Corvette(const Ship &oldShip, GameManager *mgr);

  virtual void shoot(int targetX, int targetY) override;
  virtual void performTurn() override;
};

/**
 * Amphibious:
 *  - Inherits: MovingShip, ShootingShip, SeeingRobot
 *  - Moves like Battleship, can step onto islands
 *  - If killCount >= 4 => becomes SuperShip
 */
class Amphibious : public MovingShip, public ShootingShip, public SeeingRobot {
private:
  GameManager *manager;

public:
  Amphibious(const std::string &symbol, const std::string &team,
             GameManager *manager);

  virtual void look(int offsetX, int offsetY) override;
  virtual void move() override;
  virtual void shoot(int targetX, int targetY) override;
  virtual void performTurn() override;
};

/**
 * SuperShip:
 *  - Inherits: MovingShip, ShootingShip, SeeingRobot, RamShip
 *  - Moves like Cruiser (destroy occupant)
 *  - Shoots randomly at 3 positions
 *  - Created from scratch OR from Destroyer/Amphibious
 */
class SuperShip : public MovingShip,
                  public ShootingShip,
                  public SeeingRobot,
                  public RamShip {
private:
  GameManager *manager;
  void moveLikeCruiser();
  void shoot3RandomLocations();

public:
  // Fresh creation
  SuperShip(const std::string &symbol, const std::string &team,
            GameManager *mgr);

  // Upgrade
  SuperShip(const Ship &oldShip, GameManager *mgr);

  virtual void look(int offsetX, int offsetY) override;
  virtual void move() override;
  virtual void shoot(int targetX, int targetY) override;
  virtual void ram(int targetX, int targetY) override;
  virtual void performTurn() override;
};

#endif // SHIPTYPES_H
