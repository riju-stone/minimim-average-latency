#include <iostream>
#include <vector>
#include <algorithm>
#include <unordered_map>

using namespace std;

class ReservationTable{
    protected:
        int stages, time_interval;            
        // TODO: Improve table system
        vector<int> table[10];
        vector<int> forbidden; 
        vector<int> collision;
    public:
        //Create the reservation table
        void readTable();
        //Calculate the forbidden latencies
        void forbiddenLat();
        //Calculate the collision vector
        void collisionVect();
};

class StateDiagram : public ReservationTable{
    protected:
        unordered_map<string, vector<int>> states;
    public:
        void addStage();
        void insertEdge();
        void buildSD();
};

class Cycles : public StateDiagram{
    public:
        void simpleCycles();
        void greedyCycles();
};

void ReservationTable :: readTable(){
    int temp;

    cout<<"Enter number of stages"<<endl;
    cin>>stages;
    cout<<"Enter the number of clock cycles"<<endl;
    cin>>time_interval;

    //TODO: Improve the input process...
    for(int i = 0; i < stages; i++){
        cout<<"Enter the values for Stage-"<<(i + 1)<<", row-wise (1 for job execituion 0 for nothing)"<<endl;
        for(int j = 0; j < time_interval; j++){
            cin>>temp;
            table[i].push_back(temp);
        }
    }

    // TODO: Refactor Code
    cout<<"Reservation Table :"<<endl;
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
}

void ReservationTable :: forbiddenLat(){

    int latency;
    int pos1, pos2;
    vector<int> buffer;
    vector<int> forb;
    //TODO : Optimize this code

    //Pushing all the obvious latensies into the buffer
    for(int i = 0; i < stages; i++){
        for(int j = 0; j < time_interval; j++){
            if(table[i][j] == 1)
                buffer.push_back(j);
        }
    }

    //Sorting the buffer
    std::sort(buffer.begin(), buffer.end());

    //Calculating the hidden latencies and pushing them into a local forbidden vector
    for(int i = 0; i < buffer.size(); i++){
        for(int j = i+1; j < buffer.size(); j++){
            latency = buffer[j] - buffer[i];
            forb.push_back(latency);
        }
    }

    //Sorting the elements and removing duplicates from the local forbidden vector
    sort(forb.begin(), forb.end());
    unique_copy(forb.begin(), forb.end(), std::back_inserter(forbidden));

    //Removing 0 as it's not need as forbidden latency
    forbidden.erase(forbidden.begin());

    cout<<endl;
    cout<<endl<<"Forbidden Latency : ";
    for(int i = 0; i < forbidden.size(); i++){
        cout<<forbidden[i]<<", ";
    }
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

    cout<<endl<<"Collision Vector : ";
    cout<<"[ ";
    for(int i = 0; i < time_interval; i++){
        cout<<collision[i]<<" ";
    }
    cout<<"]"<<endl;
}

void StateDiagram :: buildSD(){

    // Inserting the collision vector as the first state...
    states.insert(make_pair("root", collision));

}

int main(){

    Cycles obj;

    //Creating reservation table, forbidden latency and collision vector...
    obj.readTable();
    obj.forbiddenLat();
    obj.collisionVect();

    //Generating the state diagram...

    //Evaluating simple and greedy cycles...

    //Calculating the minimum average latency...

    return 0;
}