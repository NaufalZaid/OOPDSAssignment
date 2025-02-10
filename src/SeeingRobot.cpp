#include "SeeingRobot.h"
#include "Battlefield.h"

void SeeingRobot::look(int offsetX, int offsetY) {
  Battlefield *bf = getBattlefield();
  if (!bf)
    return;

  Position p = getPosition();
  // Center is (p.x + offsetX, p.y + offsetY)
  for (int dx = -1; dx <= 1; dx++) {
    for (int dy = -1; dy <= 1; dy++) {
      int checkX = p.x + offsetX + dx;
      int checkY = p.y + offsetY + dy;

      // Boundary check
      if (isWithinBoundary()) {
        std::cout << getSymbol() << " sees out of bounds at (" << checkX << ","
                  << checkY << ")\n";
      } else {
        Ship *occ = bf->getOccupant(checkX, checkY);
        if (!occ) {
          std::cout << getSymbol() << " sees empty cell (" << checkX << ","
                    << checkY << ")\n";
        } else if (occ == this) {
          std::cout << getSymbol() << " sees itself at (" << checkX << ","
                    << checkY << ")\n";
        } else {
          std::cout << getSymbol() << " sees " << occ->getSymbol() << " (Team "
                    << occ->getTeam() << ") at (" << checkX << "," << checkY
                    << ")\n";
        }
      }
    }
  }
}