// RamShip.h
#ifndef RAMSHIP_H
#define RAMSHIP_H

#include "Ship.h"

class RamShip : virtual public Ship {
public:
  virtual void ram(int targetX, int targetY) = 0;
};

#endif
