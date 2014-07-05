VPATH = src

objects = \
        NFA.o exception.o printfunc.o scan.o vector.o\
        status_set.o cmp.o nfa_to_dfa.o Set.o hash_for_wrap.o\
        DFAEntity.o regexTree.o LLex.o test_LLex.o

simpleLex: $(objects) 
	cc -o simpleLex $(objects)
	rm $(objects)
        
NFA.o:\
	NFA.c NFA.h constant.h typedef.h

exception.o: 

printfunc.o: NFA.h global.h typedef.h

scan.o: scan.h global.h typedef.h

vector.o: vector.h

status_set.o: vector.h status_set.h NFA.h

cmp.o: cmp.h

nfa_to_dfa.o: nfa_to_dfa.h

Set.o: Set.h

hash_for_wrap.o: hash_for_wrap.h status_set.h NFA.h

DFAEntity.o: DFAEntity.h NFA.h typedef.h

regexTree.o: LLex.h

LLex.o:      LLex.h

test_LLex.o: LLex.h

clean:
	rm $(objects)

install:
	rm $(objects)
	mv simpleLex /usr/bin
