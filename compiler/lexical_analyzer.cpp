#include <iostream>
#include <fstream>
#include <string>



using namespace std;

#define NUM_CLASS_LEX 6
#define SIZE_CASH_TABLE 256

char* FillTable(string str[], char *cash, int num) { 
    for (int i = 0; i < num; i++) {
        for (int j = 0; j < str[i]._Mysize; j++) {
            if (str[i][j] >= 0 && str[i][j] < 256 && cash[str[i][j]] == -1) {
                cash[str[i][j]] = i;
            } else {
                cout << "error fill cash table: symbol: " << str[i][j] << endl;
            }
        }
    } 
    return cash;
}

int main() {
    fstream fs;
    fs.open("input.txt");
    if (!fs.is_open()) {
        cout <<  "Error opening file" << endl;
        return -1;
    }
    filebuf* inbuf  = fs.rdbuf();
    //std::streamsize size = inbuf->pubseekoff(0,fs.end);
    int StringCount = 0, PosCount = 0, ClassLexems = 0;
    char c = 0;
    string str[NUM_CLASS_LEX] = {
        "0123456789.",
        "=*+-/",
        "|&",
        "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ_$",
        "{}();",
        " \n"
    };
    char cash[SIZE_CASH_TABLE];
    memset(cash, -1, sizeof(char)*SIZE_CASH_TABLE);



    FillTable(str, cash, NUM_CLASS_LEX);
    while (c != EOF) {
        c = inbuf->sbumpc();

        cout << c;
    }
    cout << endl;
    

    fs.close();
    return 1;
}

/*
0 const ::= INT FLOAT                     ('0'-'9', '.')                  ("0123456789.+-E")
1 AsExp ::= = *= /= += -=                 ('=', '*', '/', '+', '-')       ("=*+-/")
2 LoExp ::= || && | &                     (|, &)                          ("|&")
3 Indef ::= ID                            ('a'-'z', 'A'-'Z', '_', '$')    ("abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ_&")
4 ResWr ::= if else for in return with    ('i','e','f','i','r','w')       
5 SpeCh ::= { } ( ) ;                     ('{','}','(',')',';')           ("{}(),;")
6 Divid ::= \0 \n _                       (' ','\n','\0')                 (" ,\n\0")

01010000
hsdfsdfdfsdf d h g ddfg i sdfsd
*/