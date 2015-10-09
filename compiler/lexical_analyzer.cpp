#include <iostream>
#include <fstream>
#include <string>
#include <list>
#include <unordered_map>

using namespace std;

unordered_map<string, int> mymap;


#define NUM_CLASS_LEX 5
#define SIZE_CASH_TABLE 256

struct lexem {
    char id;
    string str;
};

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

void AssignClass(const char* const cash, const char c, int &ClassLex) {
    if (ClassLex == 0 || ClassLex == -1) {
        ClassLex = cash[c];
    }
}

int HandleCosnt         (char ch, int stat, filebuf  &file, string &str) { // class 1
    static char tab[5][8] = {
//       st int  .*  *. flt   E +/- float
        { 1,  1,  4,  4,  4,  7,  7,  7}, // 0 - 9
        {99,  8,  8,  8,  9,  6, 99,  9}, // +/-
        { 2,  3,  8,  8,  9, 99, 99,  9}, // .
        {99,  5, 99,  5,  5, 99, 99,  9}, // E
        {99,  8,  8,  8,  9, 99, 99,  9}, // other
    };
    if (ch >= '0' && ch <= '9') {
        stat = tab[0][stat];
    } else   if (ch == '+' || ch == '-') {
        stat = tab[1][stat];
    } else   if (ch == '.') {
        stat = tab[2][stat];
    } else   if (ch == 'E') {
        stat = tab[3][stat];
    } else {
        stat = tab[4][stat];
    }

    if (stat > 7 && stat != 99) { 
        file.sputbackc(ch);
    } else {
        str += ch;
    }
    return stat;
}
int HandlerAssignment   (char ch, int stat, filebuf  &file, string &str) { //class 2
    if (stat == 1) {
        switch (ch) {
        case '+':
            str += '+';
            return 9;
            break;
        case '-':
            str += '-';
            return 10;
            break;
        case '*':
            str += '*';
            return 11;
            break;
        case '/':
            str += '/';
            return 12;
            break;
        default:
            file.sputbackc(ch);
            return 8;
            break;
        }
    } else if (ch == '=') {
        str += '=';
        return 1;
    }
}
int HandlerLogical      (char ch, int stat, filebuf  &file, string &str) { //class 3
    if (stat == 0) {
        if (ch =='|') {
            str += ch;
            return 1;
        } else if (ch == '&') {
            str += ch;
            return 2;
        } else {
            return 99;
        }
    } else if (stat == 1) {
        if (ch =='|') {
            str += ch;
            return 10;
        } else {
            file.sputbackc(ch);
            return 8;
        }
    } else if (stat == 2) {
        if (ch =='&') {
            str += ch;
            return 11;
        } else {
            file.sputbackc(ch);
            return 9;
        }
    }
}
int HandlerId           (char ch, int stat, filebuf  &file, string &str) { //class 4
    if ((ch >= 'a' && ch <= 'z') || (ch >= 'A' && ch <= 'Z') || ch == '$' || ch == '_') {
        str += ch;
        return 1;
    } else {
        file.sputbackc(ch);
        return 8;
    }
}
int HandlerSpecialSymbol(char ch, int stat, filebuf  &file, string &str) { //class 5
    if (ch == ' ' || ch == '\n' ) {
        str += ch;
        return 8;
    } else {
        str += ch;
        return 9;
    }
}
int HandlerSeparator    (char ch, int stat, filebuf  &file, string &str) { //class 6
    if (ch == ' ' || ch ==  '\n' || ch ==  '\0' || ch ==  '{' || ch ==  '}' || ch ==  '(' || ch == ')') {
        return 8;
    } else {
        str += ch;
        return 1;
    }
}

int Handler(char ch, int ClassLex, int stat, filebuf  &file, string &str) {
    switch (ClassLex) {
    case -1:  
        str += ch;
        return 99;
        break;
    case 1:
        return HandleCosnt(ch, stat, file, str);
        break;
    case 2:
        return HandlerAssignment(ch, stat, file, str);
        break;
    case 3:
        return HandlerLogical(ch, stat, file, str);
        break;
    case 4:
        return  HandlerId(ch, stat, file, str);
        break;
    case 5:
        return HandlerSpecialSymbol(ch, stat, file, str);
        break;
    case 6:
        return HandlerSeparator(ch, stat, file, str);
        break;
    default:
        cout << "Error in ClassLexem" << endl;
    }
    return stat;

}

void ProcessingStatus(int &stat, int &ClassLex, string &str, list<lexem> &lst) {
    if (stat == 99) {
        ClassLex = 6;
    } else if (stat > 7) {
        if (ClassLex == 4 ) {
            unordered_map<string, int>::iterator it = mymap.find(str);
            if (it != mymap.end()) {
                stat = it->second;
                ClassLex = 7;
            }
        } 
        cout << "lexem found:" << ClassLex << "    '" <<  str << "'  ch = ";
        lexem buff;
        buff.id = (ClassLex & 0xF);
        buff.id = buff.id << 4;
        buff.id = buff.id | (stat & 0x7);
        buff.str = str;
        lst.push_back(buff);
        cout << (int)buff.id << endl;
        str = "";
        ClassLex = 0;
        stat = 0;
    } 
}

void PrintLex(list<lexem> lst) {
    for (auto& it: lst) {
        cout << "Lexem class: " << ((it.id & 0xF0) >> 4) << "   Lexem: " << (it.id & 0xF) << "   str: " << it.str << endl;
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
    list<lexem> LstLex;
    string buff = "";
    string str[NUM_CLASS_LEX] = {                                 //0 - unknown
        "0123456789.",                                            //1 - const
        "=*+-/",                                                  //2 - Assignment operation
        "|&",                                                     //3 - Logical Expression
        "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ_$", //4 - identificator and Reserved Word
        "{}(); \n",                                               //5 - special symbol
                                                                  //6 - errors  
    };
    mymap.insert(pair<string, int> ("if",     0));
    mymap.insert(pair<string, int> ("else",   1));
    mymap.insert(pair<string, int> ("for",    2));
    mymap.insert(pair<string, int> ("in",     3));
    mymap.insert(pair<string, int> ("return", 4));
    mymap.insert(pair<string, int> ("with",   5));
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
        AssignClass(cash, symbol, ClassLexems);
        status = Handler(symbol, ClassLexems, status, *inbuf, buff);
        ProcessingStatus(status, ClassLexems, buff, LstLex);
        //cout << symbol << " ";
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