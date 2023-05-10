// in this code implementation eviction from L1 and L2 are independent from each other 

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
    vector < vector< vector<long long int> > > cache; // valid,dirty,address,counter
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
            vector <vector<long long int> > row;
            for(int j=0; j<way;j++){
                vector<long long int> a;
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
    int check(long long int address){
        long long int index = (address/blocksz)%set;
        // cout<<"inside check"<<endl;
        for (int i = 0; i<assoc; i++){
            // cout<<"inside check loop"<<endl;
            if (cache[index][i][2] == address/blocksz){
                // cout<<index<<endl;
                return 1;
            }
        }
        return 0;
    }
    // reading data from memory
    vector<long long int> read(long long int address,long long int counter){
        long long int index = (address/blocksz)%set;
        vector<long long int> v;
        for (int i = 0; i<assoc; i++){
            if (cache[index][i][2] == address/blocksz){
                cache[index][i][3]=counter;
                v.push_back(1);
                v.push_back(cache[index][i][1]);
                return v;
            }
        }
        v.push_back(0);
        v.push_back(0);
        cout<<"hehe not as smart as you think"<<endl;
        return v;
    }
    // updating data
    int update(long long int address,long long int counter,int dirtybit)
    {

        long long int index = (address/blocksz)%set;
        for (int i = 0; i<assoc; i++){
        if (cache[index][i][2] == address/blocksz){
                cache[index][i][3]=counter;
                cache[index][i][1]=dirtybit;
                return 1;
            }
        }
        return 0;
    }

    vector<long long int> write(long long int address,long long int counter,int dirtybit){
        // cout<<"write "<<address<<endl;
        vector<long long int> v;
        long long int index = (address/blocksz)%set;
        int psecudo_assoc=-1;
        for (int i = 0; i<assoc; i++){
            if(cache[index][i][2]==-1)
            {
               psecudo_assoc=i; 
               break;
            }
        }
        // 2 status code means written first time at this address
        if(psecudo_assoc!=-1)
        {
            cache[index][psecudo_assoc][3]=counter;
            cache[index][psecudo_assoc][1]=dirtybit; 
            cache[index][psecudo_assoc][0]=1;
            cache[index][psecudo_assoc][2]=address/blocksz;
            v.push_back(1);
            v.push_back(-1);
            v.push_back(-1);
            v.push_back(0);
            return v;
        }

        long long int mincounter=cache[index][0][3];
        int minindex=0;
        for (int i=1;i<assoc;i++)
        {
            if(cache[index][i][3]<mincounter)
            {
                minindex=i;
                mincounter=cache[index][i][3];
            }
        }
        long long int ret_address=cache[index][minindex][2]*blocksz;
        long long int ret_counter=cache[index][minindex][3];
        int ret_dirtybit=cache[index][minindex][1];
        cache[index][minindex][3]=counter;
        cache[index][minindex][2]=address/blocksz;
        cache[index][minindex][0]=1;
        cache[index][minindex][1]=dirtybit;
        v.push_back(0);
        v.push_back(ret_address);
        v.push_back(ret_counter);
        v.push_back(ret_dirtybit);
        return v;
    }

    int invalidate(long long int address,long long int counter,int dirtybit)
    {
        // cout<<"update" <<" "<<address<<endl;
        long long int index = (address/blocksz)%set;
        for (int i = 0; i<assoc; i++){
        if (cache[index][i][2] == address/blocksz){
                cache[index][i][3]=-1;
                cache[index][i][2]=-1;
                cache[index][i][1]=0;
                cache[index][i][0]=0;
                return 1;
            }
        }
        return 0;
    }
};


int main(int argc, char *argv[]){
    int blsize=stoi(argv[1]);
    int l1size=stoi(argv[2]);
    int l1assoc=stoi(argv[3]);
    int l2size=stoi(argv[4]);
    int l2assoc=stoi(argv[5]);
    int l1set = l1size/(blsize*l1assoc);
    int l2set = l2size/(blsize*l2assoc);
    Cache L1cache = Cache(l1set,l1assoc,blsize);
    Cache L2cache = Cache(l2set,l2assoc,blsize);
    ifstream myfile(argv[6]);            
    // int evict_addresi; 
    int glcounter,i; 
    int l=0;
    cout<<"Ankit mondal"<<endl;
    while(!myfile.eof()){
        i=l;
        l++;
        // cout<<"starting new line reading from file line no "<<i<<endl;
        glcounter=i;
        vector<long long int> data;
        string line,action,word2;
        getline(myfile,line);
        stringstream ss(line); // Create a stringstream object with the line
        ss >> action >> word2; // Extract the two words from the line 
        long long int address_int = stoll(word2, 0, 16);
        // cout<<"priting address and action "<<address_int<<" "<<action<<endl;
        if ((address_int/64)%8==7){
            // cout<<l<<"lines"<<endl;
            // cout<<L1cache.cache[7][0][1]<<" "<<L1cache.cache[7][1][1]<<"hehe last try"<<endl;
        }
        if (action=="r"){
            long long int address=address_int;
            int f1 = L1cache.check(address); // is data there or not .
            // tuple<int,int,int,int> ret_write;
            vector<long long int> ret_write;
            // tuple<int,int> ret_read;
            vector<long long int> ret_read;
            int f2;
            if (f1==0){
                L1cache.readmiss+=1;
                f2 = L2cache.check(address);
                if (f2==0){
                   L2cache.readmiss += 1;
                   ret_write=L1cache.write(address,glcounter,0);
                   if(ret_write[3]==1)
                   {
                        L1cache.writeback+=1; // dirty bit one check it 
                        // cout<<ret_write[1]*64<<" "<<ret_write[2]<<" "<<glcounter<<" "<<address<<00000<<endl;
                        // cout<<L1cache.cache[7][0][2]*64<<" "<<L1cache.cache[7][0][3]<<" "<<L1cache.cache[7][1][2]*64<<" "<<L1cache.cache[7][1][3]<<endl;
                         L2cache.writehit+=1;
                        if (L2cache.check(ret_write[1])){
                            glcounter++;
                        L2cache.update(ret_write[1],glcounter,ret_write[3]);}
                        else{
                            // vector<long long int> inv_write;
                            // inv_write = L2cache.write(ret_write[1],ret_write[2],ret_write[3]);
                            // if (L1cache.check(inv_write[1])){
                            //     L1cache.invalidate(inv_write[1],inv_write[2],inv_write[3]);
                            // }
                            // if(inv_write[3]==1)
                            // {
                            //     L2cache.writeback+=1;
                            // }
                        }
                   }
                   ret_write=L2cache.write(address,glcounter,0);
                   if (L1cache.check(ret_write[1])){
                                L1cache.invalidate(ret_write[1],ret_write[2],ret_write[3]);
                                // cout<<"It was all a dream"<<endl;
                            }
                   if(ret_write[3]==1)
                   {
                        L2cache.writeback+=1;
                   }
                }
                else{
                   L2cache.readhit+=1;
                   ret_read=L2cache.read(address,glcounter);
                   if ((address_int/64)%8==7){
                    // cout<<"L2read check"<<ret_read[1]<<endl;
                }
                //    ret_write=L1cache.write(address,glcounter,ret_read[1]);
                    ret_write=L1cache.write(address,glcounter,0);
                   if(ret_write[3]==1)
                   {
                        // L1cache.writeback+=1;
                        // int status=L2cache.update(ret_write[1],ret_write[2],ret_write[3]);

                        L1cache.writeback+=1; // dirty bit one check it 
                        // cout<<ret_write[1]*64<<" "<<ret_write[2]<<" "<<glcounter<<" "<<address<<11111111<<endl;
                        // cout<<L1cache.cache[7][0][2]*64<<" "<<L1cache.cache[7][0][3]<<" "<<L1cache.cache[7][1][2]*64<<" "<<L1cache.cache[7][1][3]<<endl;
                         L2cache.writehit+=1;
                         glcounter++;
                        if (L2cache.check(ret_write[1])){
                        L2cache.update(ret_write[1],glcounter,ret_write[3]);}
                        else{
                            // vector<long long int> inv_write;
                            // inv_write = L2cache.write(ret_write[1],ret_write[2],ret_write[3]);
                            // if (L1cache.check(inv_write[1])){
                            //     L1cache.invalidate(inv_write[1],inv_write[2],inv_write[3]);
                            // }
                            // if(inv_write[3]==1)
                            // {
                            //     L2cache.writeback+=1;
                            // }
                        }
                        // if(status)
                        // {
                        //     // cout<<"passed"<<endl;
                        // }
                   }
                }
            }
            else{
                L1cache.readhit+=1;
                if ((address_int/64)%8==7){
                    // cout<<l<<"lines"<<endl;
                    // cout<<L1cache.cache[7][0][1]<<" "<<L1cache.cache[7][1][1]<<"hehe last try "<<L1cache.check(address)<<endl;
                    // cout<<"here"<<endl;
                }
                ret_read=L1cache.read(address,glcounter);
            }
        }
        else{
            long long int address=address_int;
            int f1 = L1cache.check(address); // is data there or not .
            // tuple<int,int,int,int> ret_write;
            // tuple<int,int> ret_read;
            vector<long long int> ret_write;
            vector<long long int> ret_read;
            int f2;
            if (f1==0){
                L1cache.writemiss+=1;
                f2 = L2cache.check(address_int);
                if (f2==0){
                   L2cache.readmiss += 1;
                   ret_write=L1cache.write(address,glcounter,1);
                   if(ret_write[3]==1)
                   {
                        L1cache.writeback+=1; // dirty bit one check it 
                        L2cache.writehit+=1;
                        // L2cache.update(ret_write[1],ret_write[2],ret_write[3]);
                        // cout<<ret_write[1]*64<<" "<<ret_write[2]<<" "<<glcounter<<" "<<address<<22222<<endl;
                        // cout<<L1cache.cache[7][0][2]*64<<" "<<L1cache.cache[7][0][3]<<" "<<L1cache.cache[7][1][2]*64<<" "<<L1cache.cache[7][1][3]<<endl;
                        glcounter++;
                        if (L2cache.check(ret_write[1])){
                        L2cache.update(ret_write[1],glcounter,ret_write[3]);}
                        else{
                            // vector<long long int> inv_write;
                            // inv_write = L2cache.write(ret_write[1],ret_write[2],ret_write[3]);
                            // if (L1cache.check(inv_write[1])){
                            //     L1cache.invalidate(inv_write[1],inv_write[2],inv_write[3]);
                            // }
                            // if(inv_write[3]==1)
                            // {
                            //     L2cache.writeback+=1;
                            // }
                        }
                   }
                   ret_write=L2cache.write(address,glcounter,0);// ----------- changes made here.
                   if (L1cache.check(ret_write[1])){
                                L1cache.invalidate(ret_write[1],ret_write[2],ret_write[3]);
                                // cout<<"It was all a dream"<<endl;
                            }
                   if(ret_write[3]==1)
                   {
                        L2cache.writeback+=1;
                   }
                }
                else{
                //    L2cache.writehit+=1;
                   L2cache.readhit+=1; // new points
                   L2cache.update(address,glcounter,0); // ----------- changes made here
                //    ret_read=L2cache.read(address,glcounter);
                   ret_write=L1cache.write(address,glcounter,1);
                   if(ret_write[3]==1)
                   {
                        L1cache.writeback+=1;
                         L2cache.writehit+=1;
                        // int status=L2cache.update(ret_write[1],ret_write[2],ret_write[3]);
                        // cout<<ret_write[1]*64<<" "<<ret_write[2]<<" "<<glcounter<<" "<<address<<33333<<endl;
                        // cout<<L1cache.cache[7][0][2]*64<<" "<<L1cache.cache[7][0][3]<<" "<<L1cache.cache[7][1][2]*64<<" "<<L1cache.cache[7][1][3]<<endl;
                        glcounter++;
                        if (L2cache.check(ret_write[1])){
                        L2cache.update(ret_write[1],glcounter,ret_write[3]);
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
    int l1cwrites=(L1cache.writehit+L1cache.writemiss);
    int l1creads=(L1cache.readhit+L1cache.readmiss);
    int l2cwrites=(L2cache.writehit+L2cache.writemiss);
    int l2creads=(L2cache.readhit+L2cache.readmiss);
    int answer=(1*(l1creads+l1cwrites)+20*(l2creads+l2cwrites)+200*(L2cache.readmiss+L2cache.writemiss) + 20*(L1cache.writeback)+200*(L2cache.writeback));
    
    float l1writes=(L1cache.writehit+L1cache.writemiss);
    float l1reads=(L1cache.readhit+L1cache.readmiss);
    float l2writes=(L2cache.writehit+L2cache.writemiss);
    float l2reads=(L2cache.readhit+L2cache.readmiss);

    float missrate1=(L1cache.readmiss+L1cache.writemiss)/(l1reads+l1writes);
    float missrate2=(L2cache.readmiss+L2cache.writemiss)/(l2reads+l2writes);

    cout<<"L1cache readhit : "<<L1cache.readhit<<endl;
    cout<<"L1cache readmiss : "<<L1cache.readmiss<<endl;
    cout<<"L1cache writehit : "<<L1cache.writehit<<endl;
    cout<<"L1cache writemiss : "<<L1cache.writemiss<<endl;
    cout<<"L1cache writeback : "<<L1cache.writeback<<endl;
    cout<<"L1cache missrate : "<<missrate1<<endl;
    cout<<"L2cache readhit : "<<L2cache.readhit<<endl;
    cout<<"L2cache readmiss : "<<L2cache.readmiss<<endl;
    cout<<"L2cache writehit : "<<L2cache.writehit<<endl;
    cout<<"L2cache writemiss : "<<L2cache.writemiss<<endl;
    cout<<"L2cache writeback : "<<L2cache.writeback<<endl;
    cout<<"L2cache missrate : "<<missrate2<<endl;
    cout<<"total access time : "<<answer<<endl;
    return 0;
}
