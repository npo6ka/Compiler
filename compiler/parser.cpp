#include "parser.h"

int F() {
    return 0;
}

int FSH() {
    return 0;
}

bool parser(list<lexem> &LstLex) {
    int (*table[41][24])() = {
//         ASIG   SUM    DIFF   MULT   DIVI   BN_AND LO_AND BN_OR  LO_OR  LCB    RCB    LF_PR  RG_PR  SEMIC  C_INT  C_FLT  IND    IF     ELSE   FOR    IN    RETURN  WITH   -|
/*ASIG  */{F    , F    , F    , F    , F    , F    , F    , F    , F    , F    , F    , F    , F    , F    , F    , F    , F    , F    , F    , F    , F    , F    , F    , F    }, //
/*SUM   */{F    , F    , F    , F    , F    , F    , F    , F    , F    , F    , F    , F    , F    , F    , F    , F    , F    , F    , F    , F    , F    , F    , F    , F    }, //
/*DIFF  */{F    , F    , F    , F    , F    , F    , F    , F    , F    , F    , F    , F    , F    , F    , F    , F    , F    , F    , F    , F    , F    , F    , F    , F    }, //
/*MULT  */{F    , F    , F    , F    , F    , F    , F    , F    , F    , F    , F    , F    , F    , F    , F    , F    , F    , F    , F    , F    , F    , F    , F    , F    }, //
/*DIVI  */{F    , F    , F    , F    , F    , F    , F    , F    , F    , F    , F    , F    , F    , F    , F    , F    , F    , F    , F    , F    , F    , F    , F    , F    }, //
/*BN_AND*/{F    , F    , F    , F    , F    , F    , F    , F    , F    , F    , F    , F    , F    , F    , F    , F    , F    , F    , F    , F    , F    , F    , F    , F    }, //
/*LO_AND*/{F    , F    , F    , F    , F    , F    , F    , F    , F    , F    , F    , F    , F    , F    , F    , F    , F    , F    , F    , F    , F    , F    , F    , F    }, //
/*BN_OR */{F    , F    , F    , F    , F    , F    , F    , F    , F    , F    , F    , F    , F    , F    , F    , F    , F    , F    , F    , F    , F    , F    , F    , F    }, //
/*LO_OR */{F    , F    , F    , F    , F    , F    , F    , F    , F    , F    , F    , F    , F    , F    , F    , F    , F    , F    , F    , F    , F    , F    , F    , F    }, //
/*LCB   */{F    , F    , F    , F    , F    , F    , F    , F    , F    , F    , F    , F    , F    , F    , F    , F    , F    , F    , F    , F    , F    , F    , F    , F    }, //
/*RCB   */{F    , F    , F    , F    , F    , F    , F    , F    , F    , F    , F    , F    , F    , F    , F    , F    , F    , F    , F    , F    , F    , F    , F    , F    }, //
/*LF_PR */{F    , F    , F    , F    , F    , F    , F    , F    , F    , F    , F    , F    , F    , F    , F    , F    , F    , F    , F    , F    , F    , F    , F    , F    }, //
/*RG_PR */{F    , F    , F    , F    , F    , F    , F    , F    , F    , F    , F    , F    , F    , F    , F    , F    , F    , F    , F    , F    , F    , F    , F    , F    }, //
/*SEMIC */{F    , F    , F    , F    , F    , F    , F    , F    , F    , F    , F    , F    , F    , F    , F    , F    , F    , F    , F    , F    , F    , F    , F    , F    }, //
/*C_INT */{F    , F    , F    , F    , F    , F    , F    , F    , F    , F    , F    , F    , F    , F    , F    , F    , F    , F    , F    , F    , F    , F    , F    , F    }, //
/*C_FLT */{F    , F    , F    , F    , F    , F    , F    , F    , F    , F    , F    , F    , F    , F    , F    , F    , F    , F    , F    , F    , F    , F    , F    , F    }, //
/*IND   */{F    , F    , F    , F    , F    , F    , F    , F    , F    , F    , F    , F    , F    , F    , F    , F    , F    , F    , F    , F    , F    , F    , F    , F    }, //
/*IF    */{F    , F    , F    , F    , F    , F    , F    , F    , F    , F    , F    , F    , F    , F    , F    , F    , F    , F    , F    , F    , F    , F    , F    , F    }, //
/*ELSE  */{F    , F    , F    , F    , F    , F    , F    , F    , F    , F    , F    , F    , F    , F    , F    , F    , F    , F    , F    , F    , F    , F    , F    , F    }, //
/*FOR   */{F    , F    , F    , F    , F    , F    , F    , F    , F    , F    , F    , F    , F    , F    , F    , F    , F    , F    , F    , F    , F    , F    , F    , F    }, //
/*IN    */{F    , F    , F    , F    , F    , F    , F    , F    , F    , F    , F    , F    , F    , F    , F    , F    , F    , F    , F    , F    , F    , F    , F    , F    }, //
/*RETURN*/{F    , F    , F    , F    , F    , F    , F    , F    , F    , F    , F    , F    , F    , F    , F    , F    , F    , F    , F    , F    , F    , F    , F    , F    }, //
/*WITH  */{F    , F    , F    , F    , F    , F    , F    , F    , F    , F    , F    , F    , F    , F    , F    , F    , F    , F    , F    , F    , F    , F    , F    , F    }, //

/*EE    */{F    , F    , F    , F    , F    , F    , F    , F    , F    , F    , F    , F    , F    , F    , F    , F    , F    , F    , F    , F    , F    , F    , F    , F    }, //
/*BAE   */{F    , F    , F    , F    , F    , F    , F    , F    , F    , F    , F    , F    , F    , F    , F    , F    , F    , F    , F    , F    , F    , F    , F    , F    }, //
/*BOE   */{F    , F    , F    , F    , F    , F    , F    , F    , F    , F    , F    , F    , F    , F    , F    , F    , F    , F    , F    , F    , F    , F    , F    , F    }, //
/*LAE   */{F    , F    , F    , F    , F    , F    , F    , F    , F    , F    , F    , F    , F    , F    , F    , F    , F    , F    , F    , F    , F    , F    , F    , F    }, //
/*LOE   */{F    , F    , F    , F    , F    , F    , F    , F    , F    , F    , F    , F    , F    , F    , F    , F    , F    , F    , F    , F    , F    , F    , F    , F    }, //
/*OP    */{F    , F    , F    , F    , F    , F    , F    , F    , F    , F    , F    , F    , F    , F    , F    , F    , F    , F    , F    , F    , F    , F    , F    , F    }, //
/*AE    */{F    , F    , F    , F    , F    , F    , F    , F    , F    , F    , F    , F    , F    , F    , F    , F    , F    , F    , F    , F    , F    , F    , F    , F    }, //
/*WS    */{F    , F    , F    , F    , F    , F    , F    , F    , F    , F    , F    , F    , F    , F    , F    , F    , F    , F    , F    , F    , F    , F    , F    , F    }, //
/*RS    */{F    , F    , F    , F    , F    , F    , F    , F    , F    , F    , F    , F    , F    , F    , F    , F    , F    , F    , F    , F    , F    , F    , F    , F    }, //
/*ITS   */{F    , F    , F    , F    , F    , F    , F    , F    , F    , F    , F    , F    , F    , F    , F    , F    , F    , F    , F    , F    , F    , F    , F    , F    }, //
/*IES   */{F    , F    , F    , F    , F    , F    , F    , F    , F    , F    , F    , F    , F    , F    , F    , F    , F    , F    , F    , F    , F    , F    , F    , F    }, //
/*IS    */{F    , F    , F    , F    , F    , F    , F    , F    , F    , F    , F    , F    , F    , F    , F    , F    , F    , F    , F    , F    , F    , F    , F    , F    }, //
/*B     */{F    , F    , F    , F    , F    , F    , F    , F    , F    , F    , F    , F    , F    , F    , F    , F    , F    , F    , F    , F    , F    , F    , F    , F    }, //
/*S     */{F    , F    , F    , F    , F    , F    , F    , F    , F    , F    , F    , F    , F    , F    , F    , F    , F    , F    , F    , F    , F    , F    , F    , F    }, //
/*SL    */{F    , F    , F    , F    , F    , F    , F    , F    , F    , F    , F    , F    , F    , F    , F    , F    , F    , F    , F    , F    , F    , F    , F    , F    }, //
/*v     */{F    , F    , F    , F    , F    , F    , F    , F    , F    , F    , F    , F    , F    , F    , F    , F    , F    , F    , F    , F    , F    , F    , F    , F    }, //
};



    //свертка (reduce), перенос (shift)
    /*lexem (*lexHandler[NUM_CLASS_LEX])(filebuf* inbuf, char *cash) = 
        {HandlerUnknown, HandlerAssignment, HandlerLogical, HandlerSpecialSymbol, HandlerConst, HandlerId};*/
    return 0;
}