#!/bin/bash
if [ $# = 0 ]; then
	echo "building release config";
	g++ -shared src/*.h src/*.cpp -s -O3 -o pluto.so -std=c++11;
elif [ $1 = "debug" ]; then
	echo "building debug config";
	g++ -shared -DDEBUG src/*.h src/*.cpp -g -o pluto.so -std=c++11;
fi
