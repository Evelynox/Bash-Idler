#include <iostream>
#include <thread>
#include <sstream>
#include <vector>
#include "engine.h"

std::vector<std::string> split(const std::string& s) {
    std::istringstream iss(s);
    std::vector<std::string> tokens;
    std::string token;
    while (iss >> token) tokens.push_back(token);
    return tokens;
}

int main() {
    srand(time(0));

    std::thread gameThread(updateGameStatus);
    gameThread.detach();

    clearScreen();
    std::cout << "Type 'help' for commands\n";

    while (true) {
        std::cout << "[bash@idler ~]$ ";
        std::string input;
        std::getline(std::cin, input);

        auto tokens = split(input);

        if (input == "yay -S gen") {
            std::lock_guard<std::mutex> guard(balance_mutex);
            double cost = getGeneratorCost(availableGens);
            if (balance >= cost) {
                balance -= cost;
                buyGenerator();
                std::cout << "Purchased " << generators.back().name
                          << " for $" << cost << "\n";
            } else {
                std::cout << "Need $" << cost - balance << " more!\n";
            }
        }
        else if (input == "help") {
            help();
        }
        else if (input == "yay -Ss") {
            std::cout << "Next generator costs: $"
                      << getGeneratorCost(availableGens) << "\n";
        }
        else if (input == "echo $BALANCE") {
            std::lock_guard<std::mutex> guard(balance_mutex);
            std::cout << "Balance: $" << balance << "\n";
        }
        else if (input == "ls") {
            genList();
        }
        else if (!tokens.empty() && tokens[0] == "stats") {
            if (tokens.size() == 1) showGeneratorStats();
            else try {
                int num = std::stoi(tokens[1]);
                showGeneratorStats(num);
            } catch (...) {
                std::cout << "Usage: stats [N]\n";
            }
        }
        else if (tokens.size() == 4 && tokens[0] == "yay" && tokens[1] == "-U") {
            std::string type = tokens[2];
            try {
                int num = std::stoi(tokens[3]);
                if (type == "money" || type == "speed")
                    upgradeGenerator(num, type);
                else
                    std::cout << "Usage: yay -U [money|speed] N\n";
            } catch (...) {
                std::cout << "Usage: yay -U [money|speed] N\n";
            }
        }
        else if (input == "clear") {
            clearScreen();
        }
        else {
            std::cout << input << ": command not found\n";
        }
    }
    return 0;
}
