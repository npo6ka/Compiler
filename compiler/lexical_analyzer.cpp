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
    UNK   = 0,
    CONST = 1,
    AS_OP = 2,
    LO_OP = 3,
    IDEN  = 4,
    SP_SY = 5,
    ERR   = 6,
    RES_W = 7,
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
        ClassLex = cash[ch];
    }
}

void init(char *cash) {
    string str[NUM_CLASS_LEX-1] = {
        "0123456789.",                                            //1 - const
        "=*+-/",                                                  //2 - Assignment operation
        "|&",                                                     //3 - Logical Expression
        "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ_$", //4 - identificator
        "{}(); \n"+'\0',                                          //5 - special symbol & separator
                                                                  //6 - errors 
    };                                                            //7 - Reserved Word
    memset(cash, 6, sizeof(char)*SIZE_CASH_TABLE);
    FillTable(str, cash, NUM_CLASS_LEX-1);
    ListResWord.insert(pair<string, int> ("if",     0));
    ListResWord.insert(pair<string, int> ("else",   1));
    ListResWord.insert(pair<string, int> ("for",    2));
    ListResWord.insert(pair<string, int> ("in",     3));
    ListResWord.insert(pair<string, int> ("return", 4));
    ListResWord.insert(pair<string, int> ("with",   5));
}

int HandleCosnt         (char ch, int stat, filebuf  &file, string &str) { // class 1
    static char tab[8][5] = {
//      0-9 +/-  .  E   other
        {1, 99,  2, 99, 99}, // start
        {1,  8,  3,  5,  8}, // int
        {4,  8,  8, 99,  8}, // .*
        {4,  8,  8,  5,  8}, // *.
        {4,  9,  9,  5,  9}, // float
        {7,  6, 99, 99, 99}, // E
        {7, 99, 99, 99, 99}, // +/-
        {7,  9,  9,  9,  9}  // float
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

    if (stat > 7 && stat != 99) { 
        file.sputbackc(ch);
    } else {
        str += ch;
    }
    return stat;
}
int HandlerAssignment   (char ch, int stat, filebuf  &file, string &str) { //class 2
    if (ch == '=') {
        return 8;
    } else {
        char buff = file.sbumpc();
        if (buff == '=') {
            switch (ch) {
            case '+':
                return 9;
            case '-':
                return 10;
            case '*':
                return 11;
            case '/':
                return 12; 
            }
        } else {
            file.sputbackc(buff);
            str += ch;
            return 99;
        }
    }
}
int HandlerLogical      (char ch, int stat, filebuf  &file, string &str) { //class 3
    static char tab[3][3] = {
//        |   &  other
        { 1,  2, 99}, // start
        {10,  8,  8}, // || or |
        { 9, 11,  9}, // && or &
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
    if (stat == 8 || stat == 9) {
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
        return 8;
    }
}
int HandlerSpecialSymbol(char ch, int stat, filebuf  &file, string &str) { //class 5
    switch (ch) {
    case '{':
        return 8;
    case '}':
        return 9;
    case '(':
        return 10;
    case ')':
        return 11;
    case ':':
        return 12;
    default:
        return 13;
    }
}
int HandlerError        (char ch, int stat, filebuf  &file, string &str) {
    if (ch == ' ' || ch ==  '\n' || ch ==  -1 || ch ==  '{' || ch ==  '}' || ch ==  '(' || ch == ')' || ch == ';') {
        file.sputbackc(ch);
        return 8;
    } else {
        str += ch;
        return 99;
    }
}

void ProcessingStatus(int &stat, int &ClassLex, string &str, list<lexem> &lst) {
    if (stat == 99) { //if error class = ERR
        ClassLex = ERR;
    } else if (stat > 7) { // if stat > 7 lexem complited
        if (ClassLex == IDEN) { // if class = IDEN find reserved word
            unordered_map<string, int>::iterator it = ListResWord.find(str);
            if (it != ListResWord.end()) {
                stat = it->second;
                ClassLex = RES_W;
                str = "";
            }
        } 
        if (ClassLex != SP_SY || stat != 13) { // if symbol = ' ', '\n', '\0' then don't add in list lexem
            lexem buff;
            buff.id = ((ClassLex & 0xF) << 4) | (stat & 0x7);
            buff.str = str;
            lst.push_back(buff);       
        }
        str = "";
        ClassLex = UNK;
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
    int ClassLexems = UNK;
    int status = 0;
    char symbol = 0;
    char cash[SIZE_CASH_TABLE];
    list<lexem> LstLex;
    string buff = "";
    init(cash);
    int (*lexHandler[NUM_CLASS_LEX])(char ch, int stat, filebuf  &file, string &str) = 
        {HandleCosnt, HandlerAssignment, HandlerLogical, HandlerId, HandlerSpecialSymbol, HandlerError};    
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