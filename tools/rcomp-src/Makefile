OBJ=	ResourceFile.o SpriteSheet.o utils.o RawFile.o RawBitmap.o BMPFile.o TileMap.o rcomp.o

CE=     ../../src/Panic.o ../../src/BBase.o ../../src/BBitmap.o ../../src/Memory.o

.cpp.o:
	g++ -c -O3 -I ../../src -I../../src/Display -o $*.o $*.cpp

all: rcomp

rcomp:	$(OBJ) $(CE)
	g++ -o rcomp -I../../src/ $(OBJ) $(CE)
	cp rcomp ..

clean:
	@rm -f *.o rcomp
