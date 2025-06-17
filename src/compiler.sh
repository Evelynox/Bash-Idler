#!/bin/bash
rm -f ./BashIdler 
g++ -std=c++17 main.cpp engine.cpp -o BashIdler -pthread -lreadline && ./BashIdler
