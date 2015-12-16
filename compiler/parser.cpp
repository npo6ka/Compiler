#include "parser.h"
#include <iostream>
#include <fstream>

ofstream fout;

enum RetStat {
    CRITERR = 0,
    REPAERR,
    SUCCESS,
    EXIT,
};

void handlerError(Lex &cur, StLx &st) {
    while (cur->_id != SEMIC && cur->_id != RCB && cur->_id != ERR) cur++;
    if (cur->_id != ERR) {
        while (cur->_id == SEMIC || cur->_id == RCB) cur++;
    }
    while (st.size() != 2) st.pop();
    //if (st.top() != SL) st.pop();
    fout << "\n************************* repair ERROR *************************\n" << endl;
}

string print(NumLex num) {
    static string str[54] = {
        "", "=", "+=", "-=", "*=", "/=",
        "&", "&&", "|", "||",
        "{", "}", "(", ")", ";",
        "INT", "FLOAT", "IND",
        "if", "else", "for", "in", "return", "with", 
        "-|",
        //non-term
        "<Program>", "<Statement List>", "<Statement>", "<Block>", "<If Statement>", "<If Else Statement>", "<Iteration Statement>", "<Return Statement>", "<Return Expression>", "<With Statement>", 
        "<For Expression>", "<Part_0 For Expression>", "<Part_1 For Expression>", "<Part_2 For Expression>", "<Part_3 For Expression>", "<Part_4 For Expression>", 
        "<Expression Statement>", "<Expression in Paranthesis>", "<Left Paranthesis And Expression>", 
        "<Expression>", "<Assignment Expression>", "<Assignment Operator>", 
        "<Logical Or Expression>", "<Logical And Expression>", "<Bitwise Or Expression>", "<Bitwise And Expression>", 
        "<Equality Expression>", "<Argument>", "-v-"
    };
    return str[num];
}

void prLx(StLx buf) {
    while (buf.size()) {
        fout << print(buf.top()) << " ";
        buf.pop();
    }
    fout << endl;
}

// shift
RetStat FSH(Lex &cur, StLx &st) {
    st.push(cur->_id);
    cur++;
    return SUCCESS;
}

// asig | sum | diff | mult | divi -> OP
RetStat FRE01(Lex &cur, StLx &st) {
    st.pop();
    st.push(OP);
    return SUCCESS;
}

// int | float | ind -> AR
RetStat FRE02(Lex &cur, StLx &st) {
    st.pop();
    st.push(AR);
    return SUCCESS;
}

// AR -> EE
RetStat FRE03(Lex &cur, StLx &st) {
    st.pop();
    st.push(EE);
    return SUCCESS;
}

// BAE '&' EE | EE -> BAE
RetStat FRE04(Lex &cur, StLx &st) {
    st.pop();
    if (st.top() == BN_AND) {
        st.pop();
        st.pop();
    }
    st.push(BAE);
    return SUCCESS;
}

// BAE '|' BOE | BAE -> BOE
RetStat FRE05(Lex &cur, StLx &st) {
    st.pop();
    if (st.top() == BN_OR) {
        st.pop();
        st.pop();
    }
    st.push(BOE);
    return SUCCESS;
}

// LAE '&&' BOE | BOE -> LAE
RetStat FRE06(Lex &cur, StLx &st) {
    st.pop();
    if (st.top() == LO_AND) {
        st.pop();
        st.pop();
    }
    st.push(LAE);
    return SUCCESS;
}

// LOE '||' LAE | LAE -> LOE
RetStat FRE07(Lex &cur, StLx &st) {
    st.pop();
    if (st.top() == LO_OR) {
        st.pop();
        st.pop();
    }
    st.push(LOE);
    return SUCCESS;
}

//  LOE -> AE
RetStat FRE08(Lex &cur, StLx &st) {
    st.pop();
    st.push(AE);
    return SUCCESS;
}

// ID AO AE -> AE | AE -> EX
RetStat FRE09(Lex &cur, StLx &st) {
    st.pop(); 
    if (st.top() == OP) {
        st.pop();
        st.pop();
        st.push(AE);
    } else {
        st.push(EX);
    }
    return SUCCESS;
}

// EX -> ST | RET EX -> RE | ( ID IN EX -> P0FE | P2FE EX -> P1EF | P4FE EX -> P3FE | ( EX 
RetStat FRE10(Lex &cur, StLx &st) {
    st.pop();
    switch (st.top()) {
    case LF_PR:
        st.pop();
        st.push(LPAE);
        break;
    case P4FE:
        st.pop();
        st.push(P3FE);
        break;
    case P2FE:
        st.pop();
        st.push(P1FE);
        break;
    case IN:
        st.pop();
        st.pop();
        st.pop();
        st.push(P0FE);
        break;
    case RETURN:
        st.pop();
        st.push(RE);
        break;
    default:
        st.push(ST);
    }
    return SUCCESS;
}

// if ES -> IS | WITH ES -> WS
RetStat FRE11(Lex &cur, StLx &st) {
    st.pop();
    switch(st.top()) {
    case IF:
        st.pop();
        st.push(IS);
        break;
    case WITH:
        st.pop();
        st.push(WS);
        break;
    default:
        cout << "ERROR program" << endl;
        return CRITERR;
    }
    return SUCCESS;
}

// RS -> ST
RetStat FRE12(Lex &cur, StLx &st) {
    st.pop();
    st.push(ST);
    return SUCCESS;
}

// ITS -> ST
RetStat FRE13(Lex &cur, StLx &st) {
    st.pop();
    st.push(ST);
    return SUCCESS;
}

// IES -> ST
RetStat FRE14(Lex &cur, StLx &st) {
    st.pop();
    st.push(ST);
    return SUCCESS;
}

// IS -> ST
RetStat FRE15(Lex &cur, StLx &st) {
    st.pop();
    st.push(ST);
    return SUCCESS;
}

// BL -> ST
RetStat FRE16(Lex &cur, StLx &st) {
    st.pop();
    st.push(ST);
    return SUCCESS;
}

// ST -> SL | SL ST -> SL | IS ELSE ST -> IES | FOR FE ST -> IS | EIP ST -> ES
RetStat FRE17(Lex &cur, StLx &st) {
    st.pop();
    switch(st.top()) {
    case SL:
        st.pop();
        st.push(SL);
        break;
    case ELSE:
        st.pop();
        st.pop();
        st.push(IES);
        break;
    case FE:
        st.pop();
        st.pop();
        st.push(ITS);
        break;
    case EIP:
        st.pop();
        st.push(ES);
        break;
    default:
        st.push(SL);
    }
    return SUCCESS;
}

// SL -> PR
RetStat FRE18(Lex &cur, StLx &st) {
    st.pop();
    st.push(PR);
    return SUCCESS;
}

// LCB RCB -> BL | LCB SL RCB -> BL
RetStat FRE19(Lex &cur, StLx &st) {
    st.pop();
    switch (st.top()) {
    case LCB:
        st.pop();
        st.push(BL);
        break;
    case SL:
        st.pop();
        st.pop();
        st.push(BL);
    }
    return SUCCESS;
}

// P0FE RG_PR -> FE | P1FE RG_PR -> FE | LPAE RG_PR -> EIP
RetStat FRE20(Lex &cur, StLx &st) {
    st.pop();
    switch (st.top()) {
    case P0FE:
        st.pop();
        st.push(FE);
        break;
    case P1FE:
        st.pop();
        st.push(FE);
        break;
    case LPAE:
        st.pop();
        st.push(EIP);
    }
    return SUCCESS;
}

// WS SEMIC -> ST | RET SEMIC -> RS | RE SEMIC -> RS | P3FE SEMIC -> P2FE | LPAE SEMIC -> P4FE
RetStat FRE21(Lex &cur, StLx &st) {
    st.pop();
    switch (st.top()) {
    case WS:
        st.pop();
        st.push(ST);
        break;
    case RETURN:
        st.pop();
        st.push(RS);
        break;
    case RE:
        st.pop();
        st.push(RS);
        break;
    case P3FE:
        st.pop();
        st.push(P2FE);
        break;
    case LPAE:
        st.pop();
        st.push(P4FE);
    }
    return SUCCESS;
}

//Error: need Argument
RetStat FE01(Lex &cur, StLx &st) {
    cout << "syntax error: expected 'Argument' in line " << cur->_line << endl;
    return REPAERR;
}

//Error: need Statement
RetStat FE02(Lex &cur, StLx &st) {
    cout << "syntax error: expected 'Statement' in line " << cur->_line << endl;
    return REPAERR;
}

//Error: need Expression
RetStat FE03(Lex &cur, StLx &st) {
    cout << "syntax error: expected 'Expression' in line " << cur->_line << endl;
    return REPAERR;
}

//Error: incorrect Statement
RetStat FE04(Lex &cur, StLx &st) {
    cout << "syntax error: incorrect Statement in line " << cur->_line << endl;
    return REPAERR;
}

//Error: incorrect Expression
RetStat FE05(Lex &cur, StLx &st) {
    cout << "syntax error: incorrect Expression in line " << cur->_line << endl;
    return REPAERR;
}

//Error: premature end of the program
RetStat FE06(Lex &cur, StLx &st) {
    cout << "syntax error: premature end of the program" << endl;
    return CRITERR;
}


//Error: excess characters 
RetStat FE07(Lex &cur, StLx &st) {
    cout << "syntax error: excess characters: line " << cur->_line << " terminal: " << print(cur->_id) << endl;
    return REPAERR;
}

//Error: need right parenthesis
RetStat FE08(Lex &cur, StLx &st) {
    cout << "syntax error: expected ')' in line " << cur->_line << endl;
    return REPAERR;
}

//Error: need semicolon
RetStat FE09(Lex &cur, StLx &st) {
    cout << "syntax error: expected ';' in line " << cur->_line << endl;
    return REPAERR;
}

//Error: need right parenthesis or semicolon
RetStat FE10(Lex &cur, StLx &st) {
    cout << "syntax error: expected ')' or ';' in line " << cur->_line << endl;
    return REPAERR;
}

//Error: need left parenthesis
RetStat FE11(Lex &cur, StLx &st) {
    cout << "syntax error: expected '(' in line " << cur->_line << endl;
    return REPAERR;
}

//Critical Error
RetStat FCR(Lex &cur, StLx &st) {
    cout << "Critical Error :(" << endl;
    return CRITERR;
}

// 
RetStat FEXIT(Lex &cur, StLx &st) {
    return EXIT;
}

bool parser(list<lexem> LstLex) {
    RetStat (*table[53][24])(Lex &, StLx &) = {
//         ASIG   SUM    DIFF   MULT   DIVI   BN_AND LO_AND BN_OR  LO_OR  LCB    RCB    LF_PR  RG_PR  SEMIC  C_INT  C_FLT  IND    IF     ELSE   FOR    IN    RETURN  WITH   -|
/*ASIG  */{FE01 , FE01 , FE01 , FE01 , FE01 , FE01 , FE01 , FE01 , FE01 , FE01 , FE01 , FE01 , FE01 , FE01 , FRE01, FRE01, FRE01, FE01 , FE01 , FE01 , FE01 , FE01 , FE01 , FE06 }, //++ +
/*SUM   */{FE01 , FE01 , FE01 , FE01 , FE01 , FE01 , FE01 , FE01 , FE01 , FE01 , FE01 , FE01 , FE01 , FE01 , FRE01, FRE01, FRE01, FE01 , FE01 , FE01 , FE01 , FE01 , FE01 , FE06 }, //++ +
/*DIFF  */{FE01 , FE01 , FE01 , FE01 , FE01 , FE01 , FE01 , FE01 , FE01 , FE01 , FE01 , FE01 , FE01 , FE01 , FRE01, FRE01, FRE01, FE01 , FE01 , FE01 , FE01 , FE01 , FE01 , FE06 }, //++ +
/*MULT  */{FE01 , FE01 , FE01 , FE01 , FE01 , FE01 , FE01 , FE01 , FE01 , FE01 , FE01 , FE01 , FE01 , FE01 , FRE01, FRE01, FRE01, FE01 , FE01 , FE01 , FE01 , FE01 , FE01 , FE06 }, //++ +
/*DIVI  */{FE01 , FE01 , FE01 , FE01 , FE01 , FE01 , FE01 , FE01 , FE01 , FE01 , FE01 , FE01 , FE01 , FE01 , FRE01, FRE01, FRE01, FE01 , FE01 , FE01 , FE01 , FE01 , FE01 , FE06 }, //++ +
/*BN_AND*/{FE01 , FE01 , FE01 , FE01 , FE01 , FE01 , FE01 , FE01 , FE01 , FE01 , FE01 , FE01 , FE01 , FE01 , FSH  , FSH  , FSH  , FE01 , FE01 , FE01 , FE01 , FE01 , FE01 , FE06 }, //++ +
/*LO_AND*/{FE01 , FE01 , FE01 , FE01 , FE01 , FE01 , FE01 , FE01 , FE01 , FE01 , FE01 , FE01 , FE01 , FE01 , FSH  , FSH  , FSH  , FE01 , FE01 , FE01 , FE01 , FE01 , FE01 , FE06 }, //++ +
/*BN_OR */{FE01 , FE01 , FE01 , FE01 , FE01 , FE01 , FE01 , FE01 , FE01 , FE01 , FE01 , FE01 , FE01 , FE01 , FSH  , FSH  , FSH  , FE01 , FE01 , FE01 , FE01 , FE01 , FE01 , FE06 }, //++ +
/*LO_OR */{FE01 , FE01 , FE01 , FE01 , FE01 , FE01 , FE01 , FE01 , FE01 , FE01 , FE01 , FE01 , FE01 , FE01 , FSH  , FSH  , FSH  , FE01 , FE01 , FE01 , FE01 , FE01 , FE01 , FE06 }, //++ +
/*LCB   */{FE02 , FE02 , FE02 , FE02 , FE02 , FE02 , FE02 , FE02 , FE02 , FSH  , FSH  , FE02 , FE02 , FE02 , FSH  , FSH  , FSH  , FSH  , FE02 , FSH  , FE02 , FSH  , FSH  , FE06 }, //++ +
/*RCB   */{FE02 , FE02 , FE02 , FE02 , FE02 , FE02 , FE02 , FE02 , FE02 , FRE19, FRE19, FE02 , FE02 , FRE19, FRE19, FRE19, FRE19, FRE19, FRE19, FRE19, FE02 , FRE19, FRE19, FRE19}, //++ +
/*LF_PR */{FE03 , FE03 , FE03 , FE03 , FE03 , FE03 , FE03 , FE03 , FE03 , FE03 , FE03 , FE03 , FE03 , FE03 , FSH  , FSH  , FSH  , FE03 , FE03 , FE03 , FE03 , FE03 , FE03 , FE03 }, //++ +
/*RG_PR */{FE02 , FE02 , FE02 , FE02 , FE02 , FE02 , FE02 , FE02 , FE02 , FRE20, FE02 , FE02 , FE02 , FE02 , FRE20, FRE20, FRE20, FRE20, FE02 , FRE20, FE02 , FRE20, FRE20, FE06 }, //++ +
/*SEMIC */{FE04 , FE04 , FE04 , FE04 , FE04 , FE04 , FE04 , FE04 , FE04 , FRE21, FRE21, FE04 , FE04 , FRE21, FRE21, FRE21, FRE21, FRE21, FRE21, FRE21, FE04 , FRE21, FRE21, FRE21}, //++ +
/*C_INT */{FE05 , FE05 , FE05 , FE05 , FE05 , FRE02, FRE02, FRE02, FRE02, FRE02, FE05 , FE05 , FRE02, FRE02, FRE02, FRE02, FRE02, FRE02, FRE02, FRE02, FE05 , FRE02, FRE02, FRE02}, //++ +
/*C_FLT */{FE05 , FE05 , FE05 , FE05 , FE05 , FRE02, FRE02, FRE02, FRE02, FRE02, FE05 , FE05 , FRE02, FRE02, FRE02, FRE02, FRE02, FRE02, FRE02, FRE02, FE05 , FRE02, FRE02, FRE02}, //++ +
/*IND   */{FSH  , FSH  , FSH  , FSH  , FSH  , FRE02, FRE02, FRE02, FRE02, FRE02, FE04 , FE04 , FRE02, FRE02, FRE02, FRE02, FRE02, FRE02, FRE02, FRE02, FSH  , FRE02, FRE02, FRE02}, //++ +
/*IF    */{FE11 , FE11 , FE11 , FE11 , FE11 , FE11 , FE11 , FE11 , FE11 , FE11 , FE11 , FSH  , FE11 , FE11 , FE11 , FE11 , FE11 , FE11 , FE11 , FE11 , FE11 , FE11 , FE11 , FE06 }, //++ +
/*ELSE  */{FE04 , FE04 , FE04 , FE04 , FE04 , FE04 , FE04 , FE04 , FE04 , FSH  , FE04 , FE04 , FE04 , FE04 , FSH  , FSH  , FSH  , FSH  , FE04 , FSH  , FE04 , FSH  , FSH  , FE06 }, //++ +
/*FOR   */{FE11 , FE11 , FE11 , FE11 , FE11 , FE11 , FE11 , FE11 , FE11 , FE11 , FE11 , FSH  , FE11 , FE11 , FE11 , FE11 , FE11 , FE11 , FE11 , FE11 , FE11 , FE11 , FE11 , FE06 }, //++ +
/*IN    */{FE04 , FE04 , FE04 , FE04 , FE04 , FE04 , FE04 , FE04 , FE04 , FE04 , FE04 , FE04 , FE04 , FE04 , FSH  , FSH  , FSH  , FE04 , FE04 , FE04 , FE04 , FE04 , FE04 , FE06 }, //++ +
/*RETURN*/{FE04 , FE04 , FE04 , FE04 , FE04 , FE04 , FE04 , FE04 , FE04 , FE04 , FE04 , FE04 , FE04 , FSH  , FSH  , FSH  , FSH  , FE04 , FE04 , FE04 , FE04 , FE04 , FE04 , FE06 }, //++ +
/*WITH  */{FE11 , FE11 , FE11 , FE11 , FE11 , FE11 , FE11 , FE11 , FE11 , FE11 , FE11 , FSH  , FE11 , FE11 , FE11 , FE11 , FE11 , FE11 , FE11 , FE11 , FE11 , FE11 , FE11 , FE06 }, //++ +
/*empty */{FCR  , FCR  , FCR  , FCR  , FCR  , FCR  , FCR  , FCR  , FCR  , FCR  , FCR  , FCR  , FCR  , FCR  , FCR  , FCR  , FCR  , FCR  , FCR  , FCR  , FCR  , FCR  , FCR  , FCR  }, //++ +

/*PR    */{FE07 , FE07 , FE07 , FE07 , FE07 , FE07 , FE07 , FE07 , FE07 , FE07 , FE07 , FE07 , FE07 , FE07 , FE07 , FE07 , FE07 , FE07 , FE07 , FE07 , FE07 , FE07 , FE07 , FEXIT}, //++ +
/*SL    */{FE02 , FE02 , FE02 , FE02 , FE02 , FE02 , FE02 , FE02 , FE02 , FSH  , FSH  , FE02 , FE02 , FE02 , FSH  , FSH  , FSH  , FSH  , FE02 , FSH  , FE02 , FSH  , FSH  , FRE18}, //++ +
/*ST    */{FE02 , FE02 , FE02 , FE02 , FE02 , FE02 , FE02 , FE02 , FE02 , FRE17, FRE17, FE02 , FE02 , FRE17, FRE17, FRE17, FRE17, FRE17, FRE17, FRE17, FE02 , FRE17, FRE17, FRE17}, //++ +
/*BL    */{FE04 , FE04 , FE04 , FE04 , FE04 , FE04 , FE04 , FE04 , FE04 , FRE16, FRE16, FE04 , FE04 , FRE16, FRE16, FRE16, FRE16, FRE16, FRE16, FRE16, FE04 , FRE16, FRE16, FRE16}, //++ +
/*IS    */{FE04 , FE04 , FE04 , FE04 , FE04 , FE04 , FE04 , FE04 , FE04 , FRE15, FRE15, FE04 , FE04 , FRE15, FRE15, FRE15, FRE15, FRE15, FSH  , FRE15, FE04 , FRE15, FRE15, FRE15}, //++ +
/*IES   */{FE04 , FE04 , FE04 , FE04 , FE04 , FE04 , FE04 , FE04 , FE04 , FRE14, FRE14, FE04 , FE04 , FRE14, FRE14, FRE14, FRE14, FRE14, FRE14, FRE14, FE04 , FRE14, FRE14, FRE14}, //++ +
/*ITS   */{FE04 , FE04 , FE04 , FE04 , FE04 , FE04 , FE04 , FE04 , FE04 , FRE13, FRE13, FE04 , FE04 , FRE13, FRE13, FRE13, FRE13, FRE13, FRE13, FRE13, FE04 , FRE13, FRE13, FRE13}, //++ +
/*RS    */{FE04 , FE04 , FE04 , FE04 , FE04 , FE04 , FE04 , FE04 , FE04 , FRE12, FRE12, FE04 , FE04 , FRE12, FRE12, FRE12, FRE12, FRE12, FRE12, FRE12, FE04 , FRE12, FRE12, FRE12}, //++ +
/*RE    */{FE04 , FE04 , FE04 , FE04 , FE04 , FE04 , FE04 , FE04 , FE04 , FE04 , FE04 , FE04 , FE04 , FSH  , FE04 , FE04 , FE04 , FE04 , FE04 , FE04 , FE04 , FE04 , FE04 , FE06 }, //++ +
/*WS    */{FE04 , FE04 , FE04 , FE04 , FE04 , FE04 , FE04 , FE04 , FE04 , FE04 , FE04 , FE04 , FE04 , FSH  , FE04 , FE04 , FE04 , FE04 , FE04 , FE04 , FE04 , FE04 , FE04 , FE06 }, //++ +

/*FE    */{FE05 , FE05 , FE05 , FE05 , FE05 , FE05 , FE05 , FE05 , FE05 , FSH  , FE05 , FE05 , FE05 , FE05 , FSH  , FSH  , FSH  , FSH  , FE05 , FSH  , FE05 , FSH  , FSH  , FE06 }, //++ +
/*P0FE  */{FE08 , FE08 , FE08 , FE08 , FE08 , FE08 , FE08 , FE08 , FE08 , FE08 , FE08 , FE08 , FSH  , FE08 , FE08 , FE08 , FE08 , FE08 , FE08 , FE08 , FE08 , FE08 , FE08 , FE06 }, //++ +
/*P1FE  */{FE08 , FE08 , FE08 , FE08 , FE08 , FE08 , FE08 , FE08 , FE08 , FE08 , FE08 , FE08 , FSH  , FE08 , FE08 , FE08 , FE08 , FE08 , FE08 , FE08 , FE08 , FE08 , FE08 , FE06 }, //++ +
/*P2FE  */{FE01 , FE01 , FE01 , FE01 , FE01 , FE01 , FE01 , FE01 , FE01 , FE01 , FE01 , FE01 , FE01 , FE01 , FSH  , FSH  , FSH  , FE01 , FE01 , FE01 , FE01 , FE01 , FE01 , FE06 }, //++ +
/*P3FE  */{FE09 , FE09 , FE09 , FE09 , FE09 , FE09 , FE09 , FE09 , FE09 , FE09 , FE09 , FE09 , FE09 , FSH  , FE09 , FE09 , FE09 , FE09 , FE09 , FE09 , FE09 , FE09 , FE09 , FE06 }, //++ +
/*P4FE  */{FE01 , FE01 , FE01 , FE01 , FE01 , FE01 , FE01 , FE01 , FE01 , FE01 , FE01 , FE01 , FE01 , FE01 , FSH  , FSH  , FSH  , FE01 , FE01 , FE01 , FE01 , FE01 , FE01 , FE06 }, //++ +
/*ES    */{FE04 , FE04 , FE04 , FE04 , FE04 , FE04 , FE04 , FE04 , FE04 , FRE11, FRE11, FE04 , FE04 , FRE11, FRE11, FRE11, FRE11, FRE11, FRE11, FRE11, FE04 , FRE11, FRE11, FRE11}, //++ +
/*EIP   */{FE04 , FE04 , FE04 , FE04 , FE04 , FE04 , FE04 , FE04 , FE04 , FSH  , FE04 , FE04 , FE04 , FE04 , FSH  , FSH  , FSH  , FSH  , FE04 , FSH  , FE04 , FSH  , FSH  , FE06 }, //++ +
/*LPAE  */{FE10 , FE10 , FE10 , FE10 , FE10 , FE10 , FE10 , FE10 , FE10 , FE10 , FE10 , FE10 , FSH  , FSH  , FE10 , FE10 , FE10 , FE10 , FE10 , FE10 , FE10 , FE10 , FE10 , FE06 }, //++ +

/*EX    */{FE05 , FE05 , FE05 , FE05 , FE05 , FE05 , FE05 , FE05 , FE05 , FRE10, FE05 , FE05 , FRE10, FRE10, FRE10, FRE10, FRE10, FRE10, FRE10, FRE10, FE05 , FRE10, FRE10, FRE10}, //++ +
/*AE    */{FE05 , FE05 , FE05 , FE05 , FE05 , FE05 , FE05 , FE05 , FE05 , FRE09, FRE09, FE05 , FRE09, FRE09, FRE09, FRE09, FRE09, FRE09, FRE09, FRE09, FE05 , FRE09, FRE09, FRE09}, //++ +
/*OP    */{FE05 , FE05 , FE05 , FE05 , FE05 , FE05 , FE05 , FE05 , FE05 , FE05 , FE05 , FE05 , FE05 , FE05 , FSH  , FSH  , FSH  , FE05 , FE05 , FE05 , FE05 , FE05 , FE05 , FE06 }, //++ +
/*LOE   */{FE01 , FE01 , FE01 , FE01 , FE01 , FE01 , FE01 , FE01 , FSH  , FRE08, FRE08, FE01 , FRE08, FRE08, FRE08, FRE08, FRE08, FRE08, FRE08, FRE08, FE01 , FRE08, FRE08, FRE08}, //++ +
/*LAE   */{FE01 , FE01 , FE01 , FE01 , FE01 , FE01 , FSH  , FE01 , FRE07, FRE07, FRE07, FE01 , FRE07, FRE07, FRE07, FRE07, FRE07, FRE07, FRE07, FRE07, FE01 , FRE07, FRE07, FRE07}, //++ +
/*BOE   */{FE01 , FE01 , FE01 , FE01 , FE01 , FE01 , FRE06, FSH  , FRE06, FRE06, FRE06, FE01 , FRE06, FRE06, FRE06, FRE06, FRE06, FRE06, FRE06, FRE06, FE01 , FRE06, FRE06, FRE06}, //++ +
/*BAE   */{FE01 , FE01 , FE01 , FE01 , FE01 , FSH  , FRE05, FRE05, FRE05, FRE05, FRE05, FE01 , FRE05, FRE05, FRE05, FRE05, FRE05, FRE05, FRE05, FRE05, FE01 , FRE05, FRE05, FRE05}, //++ +
/*EE    */{FE05 , FE05 , FE05 , FE05 , FE05 , FRE04, FRE04, FRE04, FRE04, FRE04, FRE04, FE05 , FRE04, FRE04, FRE04, FRE04, FRE04, FRE04, FRE04, FRE04, FE05 , FRE04, FRE04, FRE04}, //++ +
/*AR    */{FE05 , FE05 , FE05 , FE05 , FE05 , FRE03, FRE03, FRE03, FRE03, FRE03, FRE03, FE05 , FRE03, FRE03, FRE03, FRE03, FRE03, FRE03, FRE03, FRE03, FE05 , FRE03, FRE03, FRE03}, //++ +
/*v     */{FE04 , FE04 , FE04 , FE04 , FE04 , FE04 , FE04 , FE04 , FE04 , FSH  , FE04 , FE04 , FE04 , FE04 , FSH  , FSH  , FSH  , FSH  , FE04 , FSH  , FE04 , FSH  , FSH  , FE06 }, //++ +
};
    fout.open("output.txt");
    StLx _stack;
    _stack.push(v);
    LstLex.push_back(lexem(ERR, ERR_C, ""));
    int stat = SUCCESS;
    Lex currLex = LstLex.begin();
    while (stat != EXIT && stat != CRITERR) {
        stat = table[_stack.top()-1][currLex->_id-1](currLex, _stack);
        if (stat == REPAERR) {
            handlerError(currLex, _stack);
        } 
        prLx(_stack);
    }

    if (currLex->_id != ERR) {
        cout << "excess characters: line " << currLex->_line << " terminal: " << print(currLex->_id) << endl;
        return false;
    } else if (_stack.size() != 2 || _stack.top() != PR) {
        cout << "premature end of the program" << endl;
        return false;
    } else return true;
}

//свертка (reduce), перенос (shift)