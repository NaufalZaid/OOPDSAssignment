// filepath: /OOPDS_Assignment/OOPDS_Assignment/src/main.cpp
#include <iostream>
#include <cstdlib>
#include <ctime>
#include "ship.cpp" // Include the ship-related classes

using namespace std;

// Battleship Class (Can Move, Shoot, Look)
class Battleship : public MovingShip, public ShootingShip, public SeeingRobot, public RamShip
{
public:
    Battleship(string team) : Ship('*', team), MovingShip('*', team), ShootingShip('*', team), SeeingRobot('*', team), RamShip('*', team) {}

    void destroy(Direction dir) override
    {
        cout << symbol << " cannot destroy ships directly by moving into them.\n";
    }

    void look()
    {
        cout << symbol << " looking around " << x << ", " << y << endl;
    }

    void move(Direction dir)
    {
        cout << symbol << " moving " << x << ", " << y << endl;
    }

    void shoot(Direction dir) override
    {
        cout << symbol << " shooting " << x << ", " << y << endl;
    }
};

// Game Manager Class
class GameManager
{
private:
    Battlefield battlefield;
    Ship *ships[10]; // Fixed-size array of ships
    int shipCount;

public:
    GameManager() : shipCount(0) {}

    void addShip(Ship *ship)
    {
        if (shipCount < 10)
        {
            ships[shipCount++] = ship;
            battlefield.placeShip(ship);
        }
    }

    void runSimulation(int iterations)
    {
        for (int i = 0; i < iterations; i++)
        {
            cout << "\n--- Turn " << i + 1 << " ---\n";
            battlefield.display();

            for (int j = 0; j < shipCount; j++)
            {
                cout << "\n"
                     << ships[j]->getSymbol() << "'s Turn:\n";

                // Randomly select directions for each action
                Direction randomDir = static_cast<Direction>(rand() % 4); // UP, DOWN, LEFT, RIGHT

                ships[j]->look();
                ships[j]->move(randomDir);

                // Shooting can also use diagonal directions
                Direction randomShootDir = static_cast<Direction>(rand() % 8); // Includes diagonals
                ships[j]->shoot(randomShootDir);
            }
        }
    }
};

// Main Function
int main()
{
    srand(time(0)); // Seed for random number generation
    GameManager game;

    Battleship *b1 = new Battleship("Team A");
    Battleship *b2 = new Battleship("Team B");

    game.addShip(b1);
    game.addShip(b2);

    game.runSimulation(50); // Run the simulation for 5 turns

    delete b1;
    delete b2;

    return 0;
}