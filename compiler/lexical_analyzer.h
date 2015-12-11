#pragma once
#include <string>
#include <list>

using namespace std;

#define NUM_CLASS_LEX 6
#define SIZE_CASH_TABLE 256

enum ClassLex 
{
    UNK_C,
    AS_OP,
    LO_OP,
    SP_SY,
    CONST,
    IDEN,
    RES_W,
    ERR_C,
    SIZE_C
};

enum NumLex 
{
    UNK,

    //class Assignment operation
    ASIG,
    SUM,
    DIFF,
    MULT,
    DIVI,

    //class Logical Expression
    BN_AND,
    LO_AND,
    BN_OR,
    LO_OR,

    //class special symbol
    LCB,    //left curly brace
    RCB,    //right
    LF_PR,  //left parenthesis
    RG_PR,
    SEMIC,  //semicolon
    SEPR,
    SEP_n,

    //class const
    C_INT,
    C_FLT,

    //class indificator
    IND,

    //class reserved word
    IF,
    ELSE,
    FOR,
    IN,
    RETURN,
    WITH,
    //INT,
    //FLOAT,

    //class Error
    ERR,
    AMOU_L,
};

class lexem 
{
public:
    NumLex _id;
    ClassLex _class;
    string _str;
    int _line;

    lexem(NumLex id, ClassLex cl, string str, int line): _id(id), _class(cl), _str(str), _line(line) {}
    lexem(NumLex id, ClassLex cl, string str): _id(id), _class(cl), _str(str), _line(0) {}
    ~lexem() {}
};

bool lexical_analyzer(list<lexem> &);

