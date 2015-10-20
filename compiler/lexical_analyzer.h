#ifndef LEXIC
#include <string>

using namespace std;

#define NUM_CLASS_LEX 6
#define SIZE_CASH_TABLE 256

class lexem {
public:
    char _id;
    string _str;
    lexem(int i, string str): _id(i), _str(str) {}
    ~lexem() {}
};

enum {
    UNK_C = 0,
    AS_OP = 1,
    LO_OP = 2,
    SP_SY = 3,
    CONST = 4,
    IDEN  = 5,
    RES_W = 6,
    ERR_C = 7,
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
    SEPR   = 0x5 + (SP_SY << 4),
    SEP_n  = 0x6 + (SP_SY << 4),

    //class const
    C_INT  = 0x0 + (CONST << 4),
    C_FLT  = 0x1 + (CONST << 4),

    //class indificator
    IND    = 0x0 + (IDEN << 4),

    //class reserved word
    IF     = 0x0 + (RES_W << 4),
    ELSE   = 0x1 + (RES_W << 4),
    FOR    = 0x2 + (RES_W << 4),
    IN     = 0x3 + (RES_W << 4),
    RETURN = 0x4 + (RES_W << 4),
    WITH   = 0x5 + (RES_W << 4),
    INT    = 0x6 + (RES_W << 4),
    FLOAT  = 0x7 + (RES_W << 4),

    //class Error
    ERR    = 0x0 + (ERR_C << 4),
};
#endif // !LEXIC