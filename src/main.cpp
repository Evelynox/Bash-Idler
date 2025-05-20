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
    t1.detach();
    clearScreen();
    string input;
    while (true) {
        cout << "[bash@idler ~]$ ";
        getline(cin, input);

        if (input == "yay -S gen") {
            lock_guard<mutex> guard(balance_mutex);
            double cost = getGeneratorCost(availableGens);
            if (balance >= cost) {
                balance -= cost;
                availableGens++;
                cout << "Generator gekauft! (Kosten: " << cost << "$)\n";
                cout << "Du hast jetzt " << availableGens << " Generatoren.\n";
            } else {
                cout << "Not enough Money! You Need: " << cost << "$\n";
            }
        }
        else if (input == "help") {
            help();
        }
        else if (input == "yay -Ss") {
            double cost = getGeneratorCost(availableGens);
            cout << cost << "$" << endl;
        }
        else if (input == "echo $BALANCE") {
            lock_guard<mutex> guard(balance_mutex);
            cout << "You have " << balance << "$\n";
        }
        else if (input == "ls") {
            genList();
        } else {
            cout << input << ": command not found" << endl;
        }
    }
    return 0;
}