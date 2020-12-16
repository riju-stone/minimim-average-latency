#include <iostream>
#include <vector>
#include <algorithm>
#include <unordered_map>
#include <fstream>
#include <map>
#include <bitset>

#include "reservation.h"
#define MAX 16

using namespace std;

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
