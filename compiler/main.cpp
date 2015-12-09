#include "lexical_analyzer.h"
#include "parser.h"
#include <iostream>

int main () {
    list<lexem> LstLex;
    if (!lexical_analyzer(LstLex)) return 0;
    return 1;
}