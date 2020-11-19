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
    public:
        void readTable();
        void forbiddenLat();
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

    for(int i = 0; i < stages; i++){
        cout<<"Enter the values for Stage "<<(i + 1)<<", row-wise (1 for job execituion 0 for nothing)"<<endl;
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

    int minuend;

    for(int i = 0; i < stages; i++){
        for(int j = 0; j < time_interval; j++){
            //Calculate latency
            if(table[i][j] == 1)
                buffer.push_back(j);
        }
    }

    //Removing duplicates from the buffer
    std::sort(buffer.begin(), buffer.end());
    std::unique_copy(buffer.begin(), buffer.end(), std::back_inserter(forbidden));

    cout<<endl;
    cout<<endl<<"Forbidden Latency : ";
    for(int i = 0; i < forbidden.size(); i++){
        cout<<forbidden[i]<<", ";
    }
}

int main(){

    Cycles obj;

    obj.readTable();
    obj.forbiddenLat();

    return 0;
}