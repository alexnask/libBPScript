#!/bin/bash
g++ main.cpp -std=c++0x -L../src -I../src -lsfml-graphics -lsfml-window -lsfml-system -lBPScript -g -rdynamic -o test.out
./test.out
