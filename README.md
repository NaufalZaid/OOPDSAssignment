# OOPDS_Assignment
This project is a simulation game involving ships, where players can control different types of ships on a battlefield.

## Project Structure
- **src/main.cpp**: Contains the main function of the application. It initializes the game manager, creates battleship instances, adds them to the game, and runs the simulation for a specified number of turns.
  
- **src/ship.cpp**: Implements the ship-related classes, including:
  - **Ship**: An abstract base class representing a ship with properties like position, lives, symbol, and team. It declares virtual methods for movement, shooting, and looking.
  - **MovingShip**: A derived class that implements movement functionality for ships.
  - **ShootingShip**: A derived class that implements shooting functionality for ships.
  - **RamShip**: A derived class that implements the destroy functionality for ships.
  - **SeeingRobot**: A derived class that implements the look functionality for ships.
  - **Battlefield**: A class that manages the battlefield grid, places ships, and displays the grid.
  - **GameManager**: A class that manages the game flow, including adding ships and running the simulation.

## Compilation and Execution
To compile and run the application, follow these steps:

1. Navigate to the `src` directory.
2. Compile the code using a C++ compiler (e.g., g++):

   ```
   g++ main.cpp ship.cpp -o game
   ```

3. Run the compiled application:

   ```
   ./game
   ```

## Additional Information
- Ensure that you have a C++ compiler installed on your system.
- The game simulates a turn-based battle between ships, where each ship can move, shoot, and look around.
- Modify the number of turns in `main.cpp` to change the length of the simulation.


## To start program
- You can run the simple bash script by running `./run` in src