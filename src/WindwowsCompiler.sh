#!/bin/bash

x86_64-w64-mingw32-g++ -static -static-libgcc -static-libstdc++ -pthread main.cpp engine.cpp -o BashIdler.exe