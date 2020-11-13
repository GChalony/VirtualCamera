#!bin/bash
g++ -ggdb Transforms.cpp main.cpp `pkg-config --cflags --libs opencv4`