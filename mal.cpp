#include <iostream>
#include <vector>
#include <algorithm>
#include <unordered_map>
#include <fstream>
#include <map>
#include <bitset>

#define MAX 16
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

struct Edge;

struct State{
    /*
        A bitset stores bits (elements with only two possible values: 0 or 1, 
        true or false, ...). The class emulates an array of bool elements, 
        but optimized for space allocation: generally, each element occupies 
        only one bit (which, on most systems, is eight times less than the smallest 
        elemental type: char).
    */
    bitset<MAX> bset;   //Stores the states bitwise
    Edge *edge;
    State *next;
};

struct Edge{
    vector<int> weights;
    //Destination node pointer
    State *to;
    Edge *next;
};

class StateDiagram : public ReservationTable{
    protected:
        int cvsize; //Size of the collision vector
        bitset<MAX> initCol; //Stores the initial collision vector...
        State *start;
    public:
        void addState (State *state);
        void insertEdge (State *state);
        void buildSD();
};

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

void StateDiagram :: addState(State *state){
    
    // Stores and modifies the current new state and adds it
    bitset<MAX> temp;

 State *ptr;

    for(int i = 0; i < cvsize; i++){
        // Adding new state at position start
        if(!state->bset[i]){

            //Right Shifting current state ith no. of times and OR-ing with start
            temp = state->bset;
            temp = temp >> (i+1);
            temp = temp | start->bset;

            State *node = new State;

            node->bset = temp;
            node->next = nullptr;
            node->edge = nullptr;

            // If the new state is different than all the states currently available, then the new starte is added
            for(ptr = start; ptr->bset != node->bset && ptr->next; ptr = ptr->next){
                if(ptr->bset != node->bset)
                    ptr->next = node;
            }
        }
    }
}

void StateDiagram :: insertEdge(State *state){

    bitset<MAX> temp;
    State *ptr;
    Edge *head = nullptr;
    Edge *e;

    for(int i = 0; i < cvsize; i++){
        if(!state->bset[i]){
            temp = state->bset;
            temp = temp >> (i+1);
            temp = temp | start->bset;

            for(ptr = start; ptr->bset != temp; ptr= ptr->next);
            if(!head){
                //If no edge is inserted
                head = new Edge;
                head->weights.push_back(i+1);
                head->to = ptr;
                head->next = nullptr;
            }
            else{
                //Traverse to the end of the linked list and add an edge
                for(e = head; e->to != ptr, e->next; e = e->next);
                //If edge has no destination state
                if(e->to != ptr){
                    Edge *node = new Edge;
                    node->weights.push_back(i+1);
                    node->next = nullptr;
                    node->to = ptr;
                    e->next = node;
                }
                else{
                     e->weights.push_back(i+1);
                }
            }
        }
    }
    if(!head)
	{
		head = new Edge;
		head->weights.push_back(cvsize+1);
		head->next = nullptr;
		head->to = start;
	}
	else
	{
		for(e = head; e->to != start && e->next; e = e->next);
		if(e->to == start)
			e->weights.push_back(cvsize+1);
		else
		{
			Edge *node=new Edge;
			node->weights.push_back(cvsize+1);
			node->next = nullptr;
			node->to = start;
			e->next = node;
		}
	}

    state->edge = head;
    vector<int>::iterator it;

    std::cout<<"State : "<<state->bset<<endl;
    for(e = head; e != nullptr; e = e->next){
        std::cout<<"\t";
        for(it = e->weights.begin(); it != e->weights.end();){
            std::cout<<*it;
            if(++it != e->weights.end())
                std::cout<<", ";
        }
        std::cout<<"\t => "<<e->to->bset;
        std::cout<<endl;
    }
}

void StateDiagram::buildSD(){
    vector<int>::iterator it;

    for(it = forbidden.begin(); it != forbidden.end(); it++)
        initCol.set(*it - 1, 1);

    //Collision vector is the first and initial state
    start = new State;
    start->bset = initCol;
    start->next = nullptr;
    start->edge = nullptr;

    cvsize = forbidden.size();

    State *ptr;
    std::cout<<"\nState Diagram : "<<endl;
    for(ptr = start; ptr != nullptr; ptr = ptr->next){
        addState(ptr);
        insertEdge(ptr);
    }
    std::cout<<endl;
}

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
        min = min / (*it).size();

        //Storing the mal if its minimal
        if(min < mal)
            mal = min;

        cout<<" )"<<endl;
    }

    cout<<"\nMinimum Average Latency: "<<mal<<endl;

}

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