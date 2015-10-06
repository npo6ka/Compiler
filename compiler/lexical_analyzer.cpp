#include <iostream>
#include <fstream>
#include <string>

using namespace std;

#define NUM_CLASS_LEX 6
#define SIZE_CASH_TABLE 256

char* FillTable(const string str[], char *cash, const int num) { 
    for (int i = 0; i < num; i++) {
        for (int j = 0; j < str[i]._Mysize; j++) {
            if (str[i][j] >= 0 && str[i][j] < 256 && cash[str[i][j]] == -1) {
                cash[str[i][j]] = i+1;
            } else {
                cout << "error fill cash table: symbol: " << str[i][j] << endl;
            }
        }
    } 
    return cash;
}

void AssignClass(const char* const cash, const char c, int *ClassLex) {
    if (*ClassLex == 0 || *ClassLex == -1) {
        *ClassLex = cash[c];
    }
}

int HandleCosnt(const char c, int stat) { // class 1
    static char tab[5][8] = {
//       st int  .*  *. flt   E +/- float
        { 1,  1,  4,  4,  3,  7,  7,  7}, // 0 - 9
        {99,  8,  8,  8,  9,  6, 99,  9}, // +/-
        { 2,  3,  8,  8,  7, 99, 99,  9}, // .
        {99,  5, 99,  5,  5, 99, 99,  9}, // E
        {99,  8,  8,  8,  7, 99, 99,  9}, // other
    };
    int sym;
    if (c >= '0' && c <= '9') {
        sym = 0;
    } else   if (c == '+' && c == '-') {
        sym = 1;
    } else   if (c == '.') {
        sym = 2;
    } else   if (c == 'E') {
        sym = 3;
    } else {
        sym = 4;
    }
    return tab[sym][stat];
}

int HandlerAssignment(const char c, int stat) { //class 2
    static char tab[6][5] = {
//       st   +   -  *    /
        { 8,  9, 10, 11, 12}, // =
        { 1, 99, 99, 99, 99}, // +
        { 2, 99, 99, 99, 99}, // -
        { 3, 99, 99, 99, 99}, // *
        { 4, 99, 99, 99, 99}, // /
        {99, 99, 99, 99, 99}, // other
    };
    int sym;
    switch (c) {
    case '=':
        sym = 0;
        break;
    case '+':
        sym = 1;
        break;
    case '-':
        sym = 2;
        break;
    case '*':
        sym = 3;
        break;
    case '/':
        sym = 4;
        break;
    default:
        sym = 5;
    }
    return tab[sym][stat];
}
void HandlerLogical() { //class 3

}
void HandlerId() { //class 4
    
}
void HandlerSpecialSymbol() { //class 5
    
}
void HandlerSeparator() { //class 6
    
}

int Handler(const char c, int ClassLex, int stat) {
    
    switch (ClassLex) {
    case -1:
        cout << "class -1" << endl;     
        break;
    case 1:
        cout << "class 1" << endl;
        return HandleCosnt(c, stat);
        break;
    case 2:
        cout << "class 2" << endl;
        break;
    case 3:
        cout << "class 3" << endl;
        break;
    case 4:
        cout << "class 4" << endl;
        break;
    case 5:
        cout << "class 5" << endl;
        break;
    case 6:
        cout << "class 6" << endl;
        break;
    default:
        cout << "Error in ClassLexem" << endl;
    }

}

void ProcessingStatus(filebuf *file, int stat, int *ClassLex) {
    if (stat > 7) {
        *ClassLex = 0;
        stat = 0;
    }
}


int main() {
    fstream fs;
    //int StringCount = 0;
    //int PosCount = 0;
    int ClassLexems = 0;
    int status = 0;
    char symbol = 0;
    char cash[SIZE_CASH_TABLE];
    string buff = "";
    string str[NUM_CLASS_LEX] = {                                 //0 - unknown
        "0123456789.",                                            //1 - const
        "=*+-/",                                                  //2 - Assignment operation
        "|&",                                                     //3 - Logical Expression
        "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ_$", //4 - identificator and Reserved Word
        "{}();",                                                  //5 - special symbol
        " \n"                                                     //6 - separator
                                                                  //-1 - errors  
    };
    fs.open("input.txt");
    if (!fs.is_open()) {
        cout <<  "Error opening file" << endl;
        return -1;
    }
    filebuf* inbuf  = fs.rdbuf();  //std::streamsize size = inbuf->pubseekoff(0,fs.end);
    memset(cash, -1, sizeof(char)*SIZE_CASH_TABLE);
    FillTable(str, cash, NUM_CLASS_LEX);

    do {
        symbol = inbuf->sbumpc(); //c = inbuf->sputbackc(c);
        AssignClass(cash, symbol, &ClassLexems);
        status =  Handler(symbol, ClassLexems, status);
        ProcessingStatus(inbuf, status, &ClassLexems);
        cout << symbol;
    } while (symbol != EOF);

    cout << endl;
    fs.close();
    return 1;
}

/*
0 const ::= INT FLOAT                     ('0'-'9', '.')                  ("0123456789.+-E")
1 AsExp ::= = *= /= += -=                 ('=', '*', '/', '+', '-')       ("=*+-/")
2 LoExp ::= || && | &                     (|, &)                          ("|&")
3 Indef ::= ID                            ('a'-'z', 'A'-'Z', '_', '$')    ("abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ_$")
4 ResWr ::= if else for in return with    ('i','e','f','i','r','w')       
5 SpeCh ::= { } ( ) ;                     ('{','}','(',')',';')           ("{}(),;")
6 Divid ::= \0 \n _                       (' ','\n','\0')                 (" ,\n\0")

01010000
hsdfsdfdfsdf d h g ddfg i sdfsd
*/