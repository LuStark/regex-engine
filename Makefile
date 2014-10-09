VPATH = src

objects=\
	Array.o DFA.o Edge.o FirstFollow.o NFA.o RegexLL1.o Status.o StatusSet.o automaton.o\
	Regex.o testRegex.o

regex-engine-test: $(objects)
	gcc $(objects) -o regex-engine-test
	rm $(objects)

Array.o: Array.h ArrayRep.h

DFA.o:	DFA.h NFA.h StatusSet.h

Edge.o:	NFA.h Edge.h constant.h typedef.h

FirstFollow.o:	FirstFollow.h 

NFA.o: NFA.h constant.h	typedef.h Array.h automaton.h Edge.h Status.h

RegexLL1.o: FirstFollow.h NFA.h Edge.h Status.h StatusSet.h DFA.h RegexLL1.h

Status.o: NFA.h constant.h typedef.h Array.h Status.h

StatusSet.o:	StatusSet.h

automaton.o:	constant.h typedef.h automaton.h

Regex.o:	Regex.h	NFA.h	typedef.h	FirstFollow.h automaton.h

testRegex.o:    Regex.h

clean:
	rm $(objects)
