#include <iostream>
#include <fstream>
#include <vector>
#include <bitset>
#include <math.h>
#include <unordered_map>

//DEFINES
#define READING_BLOCK_SIZE 1024

using namespace std;

bool C1 = false;
bool C2 = false;
int wordLength = 0;

string readBuffer;
string writeBufferBits;
string writeBufferBytes;
string filename;
bool endOfFile = false;


bool error = false;

unsigned int currentPosition = 0;
unordered_map<unsigned long long int,string> positions; //key is position of last word occurence, in value we save that word.

int permutationsCount = 0;
void parseArguments(int argc, char *argv[]){
    if(argc>2){
        error = true;
    }

    filename = argv[1];
}

bool openFile(ifstream *file){
    file->open(filename,ios::binary);
    return file;
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

void preProcess(){
    
}

void readHeader(ifstream *file){
    char firstByte;
    file->get(firstByte);

    if(firstByte & 128){ //first bit is indicator whether it is C1 or C2 coding (Elias gamma or Elias delta)
        C2 = true;
    }else{
        C1 = true;
    }

    wordLength = int((firstByte >> 3)&15); //take next 4 bits
    
    readBuffer = bitset<3>(firstByte&7).to_string(); //last 3 bits goes to buffer for further processing

    cout<<"Zodzio ilgis: "<<wordLength<<endl;
    //cout<<"": "wordLength<<endl;
    //cout<<"Zodzio ilgis: "wordLength<<endl;
    permutationsCount = pow(2,wordLength);
    currentPosition = permutationsCount;
    cout<<"currentPosition: "<<currentPosition<<endl;
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
        cout<<"i:"<<i<<endl;;
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
                //cout<<zerosCounter<<" "<<i<<" "<<u<<"\n";
                ustr+=readBuffer.substr(i+1,zerosCounter);
                cout<<"ustr: "<<ustr<<endl;
                //subractOne(u); //function not necessary as it could be firstly converted to int and same operation applied, due to the fact that we use stoi later anyways
                int u = stoi(ustr, nullptr, 2 ) - 1;
                if(C2){//Elias delta  
                    if(readBuffer.size() - (i+zerosCounter+1) >= u){
                        ustr = '1';
                        if(u>0){
                            ustr+=readBuffer.substr(i+zerosCounter+1,u);
                            i+=u;
                        }
                        cout<<"ustr1: "<<ustr<<endl;
                        cout<<u;
                        u = stoi(ustr,nullptr,2) - 1;
                    }else{
                        readBuffer = readBuffer.substr(i-zerosCounter);
                        return;
                    }
                }
                cout<<"CurrenPosition: "<<currentPosition<<" stoi: "<<u<<endl;
                int at = currentPosition - u - 1;
                cout<<"at: "<<at<<endl;
                string word = positions[at];
                writeBufferBits+=word;
                positions.erase(at);
                positions[currentPosition]=word;
                currentPosition++;
                i+=zerosCounter+1;
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
    cout<<writeBufferBits<<endl;
    while(writeBufferBits.size()>=8){
        for ( int i = 0; i < 8; i++){
            if(writeBufferBits[i]=='1'){
                cout<<"pries"<<int(byte)<<endl;
                byte|=(128>>i);
                cout<<"po"<<int(byte)<<endl;
            }
        }
        writeBufferBytes.push_back(byte);
        writeBufferBits = writeBufferBits.substr(8);
    }
    cout<<writeBufferBytes;
    outputFile->write(writeBufferBytes.c_str(), writeBufferBytes.size());
}

int main(int argc, char *argv[]){

    ifstream inputFile;
    string outputFilename = "decompressed-";

    parseArguments(argc, argv);
    if(!openFile(&inputFile)){ //TODO:printinti error msg
        return 1;
    }
    outputFilename+=filename;
    outputFilename = "test.bin";
    ofstream outputFile(outputFilename, ios::out | ios::binary);

    readHeader(&inputFile); 

    string str(wordLength,'0');
    int temp = 0;
    generateBinaryStrings(wordLength,str,0,temp);
    for( const auto& n : positions ) {
        std::cout << "Key:[" << n.first << "] Value:[" << n.second << "]\n";
    }
    while(!endOfFile){
        readBlockFromFile(&inputFile);
        eliasDecoding();
        writeToFile(&outputFile);
    }

    inputFile.close();
    outputFile.close();
    return 0;
}
