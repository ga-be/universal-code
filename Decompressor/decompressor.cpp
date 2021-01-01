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

string readBuffer;
string writeBuffer;
bool endOfFile = false;


bool openFile(ifstream *file){
    file->open("example.bmp",ios::binary);
    return file;
}

void readHeader(ifstream *file){
    char firstByte;
    file->get(firstByte);

    if(firstByte & 128){ //first bit is indicator whether it is C1 or C2 coding (Elias gamma or Elias delta)
        C2 = true;
    }else{
        C1 = true;
    }

    tailLength = int((firstByte >> 3)&15); //take next 4 bits

    readBuffer = bitset<3>(firstByte & 7).to_string(); //append last 3 bits to buffer for decompression
}

void readBlockFromFile(ifstream *file){
    char * memblock;
    memblock = new char[READING_BLOCK_SIZE];
    file->read(memblock,READING_BLOCK_SIZE);
    int bytesRead = file->gcount();
    
    for (int i = 0; i < bytesRead; i++) { 
       readBuffer += bitset<8>(memblock[i]).to_string();
    }

    if (bytesRead!=READING_BLOCK_SIZE) {
        endOfFile = true;
    }
    delete[] memblock;
}

void eliasGammaDecoding(){
    int zerosCounter = 0;
    string buffer;
    
    for (int i = 0; i < readBuffer.size() ;){
        if(readBuffer[i]=='0'){
            zerosCounter++;
            i++;
        }else{ 
            string u="1";
            if(readBuffer.size()-i-1 >= zerosCounter){//means there is enough chars left to read in buffer
                cout<<zerosCounter<<" "<<i<<" "<<u<<"\n";
                u+=readBuffer.substr(i+1,zerosCounter);
                cout<<u<<'\n';
            }else{
                readBuffer=readBuffer.substr(i-zerosCounter);
                return;
            }
            i+=zerosCounter+1;
            zerosCounter = 0;
        }
    }
    readBuffer.erase();
}

void eliasDeltaDecoding(){

}

int main(){

    ifstream file;
    if(!openFile(&file)){ //TODO:printinti error msg
        return 1;
    }
    readHeader(&file);
    while(!endOfFile){
        readBlockFromFile(&file);
        eliasGammaDecoding();
         
    }

    


    file.close();
}
