// ShootingShip.h
#ifndef SHOOTINGSHIP_H
#define SHOOTINGSHIP_H

#include "Ship.h"

class ShootingShip : virtual public Ship {
public:
  virtual void shoot(int targetX, int targetY) = 0;
};

#endif
