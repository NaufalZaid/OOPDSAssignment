#include "ship.cpp"
#include <algorithm>
#include <cstdlib>

class Battleship : public MovingShip,
                   public ShootingShip,
                   public SeeingRobot,
                   public RamShip {
private:
  int shipsDestroyed = 0;
  const int MAX_FIRE_DISTANCE = 5;

public:
  Battleship(char sym = 'B', const string &teamName = "Blue")
      : Ship(sym), MovingShip(sym, teamName), ShootingShip(sym, teamName),
        SeeingRobot(sym, teamName), RamShip(sym, teamName) {
    setTeam(teamName);
  }

  void performTurn() {
    look();

    // Move once
    Direction moveDir = static_cast<Direction>(rand() % 4); // Non-diagonal
    move(moveDir);

    // Shoot twice
    for (int i = 0; i < 2; i++) {
      Direction shootDir = static_cast<Direction>(rand() % 8);
      int targetX, targetY;
      calculateTargetPosition(shootDir, targetX, targetY);

      // Check fire distance
      if (abs(targetX - pos.x) + abs(targetY - pos.y) <= MAX_FIRE_DISTANCE) {
        shoot(shootDir);
      }
    }
  }

  void incrementKills() { ++shipsDestroyed; }
  int getKills() const { return shipsDestroyed; }
};

class Cruiser : public MovingShip,
                public RamShip,
                public SeeingRobot,
                public ShootingShip {
private:
  int shipsDestroyed = 0;

public:
  Cruiser(char sym = 'C', string team = "Red")
      : Ship(sym, team), MovingShip(sym, team), RamShip(sym, team),
        SeeingRobot(sym, team), ShootingShip(sym, team) {}

  void performTurn() {
    look();

    // Check 3x3 neighborhood
    for (int dx = -1; dx <= 1; dx++) {
      for (int dy = -1; dy <= 1; dy++) {
        if (dx == 0 && dy == 0)
          continue;

        int targetX = pos.x + dx;
        int targetY = pos.y + dy;

        if (isWithinBoundary(targetX, targetY) &&
            !isUnoccupied(targetX, targetY)) {
          // Found enemy ship, move to destroy it
          Direction dir = calculateDirection(dx, dy);
          destroy(dir);
          return;
        }
      }
    }

    // If no enemies found, move randomly
    Direction moveDir = static_cast<Direction>(rand() % 8);
    move(moveDir);
  }

  void incrementKills() { ++shipsDestroyed; }

private:
  Direction calculateDirection(int dx, int dy) {
    // Convert dx,dy to Direction enum
    // Implementation
    return UP; // placeholder
  }
};

class Destroyer : public MovingShip,
                  public RamShip,
                  public SeeingRobot,
                  public ShootingShip {
private:
  int shipsDestroyed = 0;
  const int MAX_FIRE_DISTANCE = 5;

  Direction calculateDirection(int dx, int dy) {
    if (dx < 0) {
      if (dy < 0)
        return UP_LEFT;
      if (dy == 0)
        return UP;
      return UP_RIGHT;
    }
    if (dx == 0) {
      if (dy < 0)
        return LEFT;
      return RIGHT;
    }
    if (dy < 0)
      return DOWN_LEFT;
    if (dy == 0)
      return DOWN;
    return DOWN_RIGHT;
  }

public:
  // Constructor for fresh Destroyer
  Destroyer(char sym = 'D', const string &teamName = "Any")
      : Ship(sym), MovingShip(sym, teamName), ShootingShip(sym, teamName),
        RamShip(sym, teamName), SeeingRobot(sym, teamName) {
    setTeam(teamName);
  }

  // Constructor for upgrading from other ships
  Destroyer(const Ship &original)
      : Ship(original.getSymbol()),
        MovingShip(original.getSymbol(), original.getTeam()),
        ShootingShip(original.getSymbol(), original.getTeam()),
        RamShip(original.getSymbol(), original.getTeam()),
        SeeingRobot(original.getSymbol(), original.getTeam()) {
    setTeam(original.getTeam());
    pos = original.getPosition();
    lives = original.getLives();
  }

  void performTurn() {
    look(); // From SeeingRobot

    // First try to destroy like Cruiser
    bool enemyFound = false;
    for (int dx = -1; dx <= 1 && !enemyFound; dx++) {
      for (int dy = -1; dy <= 1; dy++) {
        if (dx == 0 && dy == 0)
          continue;

        int targetX = pos.x + dx;
        int targetY = pos.y + dy;

        if (isWithinBoundary(targetX, targetY) &&
            !isUnoccupied(targetX, targetY)) {
          Direction dir = calculateDirection(dx, dy);
          destroy(dir);
          enemyFound = true;
          break;
        }
      }
    }

    // If no enemies to destroy, act like Battleship
    if (!enemyFound) {
      // Move once (non-diagonal like Battleship)
      Direction moveDir = static_cast<Direction>(rand() % 4);
      move(moveDir);

      // Shoot twice with distance check
      for (int i = 0; i < 2; i++) {
        Direction shootDir = static_cast<Direction>(rand() % 8);
        int targetX, targetY;
        calculateTargetPosition(shootDir, targetX, targetY);

        if (abs(targetX - pos.x) + abs(targetY - pos.y) <= MAX_FIRE_DISTANCE) {
          shoot(shootDir);
        }
      }
    }
  }

  void incrementKills() {
    ++shipsDestroyed;
    cout << symbol << " is ready to upgrade to SuperShip!" << endl;
  }
};

class Frigate : public ShootingShip,
                public RamShip,
                public SeeingRobot,
                public MovingShip {
private:
  int currentDirection = 0;
  int shipsDestroyed = 0;
  static const Direction FIRING_SEQUENCE[8];

public:
  Frigate(char sym = 'F', string team = "Green")
      : Ship(sym, team), ShootingShip(sym, team), SeeingRobot(sym, team),
        RamShip(sym, team), MovingShip(sym, team) {}

  void performTurn() {
    shoot(FIRING_SEQUENCE[currentDirection]);
    currentDirection = (currentDirection + 1) % 8;
  }

  void incrementKills() {
    if (++shipsDestroyed >= 3) {
      // Upgrade to Corvette
      // Implementation of upgrade mechanism
    }
  }
};

class Corvette : public ShootingShip,
                 public RamShip,
                 public SeeingRobot,
                 public MovingShip {
public:
  Corvette(char sym = 'V', string team = "Yellow")
      : Ship(sym, team), ShootingShip(sym, team), SeeingRobot(sym, team),
        RamShip(sym, team), MovingShip(sym, team) {}

  void performTurn() {
    Direction shootDir = static_cast<Direction>(rand() % 8);
    shoot(shootDir);
  }
};

class Amphibious : public MovingShip,
                   public ShootingShip,
                   public RamShip,
                   public SeeingRobot {
private:
  int shipsDestroyed = 0;

public:
  Amphibious(char sym = 'A', string team = "Purple")
      : Ship(sym, team), MovingShip(sym, team), ShootingShip(sym, team),
        RamShip(sym, team), SeeingRobot(sym, team) {}

  void move(Direction dir) override {
    // Can move regardless of terrain
    int targetX = pos.x;
    int targetY = pos.y;
    calculateTargetPosition(dir, targetX, targetY);

    if (isWithinBoundary(targetX, targetY)) {
      battlefield[pos.x][pos.y] = EMPTY_CELL;
      pos.x = targetX;
      pos.y = targetY;
      battlefield[pos.x][pos.y] = symbol;
    }
  }

  void performTurn() {
    // Similar to Battleship but can move anywhere
    look();
    Direction moveDir = static_cast<Direction>(rand() % 4);
    move(moveDir);

    Direction shootDir = static_cast<Direction>(rand() % 8);
    shoot(shootDir);
  }

  void incrementKills() {
    if (++shipsDestroyed >= 4) {
      // Upgrade to SuperShip
      // Implementation of upgrade mechanism
    }
  }
};

class SuperShip : public MovingShip,
                  public ShootingShip,
                  public RamShip,
                  public SeeingRobot {
public:
  SuperShip(char sym = 'S', string team = "Gold")
      : Ship(sym, team), MovingShip(sym, team), ShootingShip(sym, team),
        RamShip(sym, team), SeeingRobot(sym, team) {}

  void performTurn() {
    look();

    // Move and destroy like Cruiser
    Direction moveDir = static_cast<Direction>(rand() % 8);
    destroy(moveDir);

    // Shoot at 3 random locations
    for (int i = 0; i < 3; i++) {
      Direction shootDir = static_cast<Direction>(rand() % 8);
      shoot(shootDir);
    }
  }
};

// Define static member
const Direction Frigate::FIRING_SEQUENCE[8] = {
    UP, UP_RIGHT, RIGHT, DOWN_RIGHT, DOWN, DOWN_LEFT, LEFT, UP_LEFT};