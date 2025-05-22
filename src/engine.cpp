//engine.cpp
#include "engine.h"

std::mutex balance_mutex;
double balance = 100.0;
int availableGens = 0;
std::vector<Generator> generators;

// Command System
std::unordered_map<std::string, std::string> originalCommands;
std::unordered_map<std::string, std::string> commandAliases;

std::vector<std::string> linux_names = {
    "systemd", "bash", "cron", "udev", "init", "sshd", "dbus", "Xorg", "pulseaudio", "journald", "grub", "zsh", "tmux", "screen"
};

void initializeCommands() {
    // Original Commands definieren
    originalCommands = {
        {"buy_gen", "yay -S gen"},
        {"show_cost", "yay -Ss"},
        {"upgrade_gen", "yay -U"},
        {"balance", "echo"},
        {"list_gens", "lsblk"},
        {"show_stats", "ls"},
        {"clear_screen", "clear"},
        {"rename", "mv"},
        {"help_menu", "help"}
    };
    
    // Initial alle Commands auf sich selbst verweisen lassen
    for (const auto& pair : originalCommands) {
        commandAliases[pair.second] = pair.first;
    }
}

bool moveCommand(const std::string& oldName, const std::string& newName) {
    // Prüfen ob alter Command existiert
    if (commandAliases.find(oldName) == commandAliases.end()) {
        std::cout << "mv: command '" << oldName << "' not found\n";
        return false;
    }
    
    // Prüfen ob neuer Name bereits existiert
    if (commandAliases.find(newName) != commandAliases.end()) {
        std::cout << "mv: command '" << newName << "' already exists\n";
        return false;
    }
    
    // Original Command Key finden
    std::string originalKey = commandAliases[oldName];
    
    // Neuen Alias erstellen
    commandAliases[newName] = originalKey;
    
    // Alten Alias entfernen
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
    newGen.base_income = 50.0 + (generators.size() * 20.0);
    newGen.speed = 1.0 / (1.0 + generators.size() * 0.2); // Cycles per second
    newGen.name = generateGenName();
    generators.push_back(newGen);
    availableGens++;
}

double calculatePassiveIncome() {
    double total = 0;
    for (auto& gen : generators) {
        total += gen.base_income * gen.income_multiplier * gen.speed;
    }
    return total * 0.1;
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

void upgradeGenerator(int index, const std::string& type) {
    if (index > 0 && index <= (int)generators.size()) {
        std::lock_guard<std::mutex> guard(balance_mutex);
        auto& gen = generators[index-1];
        double cost = 0;

        if (type == "money") {
            cost = gen.base_income * (10 + gen.moneyLevel * 2);
        } else if (type == "speed") {
            cost = gen.base_income * (10 + gen.speedLevel * 2);
        } else {
            std::cout << "Unknown upgrade type!\n";
            return;
        }

        if (balance >= cost) {
            balance -= cost;
            if (type == "money") {
                gen.income_multiplier *= 1.5;
                ++gen.moneyLevel;
                std::cout << gen.name << "'s income upgraded!\n";
            } else if (type == "speed") {
                gen.speed *= 1.3;
                ++gen.speedLevel;
                std::cout << gen.name << "'s speed upgraded!\n";
            }
        } else {
            std::cout << "Not enough money for upgrade! Need $" << (cost - balance) << " more.\n";
        }
    } else {
        std::cout << "Invalid generator index!\n";
    }
}

double getGeneratorCost(int availableGens) {
    return 100.0 * pow(2.5, availableGens);
}

void clearScreen() {
#ifdef _WIN32
    system("cls");
#else
    system("clear");
#endif
}

void help() {
    std::cout << "Commands:\n";
    std::cout << std::left;

    // Commands mit aktuellen Aliasen anzeigen
    std::string buyCmd = "", showCostCmd = "", upgradeCmd = "", balanceCmd = "";
    std::string listCmd = "", statsCmd = "", clearCmd = "", helpCmd = "", renameCmd = "";
    
    // Aktuelle Aliase finden
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

    // Wirtschaftliche Aktionen
    std::cout << "  " << std::setw(30) << buyCmd << "- Buy a new generator\n";
    std::cout << "  " << std::setw(30) << showCostCmd << "- Show cost of next generator\n";
    std::cout << "  " << std::setw(30) << upgradeCmd + " [money|speed] [N]" << "- Upgrade generator N's income or speed\n";

    // Information & Übersicht
    std::cout << "  " << std::setw(30) << balanceCmd << "- Show your current balance\n";
    std::cout << "  " << std::setw(30) << listCmd << "- List all owned generators\n";
    std::cout << "  " << std::setw(30) << statsCmd + " [N]" << "- Show stats for all or generator N\n";

    // System & Bedienung
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

        std::cout << "Generator " << index << " (" << gen.name << "):\n"
                  << "  Money Level: " << gen.moneyLevel << "\n"
                  << "  Income per cycle: $" << (int)income_per_cycle << "\n"
                  << "  Speed Level: " << gen.speedLevel << "\n"
                  << "  Cycle time: " << std::fixed << std::setprecision(2) << cycle_time << " seconds\n"
                  << "  Income per second: $" << (int)income_per_sec << "\n";
    } else {
        std::cout << "Invalid generator number!\n";
    }
}

void settingsMenu(std::string& username) {
    int menuInput{0};

    while (true) {
        std::cout << "Settings:\n";
        std::cout << "[1] Change Username\n";
        std::cout << "[0] Back\n";
        std::cout << "(settings) ";
        std::cin >> menuInput;

        switch(menuInput) {
            case 0: return;
            case 1: {
                std::cout << "New username: ";
                std::cin >> username;
                std::transform(username.begin(), username.end(), username.begin(),
                               [](unsigned char c){ return std::tolower(c); });
                break;
            }
            default:
                std::cout << menuInput << ": Was not found.\n";
                break;
        }
    }
}