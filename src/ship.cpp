// This file contains the implementation of the ship-related classes.

#include <iostream>
#include <cstdlib>
#include <ctime>
#include <string>

using namespace std;

// Battlefield Size
const int WIDTH = 10;
const int HEIGHT = 10;
const int topBoundary = 0;
const int leftSideBoundary = 0;

// Enum for Directions
enum Direction
{
    UP,
    DOWN,
    LEFT,
    RIGHT,
    UP_LEFT,
    UP_RIGHT,
    DOWN_LEFT,
    DOWN_RIGHT
};

// Abstract Base Class: Ship
class Ship
{
protected:
    int x, y;    // Current position of the ship on the battlefield
    int lives;   // Number of lives the ship has (default: 3)
    char symbol; // Symbol representing the ship (e.g., '*', '$')
    string team; // Team name ("Team A" or "Team B")
    int battlefield[HEIGHT][WIDTH];

public:
    Ship(char sym, string team) : symbol(sym), team(team), lives(3), x(-1), y(-1) {}

    virtual void move(Direction dir) = 0;
    virtual void look() = 0;
    virtual void shoot(Direction dir) = 0;
    virtual void destroy(Direction dir) = 0;

    bool isAlive() { return lives > 0; }
    void takeDamage()
    {
        if (--lives == 0)
            cout << symbol << " destroyed!\n";
    }

    void setPosition(int nx, int targetY)
    {
        x = nx;
        y = targetY;
    }
    int getargetX() { return x; }
    int getargetY() { return y; }
    char getSymbol() { return symbol; }
};

// Moving Ship Abstract Class
class MovingShip : virtual public Ship
{
public:
    MovingShip(char sym, string team) : Ship(sym, team) {}

    void move(Direction dir) override
    {
        int targetX = x, targetY = y;

        // Update position based on direction
        switch (dir)
        {
        case UP:
            targetX = x - 1;
            break;
        case DOWN:
            targetX = x + 1;
            break;
        case LEFT:
            targetY = y - 1;
            break;
        case RIGHT:
            targetY = y + 1;
            break;
        default:
            cout << symbol << " cannot move diagonally.\n";
            return;
        }

        // Check boundaries and if the cell is unoccupied
        if (targetX >= topBoundary && targetX < HEIGHT && targetY >= leftSideBoundary && targetY < WIDTH && battlefield[targetX][targetY] == 0)
        {
            battlefield[x][y] = 0;                  // Clear old position
            battlefield[targetX][targetY] = symbol; // Move to new position
            x = targetX;
            y = targetY;
            cout << symbol << " moved to (" << x << ", " << y << ").\n";
        }
        else
        {
            cout << symbol << " cannot move to (" << targetX << ", " << targetY << "). Blocked or out of bounds.\n";
        }
    }
};

// Shooting Ship Abstract Class
class ShootingShip : virtual public Ship
{
public:
    ShootingShip(char sym, string team) : Ship(sym, team) {}

    void shoot(Direction dir) override
    {
        int targetX = x, targetY = y;

        // Update target position based on the direction
        switch (dir)
        {
        case UP:
            targetX = x - 1;
            break;
        case DOWN:
            targetX = x + 1;
            break;
        case LEFT:
            targetY = y - 1;
            break;
        case RIGHT:
            targetY = y + 1;
            break;
        case UP_LEFT:
            targetX = x - 1;
            targetY = y - 1;
            break;
        case UP_RIGHT:
            targetX = x - 1;
            targetY = y + 1;
            break;
        case DOWN_LEFT:
            targetX = x + 1;
            targetY = y - 1;
            break;
        case DOWN_RIGHT:
            targetX = x + 1;
            targetY = y + 1;
            break;
        default:
            cout << symbol << " cannot shoot in this direction.\n";
            return;
        }

        // Ensure the target is within bounds and not shooting itself
        if (targetX >= topBoundary && targetX < HEIGHT && targetY >= leftSideBoundary && targetY < WIDTH && (targetX != x || targetY != y))
        {
            if (battlefield[targetX][targetY] != 0 && battlefield[targetX][targetY] != symbol)
            {
                cout << symbol << " shot and destroyed an enemy at (" << targetX << ", " << targetY << ").\n";
                battlefield[targetX][targetY] = 0; // Destroy the enemy ship
            }
            else
            {
                cout << symbol << " shot at (" << targetX << ", " << targetY << ") but missed.\n";
            }
        }
    }
};

// RamShip Class
class RamShip : virtual public Ship
{
public:
    RamShip(char sym, string team) : Ship(sym, team) {}

    void destroy(Direction dir) override
    {
        int targetX = x, targetY = y;

        // Update target position based on the direction
        switch (dir)
        {
        case UP:
            targetX = x - 1;
            break;
        case DOWN:
            targetX = x + 1;
            break;
        case LEFT:
            targetY = y - 1;
            break;
        case RIGHT:
            targetY = y + 1;
            break;
        default:
            cout << symbol << " cannot destroy ships diagonally.\n";
            return;
        }

        // Ensure the target is within bounds and not destroying itself
        if (targetX >= topBoundary && targetX < HEIGHT && targetY >= leftSideBoundary && targetY < WIDTH && (targetX != x || targetY != y))
        {
            if (battlefield[targetX][targetY] != 0)
            {
                cout << symbol << " destroyed an enemy at (" << targetX << ", " << targetY << ").\n";
                battlefield[targetX][targetY] = symbol; // Destroy the enemy ship
            }
            else
            {
                cout << symbol << " attempted to destroy at (" << targetX << ", " << targetY << ") but missed.\n";
            }
        }
    }
};

// SeeingRobot Class
class SeeingRobot : virtual public Ship
{
public:
    SeeingRobot(char sym, string team) : Ship(sym, team) {}

    void look() override
    {
        int targetX = x, targetY = y;

        // Update look position based on direction
        int seeingCount = 0;
        while (seeingCount < 8)
        {
            if (seeingCount == 0)
                targetX = x - 1;
            if (seeingCount == 1)
                targetX = x + 1;
            if (seeingCount == 2)
                targetY = y - 1;
            if (seeingCount == 3)
                targetY = y + 1;
            if (seeingCount == 4)
            {
                targetX = x - 1;
                targetY = y - 1;
            }
            if (seeingCount == 5)
            {
                targetX = x - 1;
                targetY = y + 1;
            }
            if (seeingCount == 6)
            {
                targetX = x + 1;
                targetY = y - 1;
            }
            if (seeingCount == 7)
            {
                targetX = x + 1;
                targetY = y + 1;
            }
            // Check if the position is within the battlefield
            if (targetX >= topBoundary && targetX < HEIGHT && targetY >= leftSideBoundary && targetY < WIDTH)
            {
                cout << symbol << " looked at (" << targetX << ", " << targetY << "). ";
                if (battlefield[targetX][targetY] == 0)
                {
                    cout << "It's empty.\n";
                }
                else
                {
                    cout << "Spotted an object: " << (char)battlefield[targetX][targetY] << "\n";
                }
            }
            else
            {
                cout << symbol << " looked outside the battlefield.\n";
            }
            seeingCount++;
        }
    }
};

// Battlefield Class
class Battlefield
{
private:
    int grid[HEIGHT][WIDTH];

public:
    Battlefield()
    {
        for (int i = 0; i < HEIGHT; i++)
            for (int j = 0; j < WIDTH; j++)
                grid[i][j] = 0; // Initialize grid with 0 (empty)
    }

    void placeShip(Ship *ship)
    {
        int x, y;
        do
        {
            x = rand() % HEIGHT;
            y = rand() % WIDTH;
        } while (grid[x][y] != 0);

        grid[x][y] = ship->getSymbol();
        ship->setPosition(x, y);
    }

    void display()
    {
        for (int i = 0; i < HEIGHT; i++)
        {
            for (int j = 0; j < WIDTH; j++)
            {
                cout << (grid[i][j] == 0 ? '.' : (char)grid[i][j]) << " ";
            }
            cout << endl;
        }
    }

    int *getGrid() { return &grid[0][0]; }
};
