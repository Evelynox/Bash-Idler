#ifndef ENGINE_H
#define ENGINE_H

#include <vector>
#include <mutex>
#include <string>
#include <iomanip>

extern std::mutex balance_mutex;
extern double balance;
extern int availableGens;
extern bool simpleModeOn;

struct Generator {
    double base_income;
    double income_multiplier = 1.0;
    double speed = 1.0; // Cycles per second
    int level = 1;
    std::string name;
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

#endif
