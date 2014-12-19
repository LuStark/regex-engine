#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <wchar.h>
#include <assert.h>
#include <locale.h>

#include "NFA.h"
#include "DFA.h"
#include "Status.h"
#include "FirstFollow.h"
#include "automaton.h"
#include "Regex.h"
#include "Edge.h"


extern regexNode
LL1_Regex();

extern regexNode
Regex_union();

extern regexNode
Regex_link();

extern regexNode
Regex_closure();

extern regexNode
Regex_top();


extern regexNode
extend_regex();


extern regexNode    normal_character();
extern regexNode    escape_character();


extern int          choose_or_not();
extern Range        range();
extern regexNode    character_range();

extern char*
string();
