#include <iostream>
#include <fstream>
#include <string>

using namespace std;

int main() {
    fstream fs;
    fs.open ("input.txt");
    if (!fs.is_open()) {
        cout <<  "Error opening file" << endl;
        return -1;
    }
    filebuf* inbuf  = fs.rdbuf();
    char c = 0;
    while (c != EOF) {
        c = inbuf->sbumpc();
        cout << c;
    }
    cout << endl;

    fs.close();
    return 1;
}

/*
const ::= INT FLOAT
AsExp ::= = *= /= += -=
LoExp ::= || && | &
Indef ::= ID 
ResWr ::= if else for in return with 
SpeCh ::= { } ( ) ; \0 \n _ 



01010000
hsdfsdfdfsdf d h g ddfg i sdfsd
*/