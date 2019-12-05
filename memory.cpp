#include <iostream>
#include <string>
#include <vector>
#include <iomanip> 
#include <cmath>
#include <cstdlib>
#include <ctime>
#include<fstream>

using namespace std;


static char MEM[160000];

//////////////////////////////////////////////////////////////////////////////////////////////////////////

    int read_word(char *mem,  int address) {
        int *data;
        data =  (int*)(mem + address);
        return *data;
    }
    // Writing instructions to memory
    void write_word(char *mem,  int address, int data) {
    
    int *data_p;
    data_p = (int*) (mem + address);
    *data_p = data;
    }

    void load_program_memory(int inp[10000],int size) {
        int address;
         int instruction[10000];
        for(int ll=0; ll<size;ll++)
        {instruction[ll]=inp[ll];}
        for(int op=0;op<size;op++){
            write_word(MEM, op, instruction[op]);
        }
    }

int cash_type, block_size, cash_size,number_of_blocks=0;;

int compulsry_misses=0, capcity_misses=0, conflict_misses=0;

int fifo_used_no=0,fifo_index=0;

bool cacheSim(unsigned int addr, int cash[3][100000], int type, int &block_counter, int index_addr, int tag_addr)
{
    int shift_offset=log2(block_size);
    bool detected=false;
    bool misses_flag=true; int fifo[10000];int fifo_used_no=0,fifo_index=0;

    if (cash_type==0)    // Direct Mapped ******************************************
    {
        if (cash[0][index_addr]==tag_addr){
            return true;
        }
        else
            {
                for (int i=0; i < number_of_blocks; i++){
                    if (cash[1][i]!=1)    
                    { 
                        misses_flag=false;
                        i=number_of_blocks; 
                    }
                }
                //  calculating misses//
                if (misses_flag)
                   {
     capcity_misses++;   // capcity miss because the cash is full ############# FIFO IS IMPLEMENTED HERE SO AS TO REPLACE A BLOBK BY NEW BLOCK WHEN CACHE IS FULL
                        fifo[fifo_index]=index_addr;
                        cash[1][fifo[fifo_used_no]]=1;
                        cash[0][fifo[fifo_used_no]]=tag_addr;
                        fifo_used_no++;
                        fifo_index++;
                        return 0;
                   }
                else {
                    if(cash[1][index_addr]==1)
                        conflict_misses++;
                    else{
                        compulsry_misses++;
                    }
                }
                cash[0][index_addr]= tag_addr ;
                cash[1][index_addr]= 1;
                fifo[fifo_index]= index_addr;
                fifo_index++;
                return 0;
            }
    } 
    // end of directed mapped
    ///////////////////////////////////////////////////////////////////
    else if (cash_type==1)   // set asscoiative *************************************
    {
        index_addr=index_addr * type;
        for (int i=0; i < type ; i++) {
            if (cash[0][index_addr+i]==tag_addr) {
                return 1;
            }
        }
        for (int j=0; j < type; j++){
            if (cash[1][index_addr+j] == -1) {
                compulsry_misses++;
                cash[0][index_addr+j]=tag_addr;
                cash[1][index_addr+j]=1;
                fifo[fifo_index]=index_addr+j;
                fifo_index++;
                return 0;
            }
        }
        
        //######################## CASE OF CAPACITY MISS = WHEN CACHE IS FULL ##############################3
        cash[0][fifo[fifo_used_no]]=tag_addr;
        cash[1][fifo[fifo_used_no]]=1;
        fifo_used_no++;
        fifo_index++;
        capcity_misses++;
        return 0;
        
    }//end of set assciative

    
    
    else if (cash_type==2)        // fully associative **************************************
    {
            if (block_counter < number_of_blocks){
                for (int i=0; i < number_of_blocks; i++){
                    if (cash[0][i]==addr >> shift_offset){
                        detected=true;
                        cash[1][i]=block_counter;
                        block_counter--;
                        return detected; //hit
                    }
                }
                
                if (!detected){
                    compulsry_misses++;
                    fifo[fifo_index]=block_counter;
                    fifo_index++;
                    cash[0][block_counter]=addr>>shift_offset;
                    cash[1][block_counter]=block_counter;
                    return false;  //miss
                }
            }
            else  // block counter is more than block size
            {
                //check for existence
                for (int i=0; i < number_of_blocks; i++){
                    if (cash[0][i]==(addr >> shift_offset)){
                        detected=true;
                        cash[1][i]=block_counter;
                        fifo[fifo_index]=block_counter;
                        fifo_index++;
                        //block_counter--;
                        return detected; //hit
                    }
                }
                
                if (!detected){
                    
                    cash[0][fifo[fifo_used_no]]=addr >> shift_offset;
                    cash[1][fifo[fifo_used_no]]=block_counter;
                    capcity_misses++;
                    fifo_used_no++;
                    fifo_index++;
                    return false; //miss
                }
            }//END OF FIFO////////////////////////////////////////////////////
                       




        if (type==0)    // LRU /////////
        {
            if (block_counter < number_of_blocks){
                for (int i=0; i < number_of_blocks; i++){
                    if (cash[0][i]==addr >> shift_offset){
                        detected=true;
                        cash[1][i]=block_counter;
                        block_counter--;
                        return detected; //hit
                    }
                }
                
                if (!detected){
                    compulsry_misses++;
                    cash[0][block_counter]=addr>>shift_offset;
                    cash[1][block_counter]=block_counter;
                    return false;  //miss
                }
            }
            else  // block counter is more than block size
            {
                //check for existence
                for (int i=0; i < number_of_blocks; i++){
                    if (cash[0][i]==(addr >> shift_offset)){
                        detected=true;
                        cash[1][i]=block_counter;
                        //block_counter--;
                        return detected; //hit
                    }
                }
                
                if (!detected){
                    int compare=0;
                    for (int i=1; i < number_of_blocks; i++){
                        if (cash[1][compare] > cash[1][i])
                            compare=i;
                    }
                    cash[0][compare]=addr >> shift_offset;
                    cash[1][compare]=block_counter;
                    capcity_misses++;
                    return false; //miss
                }
            }
        }   // end of LRU


        
    }   // end of Fully associative
    return true;
}
char *msg[2] = {"Miss","Hit"};


///////////////////////////////////////////////////////////////////

int main(int argc, const char * argv[]) {
    
    
    
    int looper, addr, flag, shift;
    cout << " 0 => DM, 1 => SA, 2 =>FA: " << endl;
    cin >> cash_type;
    cout << "size of the block as a Power of 2 between 4 and 128 byte  :" << endl;
    cin >> block_size;
    cout << " enter cache size:(size in bytes (large value preferred)); in steps that are power of 2: " << endl;
    cin >> cash_size;
    
    int cash[3][100000];
    int block_counter=0;
    int hit_counter=0;
    int index_addr=0, tag_addr=0;
        string line1;
        int fil[10000];int kp=0;
     
     ifstream fin1("mach_code.mc");
        int n,m,i=0,lines,c1[10000];
        while(fin1>>n){
        c1[i]=n;
        i++;
        }
        lines=i;
        load_program_memory(c1,lines);


looper=i;
      
      int instruction[10000];
            
            for(int ll=0; ll<i;ll++){
                instruction[ll]=c1[ll];
            }
                                

    ///////////////////////////////////////////////////////////////////
    if ( cash_type==0)   //Direct_mapped 
    {
        number_of_blocks= (cash_size)/block_size;
        ////////////////////
        for (int i=0; i < 2; i++)   // setting all the cash with -1
            for (int j=0; j < number_of_blocks; j++)
                cash[i][j]=-1;
        //////////////////
          
            
        for(int i=0; i <looper ;i++){    
            addr = i;
           
            shift= log2(block_size);
            index_addr= (addr >> shift)% number_of_blocks;
            shift= log2(number_of_blocks+block_size);
            tag_addr= addr >>shift;    
            flag = cacheSim(addr, cash, 0,block_counter, index_addr, tag_addr);
            index_addr=0;
            tag_addr=0;
            cout  << instruction[i] <<"("<< msg[flag]<<")\n";
            if (msg[flag]=="Hit"){
                hit_counter++;
            }
        }

    cout << "Hits  " << hit_counter<<endl << "COLD:  " << compulsry_misses <<endl<< "Capcity:  " << capcity_misses <<endl<< "Conflict:  " << conflict_misses <<  endl;
    }
    ///////////////////////////////////////////////////////////////////
    else if (cash_type==2)  //  Fully associative**************
    {
        int replacment_type;
        number_of_blocks= (cash_size)/block_size;
          replacment_type=0;

        
        for (int i=0; i < 2; i++)   // setting all the cash with -1
            for (int j=0; j < number_of_blocks; j++)
                cash[i][j]=-10;
        
        for(int i=0; i <looper ;i++){
            addr = i;
            flag = cacheSim(addr, cash, replacment_type, block_counter, index_addr, tag_addr);
            if (msg[flag]=="Hit")  {
                hit_counter++;
            }
            block_counter++;
            cout << instruction[i]<<"("<< msg[flag]<<")"<< endl;
        }

        cout << "Hits  " << hit_counter<<endl << "COLD:  " << compulsry_misses <<endl<< "Capcity:  " << capcity_misses <<endl<< "Conflict:  " << conflict_misses <<  endl;
        
    } // end of fully associative main
    else if (cash_type==1) // set associative
    {
        int ways;
        cout << "enter k for associative cash: 2,4,8,16" << endl;
        cin >> ways;
        number_of_blocks= (cash_size)/(block_size*ways);
        
        for (int i=0; i < 3; i++)   // setting all the cash with -1
            for (int j=0; j < 100000; j++)
                cash[i][j]=-1;
        
        for(int i=0; i <looper ;i++) {
            addr = i;
            shift= log2(block_size);
            index_addr= (addr >> shift)% (number_of_blocks);
            shift= log2(number_of_blocks+block_size);
            tag_addr= addr >>shift;
            flag = cacheSim(addr, cash, ways, block_counter, index_addr, tag_addr);
            index_addr=0;
            tag_addr=0;
            if (msg[flag]=="Hit"){
                hit_counter++;
            }
            block_counter++;
            cout << instruction[i]<<"("<< msg[flag]<<")"<< endl;
        }
        cout << "Hits  " << hit_counter << endl << "COLD:  " << compulsry_misses << endl << "Capcity:  " << capcity_misses <<endl<< "Conflict:  " << conflict_misses <<  endl;
    }
}


