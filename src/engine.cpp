// engine.cpp
#include <iostream>
#include <chrono>
#include <mutex>
#include "engine.h"

using namespace std;

extern mutex balance_mutex;

void updateGameStatus() {
    while (true) {
        this_thread::sleep_for(chrono::seconds(1));
        lock_guard<mutex> guard(balance_mutex);
        balance += 10 * availableGens; // Jede Sekunde pro Generator
    }
}