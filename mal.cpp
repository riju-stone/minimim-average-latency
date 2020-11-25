#include <iostream>
#include <vector>
#include <algorithm>

using namespace std;

class ReservationTable{
    private:
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
    public:
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

    //Removing duplicates from the local forbidden vector
    std::sort(forb.begin(), forb.end());
    std::unique_copy(forb.begin(), forb.end(), std::back_inserter(forbidden));

    //Removing 0 as a forbidden latency
    forbidden.erase(forbidden.begin());

    cout<<endl;
    cout<<endl<<"Forbidden Latency : ";
    for(int i = 0; i < forbidden.size(); i++){
        cout<<forbidden[i]<<", ";
    }
}

void ReservationTable :: collisionVect(){
    int bit_pos;
    collision.resize(forbidden.size());

    for(int i = 0; i < collision.size(); i++){
        
    }

    cout<<endl<<"Collision Vector : ";
    cout<<"[ ";
    for(int i = 0; i < forbidden.size(); i++){
        cout<<collision[i]<<" ";
    }
    cout<<" ]"<<endl;
}

int main(){

    Cycles obj;

    obj.readTable();
    obj.forbiddenLat();
    obj.collisionVect();

    return 0;
}