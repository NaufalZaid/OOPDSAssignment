// MovingShip.h
#ifndef MOVINGSHIP_H
#define MOVINGSHIP_H

#include "Ship.h"

class MovingShip : virtual public Ship {
public:
  virtual void move() = 0;
};

#endif
