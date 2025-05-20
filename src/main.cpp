// main.cpp
#include <iostream>
#include <thread>
#include <mutex>
#include "engine.h"

std::mutex balance_mutex;
double balance{10};
int availableGens{0};

using namespace std;

int main() {
    thread t1(updateGameStatus);
    t1.detach(); // Thread im Hintergrund laufen lassen
    system("clear");
    string input;
    while (true) {
        cout << "[bash@idler ~]$ ";
        getline(cin, input); // Ganze Zeile lesen

        if (input == "yay -S gen") {
            lock_guard<mutex> guard(balance_mutex);
            if (balance >= 10) {
                balance -= 10;
                availableGens++;
                cout << "You now have " << availableGens << " Gens.\n";
            } else {
                cout << "Not enough balance!\n";
            }
        }
        else if (input == "$BALANCE") {
            lock_guard<mutex> guard(balance_mutex);
            cout << "You have " << balance << "$\n";
        }
    }
    return 0;
}