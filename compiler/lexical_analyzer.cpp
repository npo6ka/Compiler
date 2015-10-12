#include <iostream>
#include <fstream>
#include <string>
#include <list>
#include <unordered_map>

using namespace std;

unordered_map<string, int> ListResWord;

#define NUM_CLASS_LEX 6
#define SIZE_CASH_TABLE 256

enum {
    UNK_C = 0,
    CONST = 1,
    AS_OP = 2,
    LO_OP = 3,
    IDEN  = 4,
    SP_SY = 5,
    ERR_C = 6,
    RES_W = 7,
};

enum {
    UNK    = 7, 
    //class const
    INT    = 16,
    FLOAT  = 17,

    //class Assignment operation
    ASIG   = 32,
    SUM    = 33,
    DIFF   = 34,
    MULT   = 35,
    DIVI   = 36,

    //class Logical Expression
    BN_AND = 48,
    LO_AND = 49,
    BN_OR  = 50,
    LO_OR  = 51,

    //class indificator
    IND    = 64,

    //class special symbol
    LFB    = 80, //left curly brace
    RFB    = 81, //right
    LF_PR  = 82, //left parenthesis
    RG_PR  = 83,
    SEMIC  = 84, //semicolon

    //class Error
    ERR    = 96,
    EX_ERR = 97, //end error
    EXIT   = 98, //not add list

    //class reserved word
    IF     = 112,
    ELSE   = 113,
    FOR    = 114,
    IN     = 115,
    RETURN = 116,
    WITH   = 117, 
};

struct lexem {
    char id;
    string str;
};

char* FillTable(const string str[], char *cash, const int num) { 
    for (int i = 0; i < num; i++) {
        for (int j = 0; j < str[i]._Mysize; j++) {
            if (str[i][j] >= 0 && str[i][j] < 256) {
                cash[str[i][j]] = i+1;
            } else {
                cout << "error fill cash table: symbol: " << str[i][j] << endl;
            }
        }
    } 
    return cash;
}

void AssignClass(const char* const cash, const char ch, int &ClassLex) {
    if (!ClassLex) {
        if (ch != EOF){
            ClassLex = cash[ch];
        } else {
            ClassLex = SP_SY;
        }
    }
}

void init(char *cash) {
    string str[NUM_CLASS_LEX-1] = {
        "0123456789.",                                            //1 - const
        "=*+-/",                                                  //2 - Assignment operation
        "|&",                                                     //3 - Logical Expression
        "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ_$", //4 - identificator
        "{}(); \n",                                               //5 - special symbol & separator
                                                                  //6 - errors 
    };                                                            //7 - Reserved Word
    memset(cash, 6, sizeof(char)*SIZE_CASH_TABLE);
    FillTable(str, cash, NUM_CLASS_LEX-1);
    ListResWord.insert(pair<string, int> ("if",     IF    ));
    ListResWord.insert(pair<string, int> ("else",   ELSE  ));
    ListResWord.insert(pair<string, int> ("for",    FOR   ));
    ListResWord.insert(pair<string, int> ("in",     IN    ));
    ListResWord.insert(pair<string, int> ("return", RETURN));
    ListResWord.insert(pair<string, int> ("with",   WITH  ));
}

int HandleConst         (char ch, int stat, filebuf  &file, string &str) { // class 1
    static char tab[8][5] = {
//      0-9   +/-     .      E     other
        { 1,  ERR,   2,     ERR,   ERR   }, // start
        { 1,  INT,   3,     5,     INT   }, // int
        { 4,  INT,   INT,   ERR,   INT   }, // .*
        { 4,  INT,   INT,   5,     INT   }, // *.
        { 4,  FLOAT, FLOAT, 5,     17    }, // float
        { 7,  6,     ERR,   ERR,   ERR   }, // E
        { 7,  ERR,   ERR,   ERR,   ERR   }, // +/-
        { 7,  FLOAT, FLOAT, FLOAT, FLOAT }  // float
    }; 
    if (ch >= '0' && ch <= '9') {
        stat = tab[stat][0];
    } else   if (ch == '+' || ch == '-') {
        stat = tab[stat][1];
    } else   if (ch == '.') {
        stat = tab[stat][2];
    } else   if (ch == 'E') {
        stat = tab[stat][3];
    } else {
        stat = tab[stat][4];
    }

    if (stat > 7 && stat != ERR) { 
        file.sputbackc(ch);
    } else {
        str += ch;
    }
    return stat;
}
int HandlerAssignment   (char ch, int stat, filebuf  &file, string &str) { //class 2
    if (ch == '=') {
        return ASIG;
    } else {
        char buff = file.sbumpc();
        if (buff == '=') {
            switch (ch) {
            case '+':
                return SUM;
            case '-':
                return DIFF;
            case '*':
                return MULT;
            case '/':
                return DIVI; 
            }
        } else {
            file.sputbackc(buff);
            str += ch;
            return ERR;
        }
    }
}
int HandlerLogical      (char ch, int stat, filebuf  &file, string &str) { //class 3
    static char tab[3][3] = {
//          |        &     other
        { 1,      2,       ERR    }, // start
        { LO_AND, BN_AND,  BN_AND }, // || or |
        { BN_AND, LO_AND,  BN_AND }, // && or &
    };
    switch (ch) {
    case '|':
        stat = tab[stat][0];
        break;
    case '&':
        stat = tab[stat][1];
        break;
    default:
        stat = tab[stat][2];
    }
    if (stat == BN_AND || stat == LO_AND) {
        file.sputbackc(ch);
        return stat;
    } else {
        return stat;
    }
}
int HandlerId           (char ch, int stat, filebuf  &file, string &str) { //class 4
    if ((ch >= 'a' && ch <= 'z') || (ch >= 'A' && ch <= 'Z') || (ch >= '0' && ch <= '9') || ch == '$' || ch == '_') {
        str += ch;
        return 1;
    } else {
        file.sputbackc(ch);
        return IND;
    }
}
int HandlerSpecialSymbol(char ch, int stat, filebuf  &file, string &str) { //class 5
    switch (ch) {
    case '{':
        return LFB;
    case '}':
        return RFB;
    case '(':
        return LF_PR;
    case ')':
        return RG_PR;
    case ';':
        return SEMIC;
    default:
        return EXIT;
    }
}
int HandlerError        (char ch, int stat, filebuf  &file, string &str) {
    if (ch == ' ' || ch ==  '\n' || ch ==  -1 || ch ==  '{' || ch ==  '}' || ch ==  '(' || ch == ')' || ch == ';') {
        file.sputbackc(ch);
        return EX_ERR;
    } else {
        str += ch;
        return ERR;
    }
}

void ProcessingStatus(int &stat, int &ClassLex, string &str, list<lexem> &lst) {
    if (stat == ERR) { //if error class = ERR
        ClassLex = ERR_C;
    } else if (stat > UNK) { // if stat > 7 lexem complited
        if (ClassLex == IDEN) { // if class = IDEN find reserved word
            unordered_map<string, int>::iterator it = ListResWord.find(str);
            if (it != ListResWord.end()) {
                stat = it->second;
                str = "";
            }
        } 
        if (stat != EXIT) { // if symbol = ' ', '\n', '\0' then don't add in list lexem
            lexem buff;
            buff.id = stat; //((ClassLex & 0xF) << 4) | (stat & 0x7);
            buff.str = str;
            lst.push_back(buff);       
        }
        str = "";
        ClassLex = UNK_C;
        stat = 0;
    } 
}

void PrintLex(list<lexem> lst) {
    char *str[RES_W] = {  "constant          ", 
                          "assigment operator", 
                          "logical operator  ", 
                          "identificator     ", 
                          "special symbol    ", 
                          "Error lexem       ", 
                          "reserved word     "};
    for (auto& it: lst) {
        cout <<  str[((it.id & 0xF0) >> 4)-1] << "   Lexem: " 
             << (it.id & 0xF) << "   str: " << it.str << endl;
    }
}

int main() {
    fstream fs;
    int ClassLexems = UNK_C;
    int status = 0;
    char symbol = 0;
    char cash[SIZE_CASH_TABLE];
    list<lexem> LstLex;
    string buff = "";
    init(cash);
    int (*lexHandler[NUM_CLASS_LEX])(char ch, int stat, filebuf  &file, string &str) = 
        {HandleConst, HandlerAssignment, HandlerLogical, HandlerId, HandlerSpecialSymbol, HandlerError};    
    fs.open("input.txt");
    if (!fs.is_open()) {
        cout <<  "Error opening file" << endl;
        return -1;
    }
    filebuf* inbuf  = fs.rdbuf();

    do {
        symbol = inbuf->sbumpc();
        AssignClass(cash, symbol, ClassLexems);
        status = lexHandler[ClassLexems-1](symbol, status, *inbuf, buff);
        ProcessingStatus(status, ClassLexems, buff, LstLex);
    } while (symbol != EOF);
    PrintLex(LstLex);
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