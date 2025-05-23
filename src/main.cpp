//main.cpp
#include <iostream>
#include <thread>
#include <sstream>
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
    
    // Command System initialisieren
    initializeCommands();

    std::thread gameThread(updateGameStatus);
    gameThread.detach();
    clearScreen();
    std::cout << "Type 'man' for commands\n";

    std::string username = "bash";

    while (true) {
        std::cout << "[" << username << "@idler ~]$ ";
        std::string input;
        std::getline(std::cin, input);

        auto tokens = split(input);
        if (tokens.empty()) continue;

        std::string command = tokens[0];
        std::string originalCmd = getOriginalCommand(command);

        // mv Command special handling
        if (originalCmd == "rename" && tokens.size() >= 3) {
            std::string oldCmd = tokens[1];
            std::string newCmd = tokens[2];
            
            // Anführungszeichen entfernen falls vorhanden
            if (oldCmd.front() == '"' && oldCmd.back() == '"') 
                oldCmd = oldCmd.substr(1, oldCmd.size()-2);
            if (newCmd.front() == '"' && newCmd.back() == '"') 
                newCmd = newCmd.substr(1, newCmd.size()-2);
            
            // Mehrere Wörter zu einem Command zusammenfügen
            for (size_t i = 3; i < tokens.size(); ++i) {
                newCmd += " ";
                std::string part = tokens[i];
                if (part.front() == '"' && part.back() == '"') 
                    part = part.substr(1, part.size()-2);
                newCmd += part;
            }
            
            moveCommand(oldCmd, newCmd);
            continue;
        }

        // Normale Commands verarbeiten
        if (originalCmd == "buy_gen") {
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
        else if (originalCmd == "help_menu") {
            help();
        }
        else if (originalCmd == "show_cost") {
            std::cout << "Next generator costs: $"
                      << getGeneratorCost(availableGens) << "\n";
        }
        else if (originalCmd == "balance") {
            std::lock_guard<std::mutex> guard(balance_mutex);
            std::cout << "Balance: $" << balance << "\n";
        }
        else if (originalCmd == "list_gens") {
            genList();
        }
        else if (originalCmd == "show_stats") {
            if (tokens.size() == 1) {
                showGeneratorStats(-1); // Alle anzeigen
            } else {
                try {
                    int num = std::stoi(tokens[1]);
                    showGeneratorStats(num);
                } catch (...) {
                    std::cout << "Usage: " << command << " [N]\n";
                }
            }
        }
        else if (originalCmd == "upgrade_gen" && tokens.size() == 3) {
            std::string type = tokens[1];
            try {
                int num = std::stoi(tokens[2]);
                if (type == "money" || type == "speed") {
                    upgradeGenerator(num, type);
                } else {
                    std::cout << "Usage: " << command << " [money|speed] N\n";
                }
            } catch (...) {
                std::cout << "Usage: " << command << " [money|speed] N\n";
            }
        }
        else if (originalCmd == "clear_screen") {
            clearScreen();
        }
        else if (input == "settings") {
            std::cin.ignore(); // Buffer leeren für getline nach cin
            settingsMenu(username);
        }
        else if ( tokens[0] == "usermod" != tokens.size() > 2) {
            username = tokens[1];
        }
        else if (originalCmd == "save_game") {
            saveGame();
        }
        else {
            std::cout << input << ": command not found\n";
        }
    }
    return 0;
}