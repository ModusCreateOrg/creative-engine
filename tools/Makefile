OBJ=	rcomp.o

.cpp.o:
	g++ -c -O3 -I ../src -o $*.o $*.cpp

rcomp:	$(OBJ)
	g++ -o rcomp -I ../src $(OBJ)

clean:
	@rm -f *.o rcomp
