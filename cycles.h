#include <iostream>
#include <vector>
#include <algorithm>
#include <unordered_map>
#include <fstream>
#include <map>
#include <bitset>

#include "state.h"

#define MAX 16
using namespace std;

class Cycles : public StateDiagram{
    private:
        // 2D string vectors for storing the cycles
        vector<vector<string>> simple;
        vector<vector<string>> greedy;
        double mal;
    public:
        void simpleCycles(State *node, vector<bitset<MAX>> trav, vector<string> cycle);
        void greedyCycles(State *node, vector<bitset<MAX>> trav, vector<string> cycle);
        void calcMAL();
};

void Cycles::simpleCycles(State *node, vector<bitset<MAX>> trav, vector<string> cycle){

    vector<bitset<MAX>>::iterator it;
    vector<int>::iterator i;

    trav.push_back(node->bset);

    string str;

    //Traversing the edges
    for(Edge *e = node->edge; e != nullptr; e = e->next){
        str.clear();
        i = e->weights.begin();

        while(1){
            str += to_string(*i);
            i++;

            if(i != e->weights.end())
                str = ",";
            else
                break;
        }

        cycle.push_back(str);
        it = find(trav.begin(), trav.end(), e->to->bset);

        //If the traversing path reaches a end vertex other than starting vertex
        if(it == trav.end())
            simpleCycles(e->to, trav, cycle);
        //If the traversing path reaches the starting vertex
        else if(it == trav.begin())
            simple.push_back(cycle);
        cycle.pop_back();
    }
}

void Cycles::greedyCycles(State *node, vector<bitset<MAX>> trav, vector<string> cycle){

    vector<bitset<MAX>>::iterator it;

    trav.push_back(node->bset);
    Edge *e = node->edge;

    //Pushing the smallest edge in the cycle
    cycle.push_back(to_string(e->weights.front()));
    it = find(trav.begin(), trav.end(), e->to->bset);
    //If the traversing path reaches a end vertex other than starting vertex
    if(it == trav.end())
        greedyCycles(e->to, trav, cycle);
    //If the traversing path reaches the starting vertex
    else if(it == trav.begin())
        greedy.push_back(cycle);
    cycle.pop_back();
}

void Cycles::calcMAL(){

    vector<bitset<MAX>> traversed;
    vector<string> cycle;

    for(State *ptr = start; ptr != nullptr; ptr = ptr->next){
        cycle.clear();
        traversed.clear();

        traversed.push_back(ptr->bset);

        for(State *p = start; p != ptr; p = p->next)
            traversed.push_back(p->bset);

        simpleCycles(ptr, traversed, cycle);
        greedyCycles(ptr, traversed, cycle);
    }

    vector<vector<string>>::iterator it;
    vector<string>::iterator it2;

    //Displaying all the simple cycles.
    cout<<"\nSimple Cycles:"<<endl;
    for(it = simple.begin(); it != simple.end(); it++){
        cout<<"( ";

        for(it2 = (*it).begin();;){
            cout<<*it2;
            if(++it2 != (*it).end())
                cout<<" => ";
            else 
                break;
        }
        cout<<" )"<<endl;
    }

    mal = INT8_MAX;
    double min;

    cout<<"\nGreedy Cycles:"<<endl;
    for(it = greedy.begin(); it != greedy.end(); it++){
        cout<<"( ";

        for(it2 = (*it).begin();;){
            cout<<(*it2);
            //Adding the weights of all the edges and storing in a temp var
            min += stoi(*it2);

            if(++it2 != (*it).end())
                cout<<" => ";
            else
                break;
        }

        //Calculating the temp mal
        if(min < mal)
            mal = min;

        cout<<" )"<<endl;
    }

    cout<<"\nMinimum Average Latency: "<<mal<<endl;

}