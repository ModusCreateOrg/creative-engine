OBJ= stmdump.o
.cpp.o:
	g++ -c -O3 -I ../src -o $*.o $*.cpp

all: stmdump

stmdump:	$(OBJ)
	g++ -o stmdump -I ../src $(OBJ)

clean:
	@rm -f *.o rcomp
