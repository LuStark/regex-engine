VPATH = src

objects=\
	Array.o DFA.o Edge.o FirstFollow.o NFA.o LL1.o Status.o StatusSet.o automaton.o\
	Regex.o 

regex-engine-test: $(objects)
	gcc -g $(objects) -o regex-engine-test
	rm $(objects)

Array.o: Array.h ArrayRep.h
	gcc -g -c src/Array.h src/Array.c src/ArrayRep.h

DFA.o:	DFA.h NFA.h StatusSet.h
	gcc -g -c src/DFA.h src/DFA.c src/NFA.h src/NFA.c src/StatusSet.h src/StatusSet.c

Edge.o:	NFA.h Edge.h constant.h typedef.h
	gcc -g -c src/NFA.h src/Edge.h src/NFA.c src/Edge.c src/constant.h src/typedef.h

FirstFollow.o:	FirstFollow.h 
	gcc -g -c src/FirstFollow.h src/FirstFollow.c

NFA.o: NFA.h constant.h	typedef.h Array.h automaton.h Edge.h Status.h
	gcc -g -c src/NFA.h src/NFA.c src/constant.h src/typedef.h src/Array.h src/Array.c src/automaton.h src/automaton.c src/Edge.h src/Edge.c src/Status.h src/Status.c

LL1.o: FirstFollow.h NFA.h Edge.h Status.h StatusSet.h DFA.h LL1.h
	gcc -g -c src/LL1.h src/LL1.c
#gcc -g src/FirstFollow.h src/NFA.h src/NFA.c src/Edge.h src/Status.h src/StatusSet.h  src/DFA.h src/LL1.h src/LL1.c  

Status.o: NFA.h constant.h typedef.h Array.h Status.h

StatusSet.o:	StatusSet.h

automaton.o:	constant.h typedef.h automaton.h

Regex.o:	Regex.h	NFA.h	typedef.h	FirstFollow.h automaton.h


clean:
	rm $(objects)
