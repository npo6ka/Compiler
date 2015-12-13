#include <iostream>
#include <fstream>
#include <unordered_map>
#include "lexical_analyzer.h"

unordered_map<string, int> ListResWord;

char* FillTable(const string str[], char *cash, const int num) { 
    for (int i = 0; i < num; i++) {
        for (int j = 0; j < (int)str[i]._Mysize; j++) {
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
    string str[SIZE_C-1] = {
        "=*+-/",                                                  //1 - Assignment operation
        "|&",                                                     //2 - Logical Expression
        "{}(); \n",                                               //3 - special symbol & separator
        "0123456789.",                                            //4 - const
        "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ_$", //5 - identificator
        "",                                                       //7 - Reserved Word
        "",
    };

    memset(cash, UNK_C, sizeof(char)*SIZE_CASH_TABLE);
    FillTable(str, cash, SIZE_C-1);
    ListResWord["if"] = IF;
    ListResWord["IF"] = IF;
    ListResWord["else"] = ELSE;
    ListResWord["ELSE"] = ELSE;
    ListResWord["for"] = FOR;
    ListResWord["FOR"] = FOR;
    ListResWord["in"] = IN;
    ListResWord["IN"] = IN;
    ListResWord["return"] = RETURN;
    ListResWord["RETURN"] = RETURN;
    ListResWord["with"] = WITH;
    ListResWord["WITH"] = WITH;
    /*ListResWord["int"] = INT;
    ListResWord["INT"] = INT;
    ListResWord["float"] = FLOAT;
    ListResWord["FLOAT"] = FLOAT;*/
}

lexem HandlerConst        (filebuf* inbuf, char *cash) {
    int shift = 10;
    int ERR_H = ERR + shift;
    int INT = C_INT + shift;
    int FLT = C_FLT + shift;
    static char tab[8][5] = {
//       0-9   +/-     .      E     other
        { 1,  ERR_H, 2,     ERR_H, ERR_H }, // start
        { 1,  INT,   3,     5,     INT   }, // int
        { 4,  ERR_H, ERR_H, ERR_H, ERR_H }, // .*
        { 4,  FLT,   FLT,   5,     FLT   }, // *.
        { 4,  FLT,   FLT,   5,     FLT   }, // float
        { 7,  6,     ERR_H, ERR_H, ERR_H }, // E
        { 7,  ERR_H, ERR_H, ERR_H, ERR_H }, // +/-
        { 7,  FLT,   FLT,   FLT,   FLT   }  // float
    }; 
    int stat = 0;
    string str = "";
    char ch;

    while (1) {
        ch = inbuf->sbumpc();
        if (ch >= '0' && ch <= '9') {
            stat = tab[abs(stat)][0];
        } else   if (ch == '+' || ch == '-') {
            stat = tab[abs(stat)][1];
        } else   if (ch == '.') {
            stat = tab[abs(stat)][2];
        } else   if (ch == 'E') {
            stat = tab[abs(stat)][3];
        } else {
            stat = tab[abs(stat)][4];
        }

        if (stat <= shift) {
            str += ch;
        } else {
            inbuf->sputbackc(ch);
            break;
        }
    } 
    stat -= shift;
    if (stat == ERR || stat < UNK) {
        return lexem(ERR, ERR_C, str);
    } else {
        return lexem((NumLex)stat, CONST, str);
    } 
}
lexem HandlerAssignment   (filebuf* inbuf, char *cash) {
    char ch = inbuf->sbumpc();

    if (ch == '=') {
        return lexem(ASIG, AS_OP, "");
    } else {
        char buff = inbuf->sbumpc();
        if (buff == '=') {
            switch (ch) {
            case '+':
                return lexem(SUM, AS_OP, "");
            case '-':
                return lexem(DIFF, AS_OP, "");
            case '*':
                return lexem(MULT, AS_OP, "");
            case '/':
                return lexem(DIVI, AS_OP, "");
            default:
                inbuf->sputbackc(buff);
                return lexem(ERR, ERR_C, string() + ch);
            }
        } else {
            inbuf->sputbackc(buff); 
            return lexem(ERR, ERR_C, string() + ch);
        }
    }
}
lexem HandlerLogical      (filebuf* inbuf, char *cash) { 
    char ch = inbuf->sbumpc();

    if (ch == '|') {
        ch = inbuf->sbumpc();
        if (ch == '|') {
            return lexem(LO_OR, LO_OP, "");
        } else {
            inbuf->sputbackc(ch);
            return lexem(BN_OR, LO_OP, "");
        }
    } else if (ch == '&') {
        ch = inbuf->sbumpc();
        if (ch == '&') {
            return lexem(LO_AND, LO_OP, "");
        } else {
            inbuf->sputbackc(ch);
            return lexem(BN_AND, LO_OP, "");
        }
    } else {
        return lexem(ERR, ERR_C, string() + ch);
    }
}
lexem HandlerId           (filebuf* inbuf, char *cash) {
    char ch = 0;
    string str;

    while (1) {
        if ((ch = inbuf->sbumpc()) != EOF && 
            (cash[ch] == IDEN || (cash[ch] == CONST && ch != '.'))) {
            str += ch;
        } else {
            inbuf->sputbackc(ch);
            unordered_map<string, int>::iterator it = ListResWord.find(str);
            if (it != ListResWord.end()) {
                return lexem((NumLex)it->second, RES_W, "");
            } else {
                return lexem(IND, IDEN, str);
            }
        }
    }
}
lexem HandlerSpecialSymbol(filebuf* inbuf, char *cash) { 
    char ch = inbuf->sbumpc();

    switch (ch) {
    case '{':
        return lexem(LCB, SP_SY, "");
    case '}':
        return lexem(RCB, SP_SY, "");
    case '(':
        return lexem(LF_PR, SP_SY, "");
    case ')':
        return lexem(RG_PR, SP_SY, "");
    case ';':
        return lexem(SEMIC, SP_SY, "");
    case '\n':
        return lexem(SEP_n, SP_SY, "");
    default:
        return lexem(SEPR, SP_SY, "");
    }
}
lexem HandlerUnknown      (filebuf* inbuf, char *cash){
    char ch = inbuf->sbumpc();
    string str; 
    str += ch;
    while ((ch = inbuf->sbumpc()) != EOF) {
        if (AssignClass(cash, ch) <= SP_SY) {
            inbuf->sputbackc(ch);
            return lexem(ERR, ERR_C, str);
        } else {
            str += ch;  
        }
    }
    return lexem(ERR, ERR_C, str);
}

void PrintLex(list<lexem> lst) {
    char *str[SIZE_C - 1] = {  "assigment operator", 
                          "logical operator  ", 
                          "special symbol    ", 
                          "constant          ", 
                          "identificator     ", 
                          "reserved word     "};
    char *output[AMOU_TR - 1] = {
        "",
        "=      ",  "+      ", "-      ", "*      ", "/      ",                                 //class Assignment operation
        "&      ",  "&&     ", "|      ", "||     ",                                            //class Logical Expression
        "{      ",  "}      ", "(      ", ")      ", ";      ",                              //class special symbol
        "", "",                                                                                 //class const
        "",                                                                                     //class indificator
        "if     ",  "else   ", "for    ", "in     ", "return ", "with   ",//"INT    ", "FLOAT  "//class reserved word
    };

    cout << "List lexem:" << endl;
    for (auto& it: lst) {
        cout <<  str[it._class - 1] << "   Lexem: " << output[it._id] + it._str << endl;
    }
}

void PrintErr(list<lexem> lst) {
    cout << "\n\n" << "Error lexem found:" << endl;
    for (auto& it: lst) {
        cout << "line: " << it._line << ":  \"" << it._str << "\"" << endl;
    }
}

bool lexical_analyzer(list<lexem> &LstLex) {
    fstream fs;
    char cash[SIZE_CASH_TABLE];
    char symbol;
    list<lexem> BufErr;
    int line = 1;
    init(cash);    
    fs.open("input.txt");
    if (!fs.is_open()) {
        cout <<  "Error opening file" << endl;
        return false;
    }
    filebuf* inbuf  = fs.rdbuf();
    lexem (*lexHandler[SIZE_C - 2])(filebuf* inbuf, char *cash) = 
        {HandlerUnknown, HandlerAssignment, HandlerLogical, HandlerSpecialSymbol, HandlerConst, HandlerId};

    while ((symbol = inbuf->sgetc()) != EOF) {
        lexem Lex = lexHandler[AssignClass(cash, inbuf->sgetc())] (inbuf, cash);
        if (Lex._class == ERR_C) {
            BufErr.push_back(lexem (ERR, ERR_C, Lex._str, line));
        } else if (Lex._class == SP_SY && Lex._id >= SEPR) {
            if (Lex._id == SEP_n) {
                line++;
            } //else don't add lexem in the list
        } else {
            LstLex.push_back(Lex);
        }  
    }

    fs.close();
    PrintLex(LstLex);
    if (BufErr.size()) {
        PrintErr(BufErr);
        return false;
    }
    return true;
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