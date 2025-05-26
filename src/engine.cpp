//engine.cpp
#include "engine.h"

std::mutex balance_mutex;
double balance = 150.0;
int availableGens = 0;
std::vector<Generator> generators;
bool debugMode = false;

//settings bs
bool compactNumbers = true;
bool colorOutput = false;

// Command System
std::unordered_map<std::string, std::string> originalCommands;
std::unordered_map<std::string, std::string> commandAliases;

std::vector<std::string> linux_names = {
    "systemd", "bash", "cron", "udev", "init", "sshd", "dbus", "Xorg", "pulseaudio", "journald", "grub", "zsh", "tmux", "screen"
};

void initializeCommands() {
    // Database keys type shi
    originalCommands = {
        {"buy_gen", "yay"},
        {"show_cost", "cat"},
        {"upgrade_gen", "systemctl"},
        {"balance", "echo"},
        {"list_gens", "lsblk"},
        {"show_stats", "ls"},
        {"clear_screen", "clear"},
        {"rename", "mv"},
        {"help_menu", "man"},
        {"set_username", "usermod"},
        {"save_game", "umount"},
        {"debug_password", "sudo"},
        {"load_game", "mount"}
    };
    
    // Let the stupid compiler know the Database type shi
    for (const auto& pair : originalCommands) {
        commandAliases[pair.second] = pair.first;
    }
}

std::string formatNumber(double number) {
    if (!compactNumbers) {
        return std::to_string((int)number);
    }
    
    if (number >= 1000000000) {
        return std::to_string((int)(number / 1000000000)) + "." + 
               std::to_string((int)((number / 100000000)) % 10) + "B";
    } else if (number >= 1000000) {
        return std::to_string((int)(number / 1000000)) + "." + 
               std::to_string((int)((number / 100000)) % 10) + "M";
    } else if (number >= 1000) {
        return std::to_string((int)(number / 1000)) + "." + 
               std::to_string((int)((number / 100)) % 10) + "k";
    } else {
        return std::to_string((int)number);
    }
}

std::string getColor(const std::string& type) {
    if (!colorOutput) return "";
    
    if (type == "money") return "\033[32m";      // Green
    if (type == "cost") return "\033[31m";       // Red
    if (type == "generator") return "\033[36m";  // Cyan
    if (type == "level") return "\033[33m";      // Yellow
    if (type == "success") return "\033[92m";    // Bright-green
    if (type == "error") return "\033[91m";      // Bright-red
    if (type == "info") return "\033[94m";       // Bright-blue
    if (type == "reset") return "\033[0m";       // Reset
    
    return "";
}


bool moveCommand(const std::string& oldName, const std::string& newName) {
    if (commandAliases.find(oldName) == commandAliases.end()) {
        std::cout << "mv: command '" << oldName << "' not found\n";
        return false;
    }
    
    if (commandAliases.find(newName) != commandAliases.end()) {
        std::cout << "mv: command '" << newName << "' already exists\n";
        return false;
    }
    
    std::string originalKey = commandAliases[oldName];
    
    commandAliases[newName] = originalKey;
    
    commandAliases.erase(oldName);
    
    std::cout << "Command '" << oldName << "' renamed to '" << newName << "'\n";
    return true;
}

std::string getOriginalCommand(const std::string& alias) {
    if (commandAliases.find(alias) != commandAliases.end()) {
        return commandAliases[alias];
    }
    return "";
}

bool commandExists(const std::string& cmdName) {
    return commandAliases.find(cmdName) != commandAliases.end();
}

std::string generateGenName() {
    if (availableGens < (int)linux_names.size())
        return linux_names[availableGens];
    return "proc-" + std::to_string(availableGens + 1);
}

void buyGenerator() {
    Generator newGen;
    
    if (availableGens == 0) {
        newGen.base_income = 25.0;  // Erster Gen: $25
    } else if (availableGens == 1) {
        newGen.base_income = 45.0;  // Zweiter Gen: $45
    } else if (availableGens == 2) {
        newGen.base_income = 80.0;  // Dritter Gen: $80
    } else { 
        newGen.base_income = 80.0 + (availableGens - 2) * 35.0;
    }
    
    if (availableGens < 3) {
        newGen.speed = 0.8 / (1.0 + availableGens * 0.15);
    } else {
        newGen.speed = 0.8 / (1.0 + availableGens * 0.1);
    }
    
    newGen.name = generateGenName();
    generators.push_back(newGen);
    availableGens++;
}


double calculatePassiveIncome() {
    double total = 0;
    for (auto& gen : generators) {
        total += gen.base_income * gen.income_multiplier * gen.speed;
    }
    return total * 0.08;
}


double getGeneratorIncomePerSecond(const Generator& gen) {
    return gen.base_income * gen.income_multiplier * gen.speed;
}

void updateGameStatus() {
    while (true) {
        std::this_thread::sleep_for(std::chrono::seconds(1));
        std::lock_guard<std::mutex> guard(balance_mutex);
        balance += calculatePassiveIncome();
    }
}

double calculateUpgradeCost(const Generator& gen, const std::string& type) {
    if (type == "money") {
        double baseCost = gen.base_income * 0.6;
        if (gen.moneyLevel <= 4) {
            return baseCost * (1.5 + gen.moneyLevel * 0.4);
        } else if (gen.moneyLevel <= 8) {
            return baseCost * pow(2.2, gen.moneyLevel - 4) * 3;
        } else {
            return baseCost * pow(4.0, gen.moneyLevel - 8) * 20;
        }
    } else if (type == "speed") {
        double baseCost = gen.base_income * 0.7;
        if (gen.speedLevel <= 4) {
            return baseCost * (2.0 + gen.speedLevel * 0.5);
        } else if (gen.speedLevel <= 8) {
            return baseCost * pow(2.5, gen.speedLevel - 4) * 4;
        } else {
            return baseCost * pow(4.5, gen.speedLevel - 8) * 25;
        }
    }
    return 0;
}


void upgradeGeneratorWithFormatting(int index, const std::string& type) {
    if (index > 0 && index <= (int)generators.size()) {
        std::lock_guard<std::mutex> guard(balance_mutex);
        auto& gen = generators[index-1];
        double cost = 0;

        if (type == "money") {
            double baseCost = gen.base_income * 0.6;
            if (gen.moneyLevel <= 4) {
                cost = baseCost * (1.5 + gen.moneyLevel * 0.4);
            } else if (gen.moneyLevel <= 8) {
                cost = baseCost * pow(2.2, gen.moneyLevel - 4) * 3;
            } else {
                cost = baseCost * pow(4.0, gen.moneyLevel - 8) * 20;
            }
        } else if (type == "speed") {
            double baseCost = gen.base_income * 0.7;
            if (gen.speedLevel <= 4) {
                cost = baseCost * (2.0 + gen.speedLevel * 0.5);
            } else if (gen.speedLevel <= 8) {
                cost = baseCost * pow(2.5, gen.speedLevel - 4) * 4;
            } else {
                cost = baseCost * pow(4.5, gen.speedLevel - 8) * 25;
            }
        } else {
            std::cout << getColor("error") << "Unbekannter Upgrade-Typ!" 
                     << getColor("reset") << "\n";
            return;
        }

        if (balance >= cost) {
            balance -= cost;
            if (type == "money") {
                if (gen.moneyLevel <= 6) {
                    gen.income_multiplier *= 1.5;
                } else if (gen.moneyLevel <= 10) {
                    gen.income_multiplier *= 1.3;
                } else {
                    gen.income_multiplier *= 1.15;
                }
                ++gen.moneyLevel;
                
                std::cout << getColor("success") << gen.name << "'s Einkommen verbessert auf Level " 
                         << getColor("level") << gen.moneyLevel << getColor("success")
                         << "! (Nächstes: " << getColor("cost") << "$" << formatNumber(calculateUpgradeCost(gen, "money")) 
                         << getColor("success") << ")" << getColor("reset") << "\n";
            } else if (type == "speed") {
                if (gen.speedLevel <= 6) {
                    gen.speed *= 1.4;
                } else if (gen.speedLevel <= 10) {
                    gen.speed *= 1.25;
                } else {
                    gen.speed *= 1.1;
                }
                ++gen.speedLevel;
                
                std::cout << getColor("success") << gen.name << "'s Geschwindigkeit verbessert auf Level " 
                         << getColor("level") << gen.speedLevel << getColor("success")
                         << "! (Nächstes: " << getColor("cost") << "$" << formatNumber(calculateUpgradeCost(gen, "speed")) 
                         << getColor("success") << ")" << getColor("reset") << "\n";
            }
        } else {
            std::cout << getColor("error") << "Nicht genug Geld für Upgrade! Benötige " 
                     << getColor("cost") << "$" << formatNumber(cost - balance) 
                     << getColor("error") << " mehr." << getColor("reset") << "\n";
        }
    } else {
        std::cout << getColor("error") << "Ungültiger Generator-Index!" 
                 << getColor("reset") << "\n";
    }
}


double getGeneratorCost(int availableGens) {
    if (availableGens == 0) {
        return 150.0;  // Erster Generator etwas teurer
    } else if (availableGens == 1) {
        return 500.0;  // Zweiter Generator deutlich teurer
    } else if (availableGens == 2) {
        return 1500.0; // Dritter Generator sehr viel teurer
    } else {
        // Ab dem 4. Generator exponentielles Wachstum
        return 1500.0 * pow(4.0, availableGens - 2);
    }
}


void clearScreen() {
#ifdef _WIN32
    system("cls");
    system("fastfetch --config-path C:\\Users\\Addiv\\Documents\\Addivs-Dotfiles\\TS\\.config\\fastfetch\\configs\\quarter.jsonc");
#else
    system("clear");
    system("~/.config/fastfetch/smarter-fastfetch");
#endif
}

std::string getSavePath() {
    std::string fullPath;

#ifdef _WIN32
    const char* appdata = getenv("APPDATA");
    if (appdata)
        fullPath = std::string(appdata) + "\\BashIdler\\savegame.txt";
    else
        fullPath = "savegame.txt"; // Fallback
#else
    const char* home = getenv("HOME");
    if (home)
        fullPath = std::string(home) + "/.local/share/bashidler/savegame.txt";
    else
        fullPath = "savegame.txt"; // Fallback
#endif

    // Versuche den Zielordner zu erstellen (macht nichts, wenn er schon da ist)
    std::filesystem::path parentDir = std::filesystem::path(fullPath).parent_path();
    std::error_code ec;
    std::filesystem::create_directories(parentDir, ec);
    if (ec) {
        // Optional: Fehler loggen
        std::cerr << "Error while saving the game: " << ec.message() << std::endl;
    }

    return fullPath;
}

void saveGame() {
    std::string path = getSavePath();
    std::ofstream file(path);
    if (file.is_open()) {
        // Speichere die aktuellen Command-Aliases, nicht die Original-Commands
        file << "aliases:\n";
        for (const auto& [alias, originalCmd] : commandAliases) {
            file << alias << "=" << originalCmd << "\n";
        }
        
        // Optional: Auch andere Spieldaten speichern
        file << "gamedata:\n";
        file << "balance=" << balance << "\n";
        file << "availableGens=" << availableGens << "\n";

        
        file.close();
        std::cout << "Spiel gespeichert unter: " << path << std::endl;
    } else {
        std::cerr << "Konnte Savefile nicht öffnen!" << std::endl;
    }
}

void loadGame() {
    std::string path = getSavePath();
    std::ifstream file(path);
    if (file.is_open()) {
        std::string line;
        commandAliases.clear(); // Alte Aliases löschen
        
        bool inAliasSection = false;
        bool inGameDataSection = false;

        while (std::getline(file, line)) {
            if (line == "aliases:") {
                inAliasSection = true;
                inGameDataSection = false;
                continue;
            }
            
            if (line == "gamedata:") {
                inAliasSection = false;
                inGameDataSection = true;
                continue;
            }

            if (inAliasSection && !line.empty()) {
                size_t pos = line.find('=');
                if (pos != std::string::npos) {
                    std::string alias = line.substr(0, pos);
                    std::string originalCmd = line.substr(pos + 1);
                    commandAliases[alias] = originalCmd;
                }
            }
            
            if (inGameDataSection && !line.empty()) {
                size_t pos = line.find('=');
                if (pos != std::string::npos) {
                    std::string key = line.substr(0, pos);
                    std::string value = line.substr(pos + 1);
                    
                    if (key == "balance") {
                        balance = std::stod(value);
                    } else if (key == "availableGens") {
                        availableGens = std::stoi(value);
                    }
                }
            }
        }
        
        file.close();
        std::cout << "Spiel geladen von: " << path << std::endl;
        
        // Debug: Zeige geladene Aliases
        if (debugMode) {
            std::cout << "Geladene Aliases:\n";
            for (const auto& [alias, cmd] : commandAliases) {
                std::cout << "  " << alias << " -> " << cmd << "\n";
            }
        }
    } else {
        std::cout << "Kein Savefile gefunden. Neue Sitzung gestartet." << std::endl;
        // Stelle sicher, dass die Standard-Aliases geladen sind
        initializeCommands();
    }
}


void showBalance() {
    std::lock_guard<std::mutex> guard(balance_mutex);
    std::cout << "Balance: " << getColor("money") << "$" << formatNumber(balance) << getColor("reset") << "\n";
}

// Alternative Lösung: Beide Maps speichern (falls du die Original-Commands auch behalten willst)
void saveGameAlternative() {
    std::string path = getSavePath();
    std::ofstream file(path);
    if (file.is_open()) {
        // Speichere Original Commands
        file << "original_commands:\n";
        for (const auto& [key, value] : originalCommands) {
            file << key << "=" << value << "\n";
        }
        
        // Speichere aktuelle Aliases
        file << "current_aliases:\n";
        for (const auto& [alias, originalCmd] : commandAliases) {
            file << alias << "=" << originalCmd << "\n";
        }
        
        file.close();
        std::cout << "Spiel gespeichert unter: " << path << std::endl;
    } else {
        std::cerr << "Konnte Savefile nicht öffnen!" << std::endl;
    }
}

void loadGameAlternative() {
    std::string path = getSavePath();
    std::ifstream file(path);
    if (file.is_open()) {
        std::string line;
        originalCommands.clear();
        commandAliases.clear();
        
        bool inOriginalSection = false;
        bool inAliasSection = false;

        while (std::getline(file, line)) {
            if (line == "original_commands:") {
                inOriginalSection = true;
                inAliasSection = false;
                continue;
            }
            
            if (line == "current_aliases:") {
                inOriginalSection = false;
                inAliasSection = true;
                continue;
            }

            if ((inOriginalSection || inAliasSection) && !line.empty()) {
                size_t pos = line.find('=');
                if (pos != std::string::npos) {
                    std::string key = line.substr(0, pos);
                    std::string value = line.substr(pos + 1);
                    
                    if (inOriginalSection) {
                        originalCommands[key] = value;
                    } else if (inAliasSection) {
                        commandAliases[key] = value;
                    }
                }
            }
        }
        
        file.close();
        std::cout << "Spiel geladen von: " << path << std::endl;
    } else {
        std::cout << "Kein Savefile gefunden. Neue Sitzung gestartet." << std::endl;
        initializeCommands();
    }
}


void showGeneratorStats(int index) {
    if (index == -1) {
        // Alle Generatoren anzeigen
        genList();
        return;
    }
    
    if (index > 0 && index <= (int)generators.size()) {
        const auto& gen = generators[index - 1];
        double cycle_time = 1.0 / gen.speed;
        double income_per_cycle = gen.base_income * gen.income_multiplier;
        double income_per_sec = getGeneratorIncomePerSecond(gen);
        
        double money_upgrade_cost = calculateUpgradeCost(gen, "money");
        double speed_upgrade_cost = calculateUpgradeCost(gen, "speed");

        std::cout << "Generator " << index << " (" << gen.name << "):\n"
                  << "  Money Level: " << gen.moneyLevel << " (Upgrade: $" << (int)money_upgrade_cost << ")\n"
                  << "  Income per cycle: $" << (int)income_per_cycle << "\n"
                  << "  Speed Level: " << gen.speedLevel << " (Upgrade: $" << (int)speed_upgrade_cost << ")\n"
                  << "  Cycle time: " << std::fixed << std::setprecision(2) << cycle_time << " seconds\n"
                  << "  Income per second: $" << (int)income_per_sec << "\n\n"
                  << "  Efficiency compared to new generator:\n";
        
        double new_gen_cost = getGeneratorCost(availableGens);
        double efficiency_ratio = income_per_sec / new_gen_cost * 100;
        
        if (efficiency_ratio > 15) {
            std::cout << "  This generator is still very efficient! Consider upgrading.\n";
        } else if (efficiency_ratio > 5) {
            std::cout << "  This generator is decent. Upgrades or new generator both viable.\n";
        } else {
            std::cout << "  Consider buying a new generator instead of upgrading.\n";
        }
        
    } else {
        std::cout << "Invalid generator number!\n";
    }
}

void handleBuyGenerator() {
    std::lock_guard<std::mutex> guard(balance_mutex);
    double cost = getGeneratorCost(availableGens);
    if (balance >= cost) {
        balance -= cost;
        buyGenerator();
        std::cout << getColor("success") << "Purchased " << getColor("generator") 
                  << generators.back().name << getColor("success") << " for " 
                  << getColor("cost") << "$" << formatNumber(cost) << getColor("reset") << "\n";
    } else {
        std::cout << getColor("error") << "Need " << getColor("cost") << "$" 
                  << formatNumber(cost - balance) << getColor("error") << " more!" << getColor("reset") << "\n";
    }
}

void settingsMenu(std::string& username) {
    int menuInput = 0;
    
    while (true) {
        std::cout << getColor("info") << "Settings:" << getColor("reset") << "\n";
        std::cout << "[1] Compact Numbers (" << getColor(compactNumbers ? "success" : "error") 
                  << (compactNumbers ? "ON" : "OFF") << getColor("reset") << ") - Shows 14.2k instead of 14263\n";
        std::cout << "[2] Colored Output (" << getColor(colorOutput ? "success" : "error") 
                  << (colorOutput ? "ON" : "OFF") << getColor("reset") << ") - Adds colors to output\n";
        std::cout << "[0] Back\n";
        std::cout << getColor("info") << "(settings) " << getColor("reset");
        std::cin >> menuInput;

        switch(menuInput) {
            case 0: 
                std::cin.ignore();
                return;
            case 1: {
                compactNumbers = !compactNumbers;
                std::cout << getColor("success") << "Compact numbers " 
                         << (compactNumbers ? "enabled" : "disabled") << getColor("reset") << "\n";
                break;
            }
            case 2: {
                colorOutput = !colorOutput;
                std::cout << getColor("success") << "Colored output " 
                         << (colorOutput ? "enabled" : "disabled") << getColor("reset") << "\n";
                break;
            }
            default:
                std::cout << getColor("error") << menuInput << ": Was not found." << getColor("reset") << "\n";
                break;
        }
    }
}

void help() {
    std::cout << "Commands:\n";
    std::cout << std::left;

    std::string buyCmd = "", showCostCmd = "", upgradeCmd = "", balanceCmd = "";
    std::string listCmd = "", statsCmd = "", clearCmd = "", helpCmd = "", renameCmd = "";
    
    for (const auto& pair : commandAliases) {
        if (pair.second == "buy_gen") buyCmd = pair.first;
        else if (pair.second == "show_cost") showCostCmd = pair.first;
        else if (pair.second == "upgrade_gen") upgradeCmd = pair.first;
        else if (pair.second == "balance") balanceCmd = pair.first;
        else if (pair.second == "list_gens") listCmd = pair.first;
        else if (pair.second == "show_stats") statsCmd = pair.first;
        else if (pair.second == "clear_screen") clearCmd = pair.first;
        else if (pair.second == "help_menu") helpCmd = pair.first;
        else if (pair.second == "rename") renameCmd = pair.first;
    }

    std::cout << "  " << std::setw(30) << buyCmd << "- Buy a new generator\n";
    std::cout << "  " << std::setw(30) << showCostCmd << "- Show cost of next generator\n";
    std::cout << "  " << std::setw(30) << upgradeCmd + " [money|speed] [N]" << "- Upgrade generator N's income or speed\n";

    std::cout << "  " << std::setw(30) << balanceCmd << "- Show your current balance\n";
    std::cout << "  " << std::setw(30) << listCmd << "- List all owned generators\n";
    std::cout << "  " << std::setw(30) << statsCmd + " [N]" << "- Show stats for all or generator N\n";

    std::cout << "  " << std::setw(30) << clearCmd << "- Clear the screen\n";
    std::cout << "  " << std::setw(30) << helpCmd << "- Show this help message\n";
    std::cout << "  " << std::setw(30) << renameCmd + " [old] [new]" << "- Rename an existing command\n";
    std::cout << "  " << std::setw(30) << "settings" << "- Open settings menu\n";
}

void genList() {
    if (generators.empty()) {
        std::cout << "No generators owned.\n";
        return;
    }
    std::cout << std::left
              << std::setw(3)  << "ID"   << " | "
              << std::setw(12) << "Name" << " | "
              << std::setw(5)  << "M-Lv" << " | "
              << std::setw(13) << "Income/cycle" << " | "
              << std::setw(5)  << "S-Lv" << " | "
              << std::setw(15) << "Cycle time (s)" << " | "
              << std::setw(11) << "Income/sec" << "\n";
    std::cout << std::string(84, '-') << "\n";

    int i = 1;
    for (const auto& gen : generators) {
        double cycle_time = 1.0 / gen.speed;
        double income_per_cycle = gen.base_income * gen.income_multiplier;
        double income_per_sec = getGeneratorIncomePerSecond(gen);

        std::cout << std::right
                  << std::setw(3) << i << " | "
                  << std::left
                  << std::setw(12) << gen.name << " | "
                  << std::setw(5)  << gen.moneyLevel << " | "
                  << "$" << std::setw(12) << std::right << (int)income_per_cycle << " | "
                  << std::setw(5)  << gen.speedLevel << " | "
                  << std::setw(15) << std::fixed << std::setprecision(2) << cycle_time << " | "
                  << "$" << std::setw(10) << std::right << (int)income_per_sec << "\n";
        ++i;
    }
}

void debug() {
    if (debugMode == true) {
    std::cout << "Passed";
    }
}