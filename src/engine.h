//engine.h
#ifndef GLOABLS_H
#define GLOABLS_H

#include <thread>
#include <chrono>

extern double balance;
extern int availableGens;

void collectResources();

void updateGameStatus();

#endif