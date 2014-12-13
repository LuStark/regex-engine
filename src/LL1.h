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
regex();

extern regexNode
pure_regex();

extern regexNode
extend_regex();

extern regexNode
pure_regex_union();

extern regexNode
pure_regex_union();

extern regexNode
pure_regex_link();

extern regexNode
pure_regex_closure();

extern regexNode
pure_regex_top();



extern regexNode
extend_regex_union();

extern regexNode
extend_regex_link();

extern regexNode
extend_regex_closure();

extern regexNode
extend_regex_top();



extern regexNode    precheck();

extern regexNode    capture();



extern regexNode    normal_character();

extern regexNode    escape_character();

extern regexNode    character();

extern int
choose_range();

extern Range    char_range();

extern regexNode    character_range();

