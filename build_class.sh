#!/bin/sh 
echo Running cmake
cmake .
echo Running make
make
echo Running server/server
./server
