#include <iostream>
#include <vector>
#include <algorithm>
#include <unordered_map>
#include <fstream>
#include <map>
#include <bitset>

using namespace std;

class ReservationTable{
    protected:
        int stages, time_interval;
        // Key : Cycle Number, Value : Execution Value (1 or 0) A multimap can have different values for the same keys           
        multimap<int, int> table;
        vector<int> forbidden; 
        vector<bool> collision;
    public:
        //Create the reservation table
        void readTable();
        //Calculate the forbidden latencies
        void forbiddenLat();
        //Calculate the collision vector
        void collisionVect();
};

void ReservationTable :: readTable(){
    
    //Improved File Input System
    ifstream fin;
    fin.open("ReservationTable.txt", ifstream::in);
    if(!fin){
        std::cout<<"Cannot open file..."<<endl;
        exit(1);
    }

    string str;
    getline(fin, str);

    /*
        stoi("31337 geek") is 31337. The stoi() function takes a string as an argument and 
        returns its value, excluding the characters...
    */

    stages = stoi(str);

    getline(fin, str);
    time_interval = stoi(str);

    // Improved Output System
    int i = 0, j;
    string::iterator it;

    std::cout<<"Reservation Table :"<<endl;
    for(j = 1; j <= time_interval; j++)
        std::cout<<"\t"<<"t"<<j;

    std::cout<<endl;

    while(!fin.eof()){
        getline(fin, str);
        i++;
        j = 0;
        std::cout<<"S"<<i<<"\t";
        for(it = str.begin(); it != str.end(); it++){
            if(*it == '1'){
                std::cout<<"X"<<"\t";
                j++;
                table.insert(pair<int, int>(i, j));
            }
            else if(*it == '0'){
                std::cout<<"0"<<"\t";
                j++;
            }
        }
        std::cout<<endl;
    }

    std::cout<<endl;

    fin.close();
}

void ReservationTable :: forbiddenLat(){

    vector<int> row;
    multimap<int, int>::iterator t;
    vector<int>::iterator f;

    int lat;

    for(int i = 1; i <= stages; i++){
        for(t = table.equal_range(i).first; t != table.equal_range(i).second; t++)
            row.push_back((*t).second);
            for(int p = 0; p < row.size(); p++) 
                for(int q = 0; q < row.size(); q++){
                    lat = row[q] - row[p];              // Subtracting all permutations of the time cycles to get various possible latencies
                    f = find(forbidden.begin(), forbidden.end(), lat);
                    if(f == forbidden.end() && lat > 0)     // No need of pushing if latency is 0
                        forbidden.push_back(lat);
            }
            row.clear();
    }
    sort(forbidden.begin(), forbidden.end());

    std::cout<<"\nForbidden latencies: ";
	for (int i = 0; i < forbidden.size(); i++)
        std::cout<<" "<<forbidden[i];
	std::cout<<endl;
}

//This function calculates the collision vector
void ReservationTable :: collisionVect(){
    int bit_pos;

    collision.reserve(time_interval);

    //Initializing vector with 0s
    for(int i = 0; i < time_interval; i++)
        collision.push_back(0);

    //Inserting 1s at proper bit positions
    for(int i = 0; i < forbidden.size(); i++){
        bit_pos = forbidden[i];
        collision[bit_pos]  = 1;
    }

    //Reversing the collision vector
    reverse(collision.begin(), collision.end());

    //Removing the redundant LSB
    collision.pop_back();

    std::cout<<endl<<"Collision Vector : ";
    std::cout<<"[ ";
    for(int i = 0; i < time_interval - 1; i++){
        std::cout<<collision[i]<<" ";
    }
    std::cout<<"]"<<endl;
}

