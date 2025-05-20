// engine.cpp
#include <iostream>
#include <chrono>
#include <mutex>
#include <cmath>
#include "engine.h"

using namespace std;

extern mutex balance_mutex;

void updateGameStatus() {
    while (true) {
        this_thread::sleep_for(chrono::seconds(1));
        lock_guard<mutex> guard(balance_mutex);
        balance += 1 * availableGens; // Jede Sekunde pro Generator
    }
}

void genList() {
    for (int i = 0; i < availableGens; ++i) {
        cout << "Generator " << i + 1 << endl;
    }
}

double getGeneratorCost(int availableGens) {
    const double baseCost = 10;
    const double multiplier = 1.15;
    return baseCost * pow(multiplier, availableGens); 
}

void help() {
    cout << "\033[1m yay -S gen \033[0m\t\tPurchases a generator if you have enough money. Increases your generator count." << endl;
    cout << "\033[1m yay -Ss \033[0m\t\tShows the current cost of the next generator." << endl;
    cout << "\033[1m echo $BALANCE \033[0m\t\tDisplays your current balance in dollars." << endl;
    cout << "\033[1m ls \033[0m\t\t\tLists all available generators and their stats." << endl;
    cout << "\033[1m (any other) \033[0m\t\tUnrecognized commands will return 'command not found' like a real shell." << endl;
}

void clearScreen() {
#ifdef _WIN32
    system("cls");
#else
    system("clear");
#endif
}