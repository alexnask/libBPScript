#!/bin/bash
g++ -c Parser.cpp Bullet.cpp BulletManager.cpp Exception.cpp Value.cpp -std=c++0x
ar rvs libBPScript.a Parser.o Bullet.o BulletManager.o Exception.o Value.o
