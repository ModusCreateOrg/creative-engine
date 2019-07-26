OBJ=	ResourceFile.o utils.o RawFile.o RawBitmap.o BMPFile.o rcomp.o

.cpp.o:
	g++ -c -O3 -I ../../src -o $*.o $*.cpp

all: rcomp

rcomp:	$(OBJ)
	g++ -o rcomp -I../../src/ $(OBJ)
	cp rcomp ..

clean:
	@rm -f *.o rcomp
