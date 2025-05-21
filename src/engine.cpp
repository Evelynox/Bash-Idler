#include "engine.h"
#include <iostream>
#include <chrono>
#include <cmath>
#include <cstdlib>
#include <ctime>
#include <thread>
#include <vector>

std::mutex balance_mutex;
double balance = 100.0;
int availableGens = 0;
std::vector<Generator> generators;

std::vector<std::string> linux_names = {
    "systemd", "bash", "cron", "udev", "init", "sshd", "dbus", "Xorg", "pulseaudio", "journald", "grub", "zsh", "tmux", "screen"
};

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
    std::cout <<
        "Commands:\n"
        "  yay -S gen               - Buy a new generator\n"
        "  yay -Ss                  - Show cost of next generator\n"
        "  yay -U [money|speed] [N] - Upgrade generator N's income or speed\n"
        "  echo                     - Show your current balance\n"
        "  lsblk                    - List all owned generators\n"
        "  ls [N]                   - Show stats for all or generator N\n"
        "  clear                    - Clear the screen\n"
        "  help                     - Show this help message\n";
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



