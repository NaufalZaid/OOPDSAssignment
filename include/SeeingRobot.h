// SeeingRobot.h
#ifndef SEEINGROBOT_H
#define SEEINGROBOT_H

#include "Ship.h"

class SeeingRobot : virtual public Ship {
public:
  virtual void look(int offsetX, int offsetY) = 0;
};

#endif
