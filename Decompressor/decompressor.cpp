#include <iostream>
#include <fstream>
#include <vector>
#include <bitset>
#include <math.h>
#include <unordered_map>
#include <map>
#include <chrono>

//DEFINES
#define READING_BLOCK_SIZE 1024

using namespace std;

bool C1 = false;
bool C2 = false;
int wordLength = 0;

string readBuffer;
string writeBufferBits;
string writeBufferBytes;
string fullpath;
string outputFilename;
string errorTxt;
bool endOfFile = false;


bool error = false;

unsigned int currentPosition = 0;
unordered_map<unsigned long long int,string> positions; //key is position of last word occurence, in value we save that word.


int permutationsCount = 0;
void parseArguments(int argc, char *argv[]){
    if(argc>2){
        errorTxt="Only 1 argument could be given.\n";
        error = true;
        return;
    }
    fullpath = argv[1];
    size_t found=fullpath.find(".elias");
    if (found==string::npos){
        error = true;
        errorTxt="Compressed file extension must be .elias.\n";
        return;
    }
    else{
        string pathWithoutElias = fullpath.substr(0,found);
        size_t extensionDot = pathWithoutElias.find_last_of('.');
        outputFilename = pathWithoutElias.substr(0,extensionDot) + "-decompressed" + pathWithoutElias.substr(extensionDot);
    }
}

void generateBinaryStrings(int n, string binStr, int i, int &count){
    if(i == n){
        positions[count] = binStr;
        count++;
        return;
    }

    binStr[i]='0';
    generateBinaryStrings(n,binStr,i+1, count);

    binStr[i]='1';
    generateBinaryStrings(n,binStr,i+1, count);
}

void readHeader(ifstream *file){
    char firstByte;
    file->get(firstByte);

    if(firstByte & 128){ //first bit is indicator whether it is C1 or C2 coding (Elias gamma or Elias delta)
        C2 = true;
    }else{
        C1 = true;
    }

    wordLength = int((firstByte >> 3)&15)+1; //take next 4 bits
    
    readBuffer = bitset<3>(firstByte&7).to_string(); //last 3 bits goes to buffer for further processing

    permutationsCount = pow(2,wordLength);
    currentPosition = permutationsCount;
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

void eliasDecoding(){
    int zerosCounter = 0;
    string buffer;
    
    for (int i = 0; i < readBuffer.size() ;){
        if(readBuffer[i]=='0'){
            if(i == readBuffer.size()-1){
                readBuffer = readBuffer.substr(i-zerosCounter);
                return;
            }
            zerosCounter++;
            i++;
        }else{ 
            string ustr="1";
            if(readBuffer.size()-i-1 >= zerosCounter){//means there is enough chars left to read in buffer
                ustr+=readBuffer.substr(i+1,zerosCounter);
                int k = stoi(ustr, nullptr, 2 ) - 1; 
                if(C2){//Elias delta  
                    if(readBuffer.size() - (i+zerosCounter+1) >= k){
                        string vstr = "1";
                        if(k>0){
                            vstr+=readBuffer.substr(i+zerosCounter+1,k); 
                            i+=k; //skip delta code part
                        }
                        k = stoi(vstr,nullptr,2) - 1; //delta code part conversion to number
                    }else{
                        readBuffer = readBuffer.substr(i-zerosCounter);
                        return;
                    }
                }
                int at = currentPosition - k - 1; 
                string word = positions[at];
                writeBufferBits+=word;
                positions.erase(at);
                positions[currentPosition]=word;
                currentPosition++;
                i+=zerosCounter+1; //skip gamma code part
            }else{
                readBuffer=readBuffer.substr(i-zerosCounter);
                return;
            }
            zerosCounter = 0;
        }
    }
    readBuffer.clear();
}

void writeToFile(ofstream *outputFile){
    unsigned char byte;
    while(writeBufferBits.size()>=8){
        for ( int i = 0; i < 8; i++){ 
            if(writeBufferBits[i]=='1'){
                byte|=(128>>i);
            }
        }
        writeBufferBytes.push_back(byte);
        writeBufferBits = writeBufferBits.substr(8);
        byte^=byte; 
    }
    outputFile->write(writeBufferBytes.c_str(), writeBufferBytes.size());
    writeBufferBytes.clear();
}

int main(int argc, char *argv[]){

    auto t1 = chrono::high_resolution_clock::now();

    ifstream inputFile;

    parseArguments(argc, argv);
    inputFile.open(fullpath,ios::binary);
    if(!inputFile.is_open()){
        errorTxt="Error while opening file.\n";
        error=true;
    }
    if(!error){
        ofstream outputFile(outputFilename, ios::out | ios::binary);

        readHeader(&inputFile); 

        string str(wordLength,'0');
        int temp = 0;
        generateBinaryStrings(wordLength,str,0,temp);
    
        while(!endOfFile){
            readBlockFromFile(&inputFile);
            eliasDecoding();
            writeToFile(&outputFile);
        }
        inputFile.close();
        outputFile.close();
    }
    auto t2 = std::chrono::high_resolution_clock::now();
    auto duration = chrono::duration_cast<chrono::milliseconds>( (t2 - t1)).count()*0.001;

    if(!error){
        cout<<"Decompression completed!\n";
        cout<<"Input file: "<<fullpath<<"\n";
        cout<<"Output file: "<<outputFilename<<"\n";
        cout<<"Elapsed time: "<<duration<<"\n";
    }else{
        cout<<errorTxt;
    }
    return 0;
}
