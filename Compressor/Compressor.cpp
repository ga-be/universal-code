#include <iostream>
#include <fstream>
#include <vector>
#include <bitset>

//DEFINES
#define READING_BLOCK_SIZE 1024

using namespace std;

bool C1 = false;
bool C2 = false;
int wordLength = 0;
int tailLength = 0;

string buffer;
bool EOF;


bool openFile(ifstream *file){
    file->open("example.bmp",ios::binary);
    return file;
}

void readHeader(ifstream *file){
    char firstByte;
    file->get(firstByte);
    char secondByte;
    file->get(secondByte);

    if(firstByte & 128){ //first bit is indicator whether it is C1 or C2 coding (Elias gamma or Elias delta)
        C2 = true;
    }else{
        C1 = true;
    }

    wordLength = int((firstByte >> 3)&15); //take next 4 bits
    tailLength = int((firstByte << 1)&7) + int((secondByte >> 7)&1); //last 3 bits from first byte and first bit from second byte for tail length.

    buffer = bitset<7>(secondByte&127).to_string(); //next seven bits goes to buffer for processing
}

void readBlockFromFile(ifstream *file){
    char * memblock;
    memblock = new char[READING_BLOCK_SIZE];
    file->read(memblock,READING_BLOCK_SIZE);
    int bytesRead = file->gcount();
    
    for (int i = 0; i < bytesRead; i++) { // O jei nuskaito nepilna bloka?
       buffer += bitset<8>(memblock[i]).to_string();
    }

    if (bytesRead!=READING_BLOCK_SIZE) {
        EOF = true;
    }
    delete[] memblock;
}


int main(){
    ifstream file;
    if(!openFile(&file)){ //TODO:printinti error msg
        return 1;
    }
    readHeader(&file);
    while(!EOF){
        readBlockFromFile(&file);
        
        
    }

    


    file.close();
}
