#!/bin/bash

x86_64-w64-mingw32-g++ -static -static-libgcc -static-libstdc++ -pthread \
  -I/usr/include/nlohmann \
  main.cpp engine.cpp -o BashIdler.exe
