#pragma once
#include "lexical_analyzer.h"
#include <stack>

typedef list<lexem>::iterator Lex;
typedef stack<NumLex> StLx;

bool parser(list<lexem>);