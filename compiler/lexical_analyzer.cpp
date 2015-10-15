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
    AS_OP = 1,
    LO_OP = 2,
    SP_SY = 3,
    CONST = 4,
    IDEN  = 5,
    ERR_C = 6,
    RES_W = 7,
};

enum {
    UNK    = 0xF + (UNK_C << 4), 
	
    //class Assignment operation
    ASIG   = 0x0 + (AS_OP << 4),
    SUM    = 0x1 + (AS_OP << 4),
    DIFF   = 0x2 + (AS_OP << 4),
    MULT   = 0x3 + (AS_OP << 4),
    DIVI   = 0x4 + (AS_OP << 4),

    //class Logical Expression
    BN_AND = 0x0 + (LO_OP << 4),
    LO_AND = 0x1 + (LO_OP << 4),
    BN_OR  = 0x2 + (LO_OP << 4),
    LO_OR  = 0x3 + (LO_OP << 4),

    //class special symbol
    LFB    = 0x0 + (SP_SY << 4), //left curly brace
    RFB    = 0x1 + (SP_SY << 4), //right
    LF_PR  = 0x2 + (SP_SY << 4), //left parenthesis
    RG_PR  = 0x3 + (SP_SY << 4),
    SEMIC  = 0x4 + (SP_SY << 4), //semicolon

    //class const
    C_INT  = 0x0 + (CONST << 4),
    C_FLT  = 0x1 + (CONST << 4),

    //class indificator
    IND    = 0x0 + (IDEN << 4),

    //class Error
    ERR    = 0x0 + (ERR_C << 4),
    EX_ERR = 0x1 + (ERR_C << 4), //end error
    EXIT   = 0x2 + (ERR_C << 4), //not add list

    //class reserved word
    IF     = 0x0 + (RES_W << 4),
    ELSE   = 0x1 + (RES_W << 4),
    FOR    = 0x2 + (RES_W << 4),
    IN     = 0x3 + (RES_W << 4),
    RETURN = 0x4 + (RES_W << 4),
    WITH   = 0x5 + (RES_W << 4),
    INT    = 0x6 + (RES_W << 4),
    FLOAT  = 0x7 + (RES_W << 4),
};

class lexem {
public:
    char _id;
    string _str;
    lexem(int i, string str): _id(ERR), _str("") {
        _id = i;
        _str = str;
    }
    ~lexem() {}
};

//????? default val in cash table
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

int AssignClass(const char* const cash, const char ch) {
    if (ch != EOF){
        return cash[ch]-1;
    } else {
        return SP_SY;
    }
}

void init(char *cash) {
    string str[NUM_CLASS_LEX-1] = {
        "=*+-/",                                                  //1 - Assignment operation
        "|&",                                                     //2 - Logical Expression
        "{}(); \n",                                               //3 - special symbol & separator
        "0123456789.",                                            //4 - const
        "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ_$", //5 - identificator
                                                                  //6 - errors 
    };                                                            //7 - Reserved Word
    
    memset(cash, SP_SY, sizeof(char)*SIZE_CASH_TABLE);
    FillTable(str, cash, NUM_CLASS_LEX-1);
    ListResWord.insert(pair<string, int> ("if",     IF    ));
    ListResWord.insert(pair<string, int> ("else",   ELSE  ));
    ListResWord.insert(pair<string, int> ("for",    FOR   ));
    ListResWord.insert(pair<string, int> ("in",     IN    ));
    ListResWord.insert(pair<string, int> ("return", RETURN));
    ListResWord.insert(pair<string, int> ("with",   WITH  ));
    ListResWord.insert(pair<string, int> ("int",   INT  ));
    ListResWord.insert(pair<string, int> ("float",   FLOAT  ));
}

lexem HandlerError        (filebuf* inbuf, char *cash, string str) {
    char ch = inbuf->sbumpc();
    do {
        str += ch;
        ch = inbuf->sbumpc();   
    } while (AssignClass(cash, ch) >= SP_SY && ch != EOF);
    inbuf->sputbackc(ch);
    return lexem(ERR, str);
}
lexem HandlerConst        (filebuf* inbuf, char *cash) {
    static char tab[8][5] = {
//      0-9   +/-     .      E     other
        { 1,  ERR,   2,     ERR,   ERR   }, // start
        { 1,  C_INT, 3,     5,     C_INT }, // int
        { 4,  ERR,   ERR,   ERR,   ERR   }, // .*
        { 4,  C_FLT, C_FLT, 5,     C_FLT }, // *.
        { 4,  C_FLT, C_FLT, 5,     C_FLT }, // float
        { 7,  6,     ERR,   ERR,   ERR   }, // E
        { 7,  ERR,   ERR,   ERR,   ERR   }, // +/-
        { 7,  C_FLT, C_FLT, C_FLT, C_FLT }  // float
    }; 
    int stat = 0;
    string str = "";
    char ch;

    do {
        ch = inbuf->sbumpc();       
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

        if ((stat & 0xF0) == UNK_C) {
            str += ch;
        } else {
            inbuf->sputbackc(ch);
        }
    } while ((stat & 0xF0) == UNK_C);

    if (stat == ERR) {
        return HandlerError (inbuf, cash, str);
    } else {
        return lexem(stat, str);
    } 
}
lexem HandlerAssignment   (filebuf* inbuf, char *cash) {
    char ch = inbuf->sbumpc();
    if (ch == '=') {
        return lexem(ASIG, "");
    } else {
        char buff = inbuf->sbumpc();
        if (buff == '=') {
            switch (ch) {
            case '+':
                return lexem(SUM, "");
            case '-':
                return lexem(DIFF, "");
            case '*':
                return lexem(MULT, "");
            case '/':
                return lexem( DIVI, ""); 
            }
        } else {
            inbuf->sputbackc(buff);
            string str;
            str += ch;
            return HandlerError (inbuf, cash, str);
        }
    }
    return lexem(1,"");
}
lexem HandlerLogical      (filebuf* inbuf, char *cash) { 
    char ch = inbuf->sbumpc();

    if (ch == '|') {
        ch = inbuf->sbumpc();
        if (ch == '|') {
            return lexem(LO_OR, "");
        } else {
            inbuf->sputbackc(ch);
            return lexem(BN_OR, "");
        }
    } else if (ch == '&') {
        ch = inbuf->sbumpc();
        if (ch == '&') {
            return lexem(LO_AND, "");
        } else {
            inbuf->sputbackc(ch);
            return lexem(BN_AND, "");
        }
    } else {
        inbuf->sputbackc(ch);
        return HandlerError(inbuf, cash, "");
    }
}
lexem HandlerId           (filebuf* inbuf, char *cash) {
    char ch = 0;
    string str;
    while (ch != EOF) {
        ch = inbuf->sbumpc();
        if (cash[ch] == IDEN || (cash[ch] == CONST && ch != '.')) {
            str += ch;
        } else {
            inbuf->sputbackc(ch);
            return lexem(IND, str);
        }
    }
    return lexem(0,"");
}
//????????????????????????? ERROR where ch = ' ' 
lexem HandlerSpecialSymbol(filebuf* inbuf, char *cash) { 
    char ch = inbuf->sbumpc();
    switch (ch) {
    case '{':
        return lexem(LFB, "");
    case '}':
        return lexem(RFB, "");
    case '(':
        return lexem(LF_PR, "");
    case ')':
        return lexem(RG_PR, "");
    case ';':
        return lexem(SEMIC, "");
    default:
        return HandlerError(inbuf, cash, "");
    }
}

void ProcessingStatus(lexem compLex, list<lexem> &lst) {
/*    if (stat == ERR) { //if error class = ERR
        ClassLex = ERR_C;
    } else if ((stat & 0xF0) != UNK_C) { // if stat > 7 lexem complited
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
    } */
}

void PrintLex(list<lexem> lst) {
    char *str[RES_W] = {  "assigment operator", 
                          "logical operator  ", 
                          "special symbol    ", 
                          "constant          ", 
                          "identificator     ", 
                          "Error lexem       ", 
                          "reserved word     "};
    char *output[7][8] = {
        {"=      ",  "+      ", "-      ", "*      ", "/      ", ""       , ""       , ""        },
        {"&      ",  "&&     ", "|      ", "||     ", ""       , ""       , ""       , ""        },
        {"{      ",  "}      ", "(      ", ")      ", ";      ", ""       , ""       , ""        },
        {"int    ",  "float  ", ""       , ""       , ""       , ""       , ""       , ""        },		
        {"       ",  ""       , ""       , ""       , ""       , ""       , ""       , ""        },
        {"       ",  ""       , ""       , ""       , ""       , ""       , ""       , ""        },
        {"if     ",  "else   ", "for    ", "in     ", "return ", "with   ", "INT    ", "FLOAT  " }
	};

    for (auto& it: lst) {
        cout <<  str[((it._id & 0xF0) >> 4)-1] << "   Lexem: " 
             << output[((it._id & 0xF0) >> 4)-1][it._id & 0xF] << "   str: " << it._str << endl;
    }
}

int main() {
    fstream fs;
    char symbol = 0;
    char cash[SIZE_CASH_TABLE];
    list<lexem> LstLex;
    init(cash);    
    fs.open("input.txt");
    if (!fs.is_open()) {
        cout <<  "Error opening file" << endl;
        return -1;
    }
    string str = "";
    char ch = 0;
    str += ch;
    str += 't';
    filebuf* inbuf  = fs.rdbuf();
    lexem (*lexHandler[NUM_CLASS_LEX-1])(filebuf* inbuf, char *cash) = 
        {HandlerAssignment, HandlerLogical, HandlerSpecialSymbol, HandlerConst, HandlerId};

    do {
        symbol = inbuf->sgetc();
        LstLex.push_back(lexHandler[AssignClass(cash, symbol)](inbuf, cash));
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
*/