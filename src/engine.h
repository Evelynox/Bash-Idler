//engine.h
#ifndef GLOABLS_H
#define GLOABLS_H

#include <cstdlib>  
#include <thread>
#include <chrono>
#include <vector>

extern double getGeneratorCost(int availableGens);
extern double balance;
extern int availableGens;

void clearScreen();
void collectResources();
void updateGameStatus();
void genList();
void help();

//struct Generator {
    //double income;    // Geld pro Sekunde
    //double speed;     // Produktionsgeschwindigkeit
    //int level;        // Upgrade-Level
    //double efficiency;// Effizienz-Faktor
//};

//extern vector<Generator> generators;

#endif