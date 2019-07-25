RCOMP_OBJ=	rcomp.o

STMDUMP_OBJ= stmdump.o
.cpp.o:
	g++ -c -O3 -I ../src -o $*.o $*.cpp

all: rcomp stmdump

rcomp:	$(RCOMP_OBJ)
	g++ -o rcomp -I ../src $(RCOMP_OBJ)

stmdump:	$(STMDUMP_OBJ)
	g++ -o stmdump -I ../src $(STMDUMP_OBJ)

clean:
	@rm -f *.o rcomp
