#include "parser.h"
#include <iostream>
#include <fstream>

ofstream fout;

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

int F(Lex &cur, StLx &st) {
    return 0;
}

// shift
int FSH(Lex &cur, StLx &st) {
    st.push(cur->_id);
    cur++;
    return 1;
}

// asig | sum | diff | mult | divi -> OP
int FRE01(Lex &cur, StLx &st) {
    st.pop();
    st.push(OP);
    return 1;
}

// int | float | ind -> AR
int FRE02(Lex &cur, StLx &st) {
    st.pop();
    st.push(AR);
    return 1;
}

// AR -> EE
int FRE03(Lex &cur, StLx &st) {
    st.pop();
    st.push(EE);
    return 1;
}

// BAE '&' EE | EE -> BAE
int FRE04(Lex &cur, StLx &st) {
    st.pop();
    if (st.top() == BN_AND) {
        st.pop();
        st.pop();
    }
    st.push(BAE);
    return 1;
}

// BAE '|' BOE | BAE -> BOE
int FRE05(Lex &cur, StLx &st) {
    st.pop();
    if (st.top() == BN_OR) {
        st.pop();
        st.pop();
    }
    st.push(BOE);
    return 1;
}

// LAE '&&' BOE | BOE -> LAE
int FRE06(Lex &cur, StLx &st) {
    st.pop();
    if (st.top() == LO_AND) {
        st.pop();
        st.pop();
    }
    st.push(LAE);
    return 1;
}

// LOE '||' LAE | LAE -> LOE
int FRE07(Lex &cur, StLx &st) {
    st.pop();
    if (st.top() == LO_OR) {
        st.pop();
        st.pop();
    }
    st.push(LOE);
    return 1;
}

//  LOE -> AE
int FRE08(Lex &cur, StLx &st) {
    st.pop();
    st.push(AE);
    return 1;
}

// ID AO AE -> AE | AE -> EX
int FRE09(Lex &cur, StLx &st) {
    st.pop(); 
    if (st.top() == OP) {
        st.pop();
        st.pop();
        st.push(AE);
    } else {
        st.push(EX);
    }
    return 1;
}

// EX -> ST | RET EX -> RE | ( ID IN EX -> P0FE | P2FE EX -> P1EF | P4FE EX -> P3FE | ( EX 
int FRE10(Lex &cur, StLx &st) {
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
    return 1;
}

// if ES -> IS | WITH ES -> WS
int FRE11(Lex &cur, StLx &st) {
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
        return 0;
    }
    return 1;
}

// RS -> ST
int FRE12(Lex &cur, StLx &st) {
    st.pop();
    st.push(ST);
    return 1;
}

// ITS -> ST
int FRE13(Lex &cur, StLx &st) {
    st.pop();
    st.push(ST);
    return 1;
}

// IES -> ST
int FRE14(Lex &cur, StLx &st) {
    st.pop();
    st.push(ST);
    return 1;
}

// IS -> ST
int FRE15(Lex &cur, StLx &st) {
    st.pop();
    st.push(ST);
    return 1;
}

// BL -> ST
int FRE16(Lex &cur, StLx &st) {
    st.pop();
    st.push(ST);
    return 1;
}

// ST -> SL | SL ST -> SL | IS ELSE ST -> IES | FOR FE ST -> IS | EIP ST -> ES
int FRE17(Lex &cur, StLx &st) {
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
    return 1;
}

// SL -> PR
int FRE18(Lex &cur, StLx &st) {
    st.pop();
    st.push(PR);
    return 1;
}

// LCB RCB -> BL | LCB SL RCB -> BL
int FRE19(Lex &cur, StLx &st) {
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
    return 1;
}

// P0FE RG_PR -> FE | P1FE RG_PR -> FE | LPAE RG_PR -> EIP
int FRE20(Lex &cur, StLx &st) {
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
    return 1;
}

// WS SEMIC -> ST | RET SEMIC -> RS | RE SEMIC -> RS | P3FE SEMIC -> P2FE | LPAE SEMIC -> P4FE
int FRE21(Lex &cur, StLx &st) {
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
    return 1;
}

// 
int FEXIT(Lex &cur, StLx &st) {
    return 2;
}

bool parser(list<lexem> LstLex) {
    int (*table[53][24])(Lex &, StLx &) = {
//         ASIG   SUM    DIFF   MULT   DIVI   BN_AND LO_AND BN_OR  LO_OR  LCB    RCB    LF_PR  RG_PR  SEMIC  C_INT  C_FLT  IND    IF     ELSE   FOR    IN    RETURN  WITH   -|
/*ASIG  */{F    , F    , F    , F    , F    , F    , F    , F    , F    , F    , F    , F    , F    , F    , FRE01, FRE01, FRE01, F    , F    , F    , F    , F    , F    , F    }, //++ +
/*SUM   */{F    , F    , F    , F    , F    , F    , F    , F    , F    , F    , F    , F    , F    , F    , FRE01, FRE01, FRE01, F    , F    , F    , F    , F    , F    , F    }, //++ +
/*DIFF  */{F    , F    , F    , F    , F    , F    , F    , F    , F    , F    , F    , F    , F    , F    , FRE01, FRE01, FRE01, F    , F    , F    , F    , F    , F    , F    }, //++ +
/*MULT  */{F    , F    , F    , F    , F    , F    , F    , F    , F    , F    , F    , F    , F    , F    , FRE01, FRE01, FRE01, F    , F    , F    , F    , F    , F    , F    }, //++ +
/*DIVI  */{F    , F    , F    , F    , F    , F    , F    , F    , F    , F    , F    , F    , F    , F    , FRE01, FRE01, FRE01, F    , F    , F    , F    , F    , F    , F    }, //++ +
/*BN_AND*/{F    , F    , F    , F    , F    , F    , F    , F    , F    , F    , F    , F    , F    , F    , FSH  , FSH  , FSH  , F    , F    , F    , F    , F    , F    , F    }, //++ +
/*LO_AND*/{F    , F    , F    , F    , F    , F    , F    , F    , F    , F    , F    , F    , F    , F    , FSH  , FSH  , FSH  , F    , F    , F    , F    , F    , F    , F    }, //++ +
/*BN_OR */{F    , F    , F    , F    , F    , F    , F    , F    , F    , F    , F    , F    , F    , F    , FSH  , FSH  , FSH  , F    , F    , F    , F    , F    , F    , F    }, //++ +
/*LO_OR */{F    , F    , F    , F    , F    , F    , F    , F    , F    , F    , F    , F    , F    , F    , FSH  , FSH  , FSH  , F    , F    , F    , F    , F    , F    , F    }, //++ +
/*LCB   */{F    , F    , F    , F    , F    , F    , F    , F    , F    , FSH  , FSH  , F    , F    , F    , FSH  , FSH  , FSH  , FSH  , F    , FSH  , F    , FSH  , FSH  , F    }, //++ +
/*RCB   */{F    , F    , F    , F    , F    , F    , F    , F    , F    , FRE19, FRE19, F    , F    , FRE19, FRE19, FRE19, FRE19, FRE19, FRE19, FRE19, F    , FRE19, FRE19, FRE19}, //++ +
/*LF_PR */{F    , F    , F    , F    , F    , F    , F    , F    , F    , F    , F    , F    , F    , F    , FSH  , FSH  , FSH  , F    , F    , F    , F    , F    , F    , F    }, //++ +
/*RG_PR */{F    , F    , F    , F    , F    , F    , F    , F    , F    , FRE20, F    , F    , F    , F    , FRE20, FRE20, FRE20, FRE20, F    , FRE20, F    , FRE20, FRE20, F    }, //++ +
/*SEMIC */{F    , F    , F    , F    , F    , F    , F    , F    , F    , FRE21, FRE21, F    , F    , FRE21, FRE21, FRE21, FRE21, FRE21, FRE21, FRE21, F    , FRE21, FRE21, FRE21}, //++ +
/*C_INT */{F    , F    , F    , F    , F    , FRE02, FRE02, FRE02, FRE02, FRE02, F    , F    , FRE02, FRE02, FRE02, FRE02, FRE02, FRE02, FRE02, FRE02, F    , FRE02, FRE02, FRE02}, //++ +
/*C_FLT */{F    , F    , F    , F    , F    , FRE02, FRE02, FRE02, FRE02, FRE02, F    , F    , FRE02, FRE02, FRE02, FRE02, FRE02, FRE02, FRE02, FRE02, F    , FRE02, FRE02, FRE02}, //++ +
/*IND   */{FSH  , FSH  , FSH  , FSH  , FSH  , FRE02, FRE02, FRE02, FRE02, FRE02, F    , F    , FRE02, FRE02, FRE02, FRE02, FRE02, FRE02, FRE02, FRE02, FSH  , FRE02, FRE02, FRE02}, //++ +
/*IF    */{F    , F    , F    , F    , F    , F    , F    , F    , F    , F    , F    , FSH  , F    , F    , F    , F    , F    , F    , F    , F    , F    , F    , F    , F    }, //++ +
/*ELSE  */{F    , F    , F    , F    , F    , F    , F    , F    , F    , FSH  , F    , F    , F    , F    , FSH  , FSH  , FSH  , FSH  , F    , FSH  , F    , FSH  , FSH  , F    }, //++ +
/*FOR   */{F    , F    , F    , F    , F    , F    , F    , F    , F    , F    , F    , FSH  , F    , F    , F    , F    , F    , F    , F    , F    , F    , F    , F    , F    }, //++ +
/*IN    */{F    , F    , F    , F    , F    , F    , F    , F    , F    , F    , F    , F    , F    , F    , FSH  , FSH  , FSH  , F    , F    , F    , F    , F    , F    , F    }, //++ +
/*RETURN*/{F    , F    , F    , F    , F    , F    , F    , F    , F    , F    , F    , F    , F    , FSH  , FSH  , FSH  , FSH  , F    , F    , F    , F    , F    , F    , F    }, //++ +
/*WITH  */{F    , F    , F    , F    , F    , F    , F    , F    , F    , F    , F    , FSH  , F    , F    , F    , F    , F    , F    , F    , F    , F    , F    , F    , F    }, //++ +
/*empty */{0    , 0    , 0    , 0    , 0    , 0    , 0    , 0    , 0    , 0    , 0    , 0    , 0    , 0    , 0    , 0    , 0    , 0    , 0    , 0    , 0    , 0    , 0    , 0    }, //++ +

/*PR    */{F    , F    , F    , F    , F    , F    , F    , F    , F    , F    , F    , F    , F    , F    , F    , F    , F    , F    , F    , F    , F    , F    , F    , FEXIT}, //++ +
/*SL    */{F    , F    , F    , F    , F    , F    , F    , F    , F    , FSH  , FSH  , F    , F    , F    , FSH  , FSH  , FSH  , FSH  , F    , FSH  , F    , FSH  , FSH  , FRE18}, //++ +
/*ST    */{F    , F    , F    , F    , F    , F    , F    , F    , F    , FRE17, FRE17, F    , F    , FRE17, FRE17, FRE17, FRE17, FRE17, FRE17, FRE17, F    , FRE17, FRE17, FRE17}, //++ +
/*BL    */{F    , F    , F    , F    , F    , F    , F    , F    , F    , FRE16, FRE16, F    , F    , FRE16, FRE16, FRE16, FRE16, FRE16, FRE16, FRE16, F    , FRE16, FRE16, FRE16}, //++ +
/*IS    */{F    , F    , F    , F    , F    , F    , F    , F    , F    , FRE15, FRE15, F    , F    , FRE15, FRE15, FRE15, FRE15, FRE15, FSH  , FRE15, F    , FRE15, FRE15, FRE15}, //++ +
/*IES   */{F    , F    , F    , F    , F    , F    , F    , F    , F    , FRE14, FRE14, F    , F    , FRE14, FRE14, FRE14, FRE14, FRE14, FRE14, FRE14, F    , FRE14, FRE14, FRE14}, //++ +
/*ITS   */{F    , F    , F    , F    , F    , F    , F    , F    , F    , FRE13, FRE13, F    , F    , FRE13, FRE13, FRE13, FRE13, FRE13, FRE13, FRE13, F    , FRE13, FRE13, FRE13}, //++ +
/*RS    */{F    , F    , F    , F    , F    , F    , F    , F    , F    , FRE12, FRE12, F    , F    , FRE12, FRE12, FRE12, FRE12, FRE12, FRE12, FRE12, F    , FRE12, FRE12, FRE12}, //++ +
/*RE    */{F    , F    , F    , F    , F    , F    , F    , F    , F    , F    , F    , F    , F    , FSH  , F    , F    , F    , F    , F    , F    , F    , F    , F    , F    }, //++ +
/*WS    */{F    , F    , F    , F    , F    , F    , F    , F    , F    , F    , F    , F    , F    , FSH  , F    , F    , F    , F    , F    , F    , F    , F    , F    , F    }, //++ +

/*FE    */{F    , F    , F    , F    , F    , F    , F    , F    , F    , FSH  , F    , F    , F    , F    , FSH  , FSH  , FSH  , FSH  , F    , FSH  , F    , FSH  , FSH  , F    }, //++ +
/*P0FE  */{F    , F    , F    , F    , F    , F    , F    , F    , F    , F    , F    , F    , FSH  , F    , F    , F    , F    , F    , F    , F    , F    , F    , F    , F    }, //++ +
/*P1FE  */{F    , F    , F    , F    , F    , F    , F    , F    , F    , F    , F    , F    , FSH  , F    , F    , F    , F    , F    , F    , F    , F    , F    , F    , F    }, //++ +
/*P2FE  */{F    , F    , F    , F    , F    , F    , F    , F    , F    , F    , F    , F    , F    , F    , FSH  , FSH  , FSH  , F    , F    , F    , F    , F    , F    , F    }, //++ +
/*P3FE  */{F    , F    , F    , F    , F    , F    , F    , F    , F    , F    , F    , F    , F    , FSH  , F    , F    , F    , F    , F    , F    , F    , F    , F    , F    }, //++ +
/*P4FE  */{F    , F    , F    , F    , F    , F    , F    , F    , F    , F    , F    , F    , F    , F    , FSH  , FSH  , FSH  , F    , F    , F    , F    , F    , F    , F    }, //++ +
/*ES    */{F    , F    , F    , F    , F    , F    , F    , F    , F    , FRE11, FRE11, F    , F    , FRE11, FRE11, FRE11, FRE11, FRE11, FRE11, FRE11, F    , FRE11, FRE11, FRE11}, //++ +
/*EIP   */{F    , F    , F    , F    , F    , F    , F    , F    , F    , FSH  , F    , F    , F    , F    , FSH  , FSH  , FSH  , FSH  , F    , FSH  , F    , FSH  , FSH  , F    }, //++ +
/*LPAE  */{F    , F    , F    , F    , F    , F    , F    , F    , F    , F    , F    , F    , FSH  , FSH  , F    , F    , F    , F    , F    , F    , F    , F    , F    , F    }, //++ +

/*EX    */{F    , F    , F    , F    , F    , F    , F    , F    , F    , FRE10, F    , F    , FRE10, FRE10, FRE10, FRE10, FRE10, FRE10, FRE10, FRE10, F    , FRE10, FRE10, FRE10}, //++ +
/*AE    */{F    , F    , F    , F    , F    , F    , F    , F    , F    , FRE09, FRE09, F    , FRE09, FRE09, FRE09, FRE09, FRE09, FRE09, FRE09, FRE09, F    , FRE09, FRE09, FRE09}, //++ +
/*OP    */{F    , F    , F    , F    , F    , F    , F    , F    , F    , F    , F    , F    , F    , F    , FSH  , FSH  , FSH  , F    , F    , F    , F    , F    , F    , F    }, //++ +
/*LOE   */{F    , F    , F    , F    , F    , F    , F    , F    , FSH  , FRE08, FRE08, F    , FRE08, FRE08, FRE08, FRE08, FRE08, FRE08, FRE08, FRE08, F    , FRE08, FRE08, FRE08}, //++ +
/*LAE   */{F    , F    , F    , F    , F    , F    , FSH  , F    , FRE07, FRE07, FRE07, F    , FRE07, FRE07, FRE07, FRE07, FRE07, FRE07, FRE07, FRE07, F    , FRE07, FRE07, FRE07}, //++ +
/*BOE   */{F    , F    , F    , F    , F    , F    , FRE06, FSH  , FRE06, FRE06, FRE06, F    , FRE06, FRE06, FRE06, FRE06, FRE06, FRE06, FRE06, FRE06, F    , FRE06, FRE06, FRE06}, //++ +
/*BAE   */{F    , F    , F    , F    , F    , FSH  , FRE05, FRE05, FRE05, FRE05, FRE05, F    , FRE05, FRE05, FRE05, FRE05, FRE05, FRE05, FRE05, FRE05, F    , FRE05, FRE05, FRE05}, //++ +
/*EE    */{F    , F    , F    , F    , F    , FRE04, FRE04, FRE04, FRE04, FRE04, FRE04, F    , FRE04, FRE04, FRE04, FRE04, FRE04, FRE04, FRE04, FRE04, F    , FRE04, FRE04, FRE04}, //++ +
/*AR    */{F    , F    , F    , F    , F    , FRE03, FRE03, FRE03, FRE03, FRE03, FRE03, F    , FRE03, FRE03, FRE03, FRE03, FRE03, FRE03, FRE03, FRE03, F    , FRE03, FRE03, FRE03}, //++ +
/*v     */{F    , F    , F    , F    , F    , F    , F    , F    , F    , FSH  , F    , F    , F    , F    , FSH  , FSH  , FSH  , FSH  , F    , FSH  , F    , FSH  , FSH  , F    }, //++ +
};
    fout.open("output.txt");
    StLx buf;
    buf.push(v);
    LstLex.push_back(lexem(ERR, ERR_C, ""));
    Lex currLex = LstLex.begin();
    if (!LstLex.size()) return 1;
    //while (buf.top() != PR) {
    for (int i=0; i<140; i++) {
        if (!table[buf.top()-1][currLex->_id-1](currLex, buf)) return 0;
        prLx(buf);
    }
    while (buf.top() != PR) {
        if (!table[buf.top()-1][currLex->_id-1](currLex, buf)) return 0;
        prLx(buf);
    }

    if (currLex->_id != ERR) {
        cout << "excess characters: line " << currLex->_line << " terminal: " << print(currLex->_id) << endl;
        return 0;
    }
    if (buf.size() != 2) {
        buf.pop();
        cout << "premature end of the program: terminal " << print(buf.top()) << endl;
        return 0;
    }
    //свертка (reduce), перенос (shift)
    /*lexem (*lexHandler[NUM_CLASS_LEX])(filebuf* inbuf, char *cash) = 
        {HandlerUnknown, HandlerAssignment, HandlerLogical, HandlerSpecialSymbol, HandlerConst, HandlerId};*/
    return 1;
}