#!/bin/bash
rm ./BashIdler
g++ -std=c++11 main.cpp engine.cpp -o BashIdler -pthread
./BashIdler