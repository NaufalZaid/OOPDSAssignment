#include "shipTypes.cpp"
#include <fstream>
#include <iostream>
#include <sstream>

// Constants for maximum sizes
const int MAX_TEAMS = 2;
const int MAX_SHIPS_PER_TEAM = 10;
const int MAX_SHIPS_TOTAL = 50;

struct ShipConfig {
  string type;
  char symbol;
  int count;
};

struct TeamConfig {
  string name;
  int shipTypeCount;
  ShipConfig ships[MAX_SHIPS_PER_TEAM];
};

struct GameConfig {
  int iterations;
  int width;
  int height;
  int battlefield[HEIGHT][WIDTH];
  int teamCount;
  TeamConfig teams[MAX_TEAMS];
};

class GameParser {
private:
  GameConfig config;

  bool parseHeader(ifstream &file) {
    string line;
    // Parse iterations
    getline(file, line);
    istringstream iss(line);
    string word;
    iss >> word >> config.iterations;

    // Parse width and height
    getline(file, line);
    iss.clear();
    iss.str(line);
    iss >> word >> config.width;

    getline(file, line);
    iss.clear();
    iss.str(line);
    iss >> word >> config.height;

    return true;
  }

  bool parseTeams(ifstream &file) {
    string line;
    config.teamCount = 0;

    while (getline(file, line)) {
      if (line.empty())
        continue;

      istringstream iss(line);
      string word;
      iss >> word;

      if (word == "Team") {
        if (config.teamCount >= MAX_TEAMS) {
          throw runtime_error("Too many teams");
        }

        TeamConfig &team = config.teams[config.teamCount];
        iss >> team.name >> team.shipTypeCount;

        // Parse ships for this team
        for (int i = 0; i < team.shipTypeCount; i++) {
          getline(file, line);
          istringstream shipIss(line);
          shipIss >> team.ships[i].type >> team.ships[i].symbol >>
              team.ships[i].count;
        }

        config.teamCount++;
      } else if (isdigit(word[0]) || word[0] == '0') {
        // Start of battlefield grid
        int row = 0;
        do {
          istringstream gridIss(line);
          int col = 0;
          string value;
          while (gridIss >> value && col < config.width) {
            config.battlefield[row][col] = stoi(value);
            col++;
          }
          row++;
        } while (row < config.height && getline(file, line));
      }
    }
    return true;
  }

public:
  GameConfig parseFile(const string &filename) {
    ifstream file(filename);
    if (!file.is_open()) {
      throw runtime_error("Cannot open file: " + filename);
    }

    parseHeader(file);
    parseTeams(file);

    return config;
  }

  Ship *createShip(const string &type, char symbol, const string &teamName) {
    Ship *ship = nullptr;

    if (type == "Battleship")
      ship = new Battleship(symbol);
    else if (type == "Cruiser")
      ship = new Cruiser(symbol);
    else if (type == "Destroyer")
      ship = new Destroyer(symbol);
    else if (type == "Frigate")
      ship = new Frigate(symbol);
    else
      throw runtime_error("Unknown ship type: " + type);

    ship->setTeam(teamName);
    return ship;
  }
};