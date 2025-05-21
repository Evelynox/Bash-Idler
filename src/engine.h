#ifndef ENGINE_H
#define ENGINE_H

#include <iostream>
#include <vector>
#include <mutex>
#include <string>
#include <iomanip>
#include <algorithm>
#include <cctype> 
#include <map>
#include <thread>
#include <ctime>
#include <cstdlib>
#include <cmath>
#include <chrono>

extern std::mutex balance_mutex;
extern double balance;
extern int availableGens;
extern bool simpleModeOn;
extern std::string username;

struct Generator {
    double base_income;
    double income_multiplier = 1.0;
    double speed = 1.0;
    std::string name;
    int moneyLevel{1};
    int speedLevel{1};
};

extern std::vector<Generator> generators;

// Function declarations
void clearScreen();
void updateGameStatus();
void genList();
void help();
void showGeneratorStats(int index = -1);
void upgradeGenerator(int index, const std::string& type);
double getGeneratorCost(int availableGens);
void buyGenerator();
std::string generateGenName();
double calculatePassiveIncome();
double getGeneratorIncomePerSecond(const Generator& gen);
void simpleMode();
void settingsMenu(std::string& username);
std::map<std::string, std::string> commands

#endif
