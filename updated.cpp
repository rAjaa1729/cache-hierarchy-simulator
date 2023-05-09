#include<iostream>
#include<vector>
#include<tuple>
#include<sstream>
#include<fstream>
using namespace std;

// int glcounter;

struct Cache{
    int set ;
    int assoc ;
    int blocksz;
    vector < vector< vector<int> > > cache; // valid,dirty,address,counter
    int readhit;
    int readmiss;
    int writehit;
    int writemiss;
    int writeback;

    Cache(int sets, int way,int blksize){
        set = sets;
        assoc = way;
        blocksz = blksize;
        for(int i=0; i<sets; i++){
            vector <vector<int> > row;
            for(int j=0; j<way;j++){
                vector<int> a;
                a.push_back(0);
                a.push_back(0);
                a.push_back(-1);
                a.push_back(-1);
                row.push_back(a);
            }
            cache.push_back(row);
        }
        readhit = 0;
        readmiss =0;
        writehit = 0;
        writemiss = 0;
        writeback = 0;
    }
    // searching data in cache 1 for found and 0 for not
    int check(int address){
        int index = (address/blocksz)%set;
        cout<<"inside check"<<endl;
        for (int i = 0; i<assoc; i++){
            // cout<<"inside check loop"<<endl;
            if (cache[index][i][2] == address/blocksz){
                cout<<index<<endl;
                return 1;
            }
        }
        return 0;
    }
    // reading data from memory
    tuple<int,int> read(int address,int counter){
        int index = (address/blocksz)%set;

        for (int i = 0; i<assoc; i++){
            if (cache[index][i][2] == address/blocksz){
                cache[index][i][3]=counter;
                return make_tuple(1,cache[index][i][1]);
            }
        }
        
        int mincounter=cache[index][0][3];
        int minindex=0;
        for (int i=1;i<assoc;i++)
        {
            if(cache[index][i][3]<mincounter)
            {
                minindex=i;
                mincounter=cache[index][i][3];
            }
        }
        cache[index][minindex][3]=counter;
        cache[index][minindex][2]=address/blocksz;
        cache[index][minindex][0]=1;
        cache[index][minindex][1]=0;
        return make_tuple(0,0);
    }
    // updating data
    int update(int address,int counter,int dirtybit)
    {
        cout<<"update" <<" "<<address<<endl;
        int index = (address/blocksz)%set;
        for (int i = 0; i<assoc; i++){
        if (cache[index][i][2] == address/blocksz){
                cache[index][i][3]=counter;
                cache[index][i][1]=dirtybit;
                return 1;
            }
        }
    }

    tuple<int,int,int,int> write(int address,int counter,int dirtybit){
        cout<<"write "<<address<<endl;
        int index = (address/blocksz)%set;
        int psecudo_assoc=-1;
        for (int i = 0; i<assoc; i++){
            if(cache[index][i][2]==-1)
            {
               psecudo_assoc=i; 
            }
        }
        // 2 status code means written first time at this address
        if(psecudo_assoc!=-1)
        {
            cache[index][psecudo_assoc][3]=counter;
            cache[index][psecudo_assoc][1]=0; 
            cache[index][psecudo_assoc][0]=dirtybit;
            cache[index][psecudo_assoc][2]=address/blocksz;
            return make_tuple(1,-1,-1,0);
        }

        int mincounter=cache[index][0][3];
        int minindex=0;
        for (int i=1;i<assoc;i++)
        {
            if(cache[index][i][3]<mincounter)
            {
                minindex=i;
                mincounter=cache[index][i][3];
            }
        }
        int ret_address=cache[index][minindex][2];
        int ret_counter=cache[index][minindex][3];
        int ret_dirtybit=cache[index][minindex][1];
        cache[index][minindex][3]=counter;
        cache[index][minindex][2]=address/blocksz;
        cache[index][minindex][0]=1;
        cache[index][minindex][1]=dirtybit;
        return make_tuple(0,ret_address,ret_counter,ret_dirtybit);
    }
};


int main(int argc, char *argv[]){//int blsize, int l1size, int l1assoc,int l2size,int l2assoc){
    // glcounter=0;
    int l1size = stoi(argv[2]);
    int blsize = stoi(argv[1]);
    int l1assoc = stoi(argv[3]);
    int l2size = stoi(argv[4]);
    int l2assoc = stoi(argv[5]);
    cout<<l1size<<"l1size"<<endl;
    int l1set = l1size/(blsize*l1assoc);
    int l2set = l2size/(blsize*l2assoc);
    Cache L1cache = Cache(l1set,l1assoc,blsize);
    Cache L2cache = Cache(l2set,l2assoc,blsize);
    ifstream myfile(argv[6]);            
    // int evict_addresi; 
    int glcounter,i; 
    int l=0;
    while(!myfile.eof() && l<40){
        i=l;
        l++;
        cout<<"starting new line reading from file line no "<<i<<endl;
        glcounter=i;
        vector<int> data;
        string line,action,word2;
        getline(myfile,line);
        stringstream ss(line); // Create a stringstream object with the line
        ss >> action >> word2; // Extract the two words from the line 
        int address_int = stoi(word2, 0, 16);
        cout<<"priting address and action "<<address_int<<" "<<action<<endl;
        if (action=="r"){
            int address=address_int;
            int f1 = L1cache.check(address); // is data there or not .
            tuple<int,int,int,int> ret_write;
            tuple<int,int> ret_read;
            int f2;
            if (f1==0){
                L1cache.readmiss+=1;
                f2 = L2cache.check(address);
                if (f2==0){
                   L2cache.readmiss += 1;
                   ret_write=L1cache.write(address,glcounter,0);
                   if(get<3>(ret_write))
                   {
                        L1cache.writeback+=1; // dirty bit one check it 
                        L2cache.update(get<1>(ret_write),get<2>(ret_write),get<3>(ret_write));
                   }
                   ret_write=L2cache.write(address,glcounter,0);
                   if(get<3>(ret_write))
                   {
                        L2cache.writeback+=1;
                   }
                }
                else{
                   L2cache.readhit+=1;
                   ret_read=L2cache.read(address,glcounter);
                   ret_write=L1cache.write(address,glcounter,get<1>(ret_read));
                   if(get<3>(ret_write))
                   {
                        L1cache.writeback+=1;
                        int status=L2cache.update(get<1>(ret_write),get<2>(ret_write),get<3>(ret_write));
                        if(status)
                        {
                            cout<<"passed"<<endl;
                        }
                   }
                }
            }
            else{
                L1cache.readhit+=1;
                ret_read=L1cache.read(address,glcounter);
            }
        }
        else{
            int address=address_int;
            int f1 = L1cache.check(address); // is data there or not .
            tuple<int,int,int,int> ret_write;
            tuple<int,int> ret_read;
            int f2;
            if (f1==0){
                L1cache.writemiss+=1;
                f2 = L2cache.check(address_int);
                if (f2==0){
                   L2cache.writemiss += 1;
                   ret_write=L1cache.write(address,glcounter,1);
                   if(get<3>(ret_write))
                   {
                        L1cache.writeback+=1; // dirty bit one check it 
                        L2cache.update(get<1>(ret_write),get<2>(ret_write),get<3>(ret_write));
                   }
                   ret_write=L2cache.write(address,glcounter,1);
                   if(get<3>(ret_write))
                   {
                        L2cache.writeback+=1;
                   }
                }
                else{
                   L2cache.writehit+=1;
                   L2cache.update(address,glcounter,1);
                //    ret_read=L2cache.read(address,glcounter);
                   ret_write=L1cache.write(address,glcounter,1);
                   if(get<3>(ret_write))
                   {
                        L1cache.writeback+=1;
                        int status=L2cache.update(get<1>(ret_write),get<2>(ret_write),get<3>(ret_write));
                        if(status)
                        {
                            cout<<"passed"<<endl;
                        }
                   }
                }
            }
            else{
                L1cache.writehit+=1;
                L1cache.update(address,glcounter,1);
            }
        }
    }
    cout<<" final answer "<<endl;
    cout<<" for l1"<<endl;
    cout<<L1cache.readhit<<endl;
    cout<<L1cache.readmiss<<endl;
    cout<<L1cache.writehit<<endl;
    cout<<L1cache.writemiss<<endl;
    cout<<L1cache.writeback<<endl;
    cout<<" for l2"<<endl;
    cout<<L2cache.readhit<<endl;
    cout<<L2cache.readmiss<<endl;
    cout<<L2cache.writehit<<endl;
    cout<<L2cache.writemiss<<endl;
    cout<<L2cache.writeback<<endl;
    // cout<<L1cache.check(data[2][1])<<endl;
}
