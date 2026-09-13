#include<iostream>
#include<vector>
using namespace std;


struct Cache{
    int set ;
    int assoc ;
    vector < vector<tuple <int,int,int> > > cache;
    int readhit;
    int readmiss;
    int writehit;
    int writemiss;
    Cache(int sets, int way){
        set = sets;
        assoc = way;
        for(int i=0; i<sets; i++){
            vector <tuple <int,int,int>> row;
            for(int j=0; j<way;j++){
                row.push_back(make_tuple(0,0,0));
            }
            cache.push_back(row);
        }
        readhit = 0;
        readmiss =0;
        writehit = 0;
        writemiss = 0;
    }
};


int main(int blsize, int l1size, int l1assoc,int l2size,int l2assoc){
    int l1set = l1size/(blsize*l1assoc);
    int l2set = l2size/(blsize*l2assoc);
    int l1cache[l1set][l1assoc];
    int l2cache[l2set][l2assoc];
    Cache L1cache = Cache(l1set,l1assoc);
    Cache L2cache = Cache(l2set,l2assoc);
    vector <int,int> data;
    for(int i=0; i<data.size(); i++){
        if data[i][0] = 0{
            int f1 = L1cache.find(data[i][1]);
            int f2;
            if (f1==0){
                 L1cache.readmiss+=1;
                f2 = L2cache.find(data[i][1]);
                if (f2==0){
                   L2cache.readmiss += 1;
                }
                else{
                   L2cache.readhit+=1;
                }
                L1.put(data[i][1]);
                // check evicted value here
            }
            else{
                L1cache.readhit+=1;
            }
        }
        else{
            int f1 = L1cache.find(data[i][1]);
            int f2;
            if (f1==0){
                L1cache.writemiss+=1;
                f2 = L2cache.find(data[i][1]);
                if (f2==0){
                   L2cache.writemiss += 1;
                }
                else{
                   L2cache.writehit+=1;
                }
                L1.put2(data[i][1]);
            }
            else{
                L1cache.writehit+=1;
                L1.put2(data[i][1]);
            }
        }
    }
}