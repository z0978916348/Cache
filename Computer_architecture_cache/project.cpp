#include <iostream>
#include <fstream>
#include <cmath>
using namespace std;

int main(int argc, char **argv)
{
    string trash;
    string reference;
    int Adress_bits, Block_size, Cache_sets, Associativity;
    int Offset_bit_count, Indexing_bit_count;
    
    ifstream inFile_cache(argv[1], ios::in);
    if (! inFile_cache) {
        cout << "cannot open " << argv[1] << endl;
        return 1;
    }

    inFile_cache >> trash >> Adress_bits >> trash >> Block_size >> trash >> Cache_sets >> trash >> Associativity;
    
    Offset_bit_count = log2(Block_size);
    Indexing_bit_count = log2(Cache_sets);

    ifstream inFile_reference(argv[2], ios::in);
    if (! inFile_reference) {
        cout << "cannot open " << argv[2] << endl;
        return 1;
    }
    string lst_name1, lst_name2;
    inFile_reference >> lst_name1 >> lst_name2;

    int testcase[Adress_bits];
    int index = 0, p = 1;

    string **cache = new string*[Cache_sets];
    bool **referenced = new bool*[Cache_sets];   // if referenced = true, else = false. if all true , then all set false
    for (int i=0; i<Cache_sets; i++) {
        cache[i] = new string[Associativity];
        referenced[i] = new bool[Associativity]; 
    }
    
    for (int i=0; i<Cache_sets; i++) {
        for (int j=0; j<Associativity; j++) {
            cache[i][j] = "I";
            referenced[i][j] = false;
        }
    }
    
    ofstream outFile(argv[3], ios::out);
    if ( !outFile ) {
        cout << "cannot read " << argv[3] << endl;
        return 1;
    }

    outFile << "Address bits: " << Adress_bits << endl;
    outFile << "Block size: " << Block_size << endl;
    outFile << "Cache sets: " << Cache_sets << endl;
    outFile << "Associativity: " << Associativity << endl << endl;
    outFile << "Offset bit count: " << Offset_bit_count << endl;
    outFile << "Indexing bit count: " << Indexing_bit_count << endl;
    outFile << "Indexing bits: ";
    for (int i=Offset_bit_count+Indexing_bit_count-1; i>=Offset_bit_count; i--) outFile << i << " ";  
    outFile << endl << endl;
    outFile << lst_name1 << " " << lst_name2 << endl;
    bool jump = false;
    bool hit_or_miss = true;
    int counter = 0;
    int Total_cache_miss_count = 0;
    while (1) {
        index = 0, p = 1, jump = false, counter = 0, hit_or_miss = true;  
        inFile_reference >> reference;
        if (reference == ".end") break;
        for (int i=0; i<Adress_bits; i++) testcase[Adress_bits-i-1] = int(reference[i]) - 48; // reverse, testcase[0] -> first bit 
                
        for (int i=Offset_bit_count; i<Offset_bit_count+Indexing_bit_count; i++) {
            index += testcase[i] * p;
            p *= 2;
        }


        for (int i=0; i<Associativity; i++) {  // check whether hit or miss  
            hit_or_miss = true;
            for (int j=0; j<Adress_bits-Offset_bit_count; j++) {
                if ( cache[index][i][j] !=  reference[j] ) {
                    hit_or_miss = false;
                    break;
                }
            }
            if (hit_or_miss) {
                //outFile << "number = " << i << endl;
                referenced[index][i] = true;
                break;
            }
        }



        if (hit_or_miss) {            // Hit!!!
            //cout << reference << " hit" << endl;
            outFile << reference << " hit" << endl; 
            continue;   
        } else {                      // Miss!!!
            //cout << reference << " miss" << endl;
            outFile << reference << " miss" << endl;
            Total_cache_miss_count += 1;
        }

        for (int i=0; i<Associativity; i++) {  // check whether empty space exist
            if ( cache[index][i] == "I" ) {
                cache[index][i] = reference;
                referenced[index][i] = true;
                jump = true;
                break;
            } 
        }
        
        if ( jump ) continue;  

        for (int i=0; i<Associativity; i++) {  // check if all referenced
            if ( referenced[index][i] == true ) counter++;
            else break;
        }

        if (counter == Associativity) {        // if all referenced, set all referenced as false
            //if (index == 11) outFile << " all referenced " << endl;
            for (int i=0; i<Associativity; i++) {
                referenced[index][i] = false;
            }
        }        
            
                

        for (int i=0; i<Associativity; i++) {  // if all occupied, find not referenced space from head
            if ( referenced[index][i] == false ) {
                //if ( index == 11) outFile << "miss number = " << i << endl;
                referenced[index][i] = true;
                cache[index][i] = reference;
                jump = true;
                break;
            } 
        }

        if ( jump ) continue;
    }
    //cout << ".end" << endl << endl << "Total cache miss count: " << Total_cache_miss_count << " ";
    outFile << ".end" << endl << endl << "Total cache miss count: " << Total_cache_miss_count << endl;

    return 0;
}