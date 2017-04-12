#!/bin/sh
rm bugBot.o
g++ bugBot.cpp -o bugBot.o
echo 'Compiled!'
echo 'Running'
./bugBot.o
