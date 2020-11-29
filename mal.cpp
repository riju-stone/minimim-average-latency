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
        // TODO: Improve table system
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

struct Header {
    /*
        A bitset stores bits (elements with only two possible values: 0 or 1, 
        true or false, ...). The class emulates an array of bool elements, 
        but optimized for space allocation: generally, each element occupies 
        only one bit (which, on most systems, is eight times less than the smallest 
        elemental type: char).
    */
    bitset<MAX> bset;
    Header *next;
    Edge *edge;
};

struct Edge{
    vector<int> weights;
    Edge *next;
    Header *to;
};

class StateDiagram : public ReservationTable{
    protected:
        int cvsize; //Size of the collision vector
        bitset<MAX> initialcv; //Stores the initial collision vector...
        Header *start;
    public:
        void addState(Header *state);
        void insertEdge(Header *state);
        void buildSD();
};

class Cycles : public StateDiagram{
    public:
        void simpleCycles();
        void greedyCycles();
};

void ReservationTable :: readTable(){
    
    // NEW INPUT SYSTEM
    ifstream fin;
    fin.open("ReservationTable.txt", ifstream::in);
    if(!fin){
        cout<<"Cannot open file..."<<endl;
        exit(1);
    }

    string str;
    getline(fin, str);
    stages = stoi(str);

    getline(fin, str);
    time_interval = stoi(str);
    
    /* OLD INPUT SYSTEM
    cout<<"Enter number of stages"<<endl;
    cin>>stages;
    cout<<"Enter the number of clock cycles"<<endl;
    cin>>time_interval;

    for(int i = 0; i < stages; i++){
        cout<<"Enter the values for Stage-"<<(i + 1)<<", row-wise (1 for job execution 0 for nothing)"<<endl;
        for(int j = 0; j < time_interval; j++){
            cin>>temp;
            table[i].push_back(temp);
        }
    }
    */

    int i = 0, j;
    string::iterator it;

    cout<<"Reservation Table :"<<endl;
    for(j = 1; j <= time_interval; j++)
        cout<<"\t"<<"t"<<j;
    cout<<endl;

    while(!fin.eof()){
        getline(fin, str);
        i++;
        j = 0;
        cout<<"S"<<i<<"\t";
        for(it = str.begin(); it != str.end(); it++){
            if(*it == '1'){
                cout<<"X"<<"\t";
                j++;
                table.insert(pair<int, int>(i, j));
            }
            else if(*it == '0'){
                cout<<"0"<<"\t";
                j++;
            }
        }
        cout<<endl;
    }
    cout<<endl;
    fin.close();
    /* OLD OUTPUT SYSTEM
    for(int i = 0; i < stages; i++){

        cout<<"S"<<(i + 1)<<"\t";
        for(int j = 0; j < time_interval; j++){
            cout<<table[i][j]<<"\t";
        }
        if( i == (stages - 1)){
            cout<<endl;
            cout<<"\t";
            for(int j = 0; j < time_interval; j++)
                cout<<"t"<<j<<"\t";
        }
        else
            cout<<endl;
    }
    */
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
                    lat = row[q] - row[p];
                    f = find(forbidden.begin(), forbidden.end(), lat);
                    if(f == forbidden.end() && lat > 0)
                        forbidden.push_back(lat);
            }
            row.clear();
    }
    sort(forbidden.begin(), forbidden.end());
    cout<<"\nForbidden latencies: ";
	for (int i = 0; i < forbidden.size(); i++)
        cout<<" "<<forbidden[i];
	cout<<endl;

}

//This function calculates the collision vector
void ReservationTable :: collisionVect(){
    int bit_pos;

    collision.reserve(time_interval);

    for(int i = 0; i < time_interval; i++)
        collision.push_back(0);

    for(int i = 0; i < forbidden.size(); i++){
        bit_pos = forbidden[i];
        collision[bit_pos]  = 1;
    }

    reverse(collision.begin(), collision.end());
    collision.pop_back();

    cout<<endl<<"Collision Vector : ";
    cout<<"[ ";
    for(int i = 0; i < time_interval - 1; i++){
        cout<<collision[i]<<" ";
    }
    cout<<"]"<<endl;
}

void StateDiagram :: addState(Header *state){
    
    // Stores and modifies the current new state and adds it
    bitset<MAX> temp;

    Header *ptr;

    for(int i = 0; i < cvsize; i++){
        // Adding new state at position *state
        if(!state->bset[i]){
            temp = state->bset;
            temp = temp >> (i+1);
            temp = temp | start->bset;

            Header *node = new Header;

            node->bset = temp;
            node->next = nullptr;
            node->edge = nullptr;

            for(ptr = start; ptr->bset != node->bset && ptr->next; ptr = ptr->next){
                if(ptr->bset != node->bset)
                    ptr->next = node;
            }
        }
    }
}

void StateDiagram :: insertEdge(Header *state){

    bitset<MAX> temp;
    Header *ptr;
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

    cout<<"State : "<<state->bset<<endl;
    for(e = head; e != nullptr; e = e->next){
        cout<<"\t";
        for(it = e->weights.begin(); it != e->weights.end();){
            cout<<*it;
            if(++it != e->weights.end())
                cout<<", ";
        }
        cout<<"\t => "<<e->to->bset;
        cout<<endl;
    }
}

void StateDiagram::buildSD(){
    vector<int>::iterator it;

    for(it = forbidden.begin(); it != forbidden.end(); it++)
        initialcv.set(*it - 1, 1);

    start = new Header;
    start->bset = initialcv;
    start->next = nullptr;
    start->edge = nullptr;

    cvsize = forbidden.size();

    Header *ptr;
    cout<<"\nState Diagram : "<<endl;
    for(ptr = start; ptr != nullptr; ptr = ptr->next){
        addState(ptr);
        insertEdge(ptr);
    }
    cout<<endl;
}

int main(){

    Cycles obj;

    //Creating reservation table, forbidden latency and collision vector...
    obj.readTable();
    obj.forbiddenLat();
    obj.collisionVect();

    //Generating the state diagram...
    obj.buildSD();
    
    //Evaluating simple and greedy cycles...

    //Calculating the minimum average latency...

    return 0;
}