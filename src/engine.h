//engine.h
#ifndef ENGINE_H
#define ENGINE_H

#include <cstdio>
#include <readline/readline.h>
#include <readline/history.h>
#include <codecvt>
#include <iostream>
#include <vector>
#include <mutex>
#include <string>
#include <iomanip>
#include <algorithm>
#include <limits>
#include <locale>
#include <cctype> 
#include <map>
#include <unordered_map>
#include <thread>
#include <ctime>
#include <cstdlib>
#include <cmath>
#include <chrono>
#include <regex>
#include <fstream>
#include <filesystem>
#include <sstream>
#include "json.hpp"

extern std::mutex balance_mutex;
extern double balance;
extern int availableGens;
extern bool simpleModeOn;
extern std::string username;
extern std::string hostname;
extern bool debugMode;
extern bool compactNumbers;  // Standard: true
extern bool colorOutput;     // Standard: false
extern double getGeneratorCost(int availableGens);

struct Generator {
    double base_income;
    double income_multiplier = 1.0;
    double speed = 1.0;
    std::string name;
    int moneyLevel{1};
    int speedLevel{1};
};

extern std::vector<Generator> generators;

// Command System
extern std::unordered_map<std::string, std::string> originalCommands;
extern std::unordered_map<std::string, std::string> commandAliases;

// Function declarations
void clearScreen();
void updateGameStatus();
void genList();
void help();
void showGeneratorStats(int index = -1);
void upgradeGeneratorWithFormatting(int index, const std::string& type);
void buyGenerator();
std::string generateGenName();
double calculatePassiveIncome();
double getGeneratorIncomePerSecond(const Generator& gen);
void settings();
std::string getSavePath();
std::string formatNumber(double number);
void saveGame();
void loadGame();
void showBalance();
void debug();

// Command System Functions
void initializeCommands();
bool moveCommand(const std::string& oldName, const std::string& newName);
std::string getOriginalCommand(const std::string& alias);
bool commandExists(const std::string& cmdName);

#endif
