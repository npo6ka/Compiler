#include <iostream>
#include <fstream>
#include <list>
#include <unordered_map>
#include "lexical_analyzer.h"

unordered_map<string, int> ListResWord;

char* FillTable(const string str[], char *cash, const int num) { 
    for (int i = 0; i < num; i++) {
        for (int j = 0; j < str[i]._Mysize; j++) {
            if (str[i][j] >= 0 && str[i][j] < 256 && str[i][j]) {
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
        return cash[ch];
    } else {
        return UNK_C;
    }
}

void init(char *cash) {
    string str[NUM_CLASS_LEX-1] = {
        "=*+-/",                                                  //1 - Assignment operation
        "|&",                                                     //2 - Logical Expression
        "{}(); \n",                                               //3 - special symbol & separator
        "0123456789.",                                            //4 - const
        "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ_$", //5 - identificator
    };                                                            //7 - Reserved Word

    memset(cash, UNK_C, sizeof(char)*SIZE_CASH_TABLE);
    FillTable(str, cash, NUM_CLASS_LEX-1);
    ListResWord.insert(pair<string, int> ("if",     IF    ));
    ListResWord.insert(pair<string, int> ("else",   ELSE  ));
    ListResWord.insert(pair<string, int> ("for",    FOR   ));
    ListResWord.insert(pair<string, int> ("in",     IN    ));
    ListResWord.insert(pair<string, int> ("return", RETURN));
    ListResWord.insert(pair<string, int> ("with",   WITH  ));
    ListResWord.insert(pair<string, int> ("int",    INT   ));
    ListResWord.insert(pair<string, int> ("float",  FLOAT ));
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

    while ((ch = inbuf->sbumpc()) != EOF) {
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
            break;
        }
    } 

    if (stat == ERR || stat < UNK) {
        return lexem(ERR, str);
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
                return lexem(DIVI, "");
            default:
                inbuf->sputbackc(buff);
                return lexem(ERR, string() + ch);
            }
        } else {
            inbuf->sputbackc(buff); 
            return lexem(ERR, string() + ch);
        }
    }
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
        return lexem(ERR, string() + ch);
    }
}
lexem HandlerId           (filebuf* inbuf, char *cash) {
    char ch = 0;
    string str;

    while ((ch = inbuf->sbumpc()) != EOF) {
        if (cash[ch] == IDEN || (cash[ch] == CONST && ch != '.')) {
            str += ch;
        } else {
            inbuf->sputbackc(ch);
            unordered_map<string, int>::iterator it = ListResWord.find(str);
            if (it != ListResWord.end()) {
                return lexem(it->second, "");
            } else {
                return lexem(IND, str);
            }
        }
    }
    return lexem(0,"");
}
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
    case '\n':
        return lexem(SEP_n, "");
    default:
        return lexem(SEPR, "");
    }
}
lexem HandlerUnknown      (filebuf* inbuf, char *cash){
    string str;
    char ch = inbuf->sbumpc();

    while ((ch = inbuf->sbumpc()) != EOF) {
        if (AssignClass(cash, ch) < SP_SY) {
            inbuf->sputbackc(ch);
            return lexem(ERR, str);
        } else {
            str += ch;  
        }
    } 
    return lexem(ERR, str);
}

bool PrintLex(list<lexem> lst) {
    char *str[ERR_C] = {  "assigment operator", 
                          "logical operator  ", 
                          "special symbol    ", 
                          "constant          ", 
                          "identificator     ", 
                          "reserved word     ", 
                          "Error lexem       "};
    char *output[7][8] = {
        {"=      ",  "+      ", "-      ", "*      ", "/      ", ""       , ""       , ""        },
        {"&      ",  "&&     ", "|      ", "||     ", ""       , ""       , ""       , ""        },
        {"{      ",  "}      ", "(      ", ")      ", ";      ", "sapce  ", "enter  ", ""        },
        {""       ,  ""       , ""       , ""       , ""       , ""       , ""       , ""        },
        {""       ,  ""       , ""       , ""       , ""       , ""       , ""       , ""        },
        {"if     ",  "else   ", "for    ", "in     ", "return ", "with   ", "INT    ", "FLOAT  " },
        {""       ,  ""       , ""       , ""       , ""       , ""       , ""       , ""        } 
	};
    list<lexem> BufErr;
    int buff;
    int line = 1;
    std::list<lexem>::iterator it;

    cout << "List lexem:" << endl;
    for (it = lst.begin(); it != lst.end(); ++it) {
        buff = ((it->_id & 0xF0) >> 4);
        if (buff == ERR_C) {
            BufErr.push_back(lexem(line, it->_str));
            it = --lst.erase(it);
        } else if (buff-- == SP_SY && (it->_id & 0xF) > 4) {
            if ((it->_id & 0xF) != 5) {
                line++;
            }
            it = --lst.erase(it);
        } else {
            cout <<  str[buff] << "   Lexem: " << output[buff][it->_id & 0xF] + it->_str << endl;
        }
    }
    if (BufErr.size()) {
        cout << "\n\n" << "Error lexem found:" << endl;
        for (auto& it: BufErr) {
            cout << "line: " << (int)it._id << ":  \"" << it._str << "\"" << endl;
        }
        return 0;
    } else {
        return 1;
    }
}

int main() {
    fstream fs;
    char cash[SIZE_CASH_TABLE];
    char symbol;
    list<lexem> LstLex;
    init(cash);    
    fs.open("input.txt");
    if (!fs.is_open()) {
        cout <<  "Error opening file" << endl;
        return -1;
    }
    filebuf* inbuf  = fs.rdbuf();
    lexem (*lexHandler[NUM_CLASS_LEX])(filebuf* inbuf, char *cash) = 
        {HandlerUnknown, HandlerAssignment, HandlerLogical, HandlerSpecialSymbol, HandlerConst, HandlerId};

    while ((symbol = inbuf->sgetc()) != EOF) {
        LstLex.push_back(
            lexHandler[AssignClass(cash, inbuf->sgetc())] (inbuf, cash));
    }
   
    fs.close();
    return PrintLex(LstLex);
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