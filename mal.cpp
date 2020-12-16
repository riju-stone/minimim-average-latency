#include <iostream>
#include <vector>
#include <algorithm>
#include <unordered_map>
#include <fstream>
#include <map>
#include <bitset>

//User-defined header files
#include "cycles.h"

int main(){
    Cycles obj;

    //Creating reservation table, forbidden latency and collision vector...
    obj.readTable();
    obj.forbiddenLat();
    obj.collisionVect();

    //Generating the state diagram...
    obj.buildSD();

    //Calculating the simple, greedy cycles and minimum average latency...
    obj.calcMAL();

    system("pause");
    return 0;
}