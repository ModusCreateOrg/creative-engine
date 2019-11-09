OBJ=	ResourceFile.o SpriteSheet.o RawFile.o BMPFile.o TileMap.o TMXFile.o rcomp.o

TMX=    tmx/TMXMap.o tmx/TMXTileSet.o tmx/TMXLayer.o

LIB=    lib/BSymbolTable.o lib/utils.o lib/RawBitmap.o

CE=     ../../src/Panic.o ../../src/BBase.o ../../src/BBitmap.o ../../src/Memory.o ../../src/BList.o

.cpp.o:
	g++ -c -O3 -I. -I ../../src -I../../src/Display -o $*.o $*.cpp

all: rcomp

rcomp:	$(LIB) $(OBJ) $(TMX) $(CE)
	g++ -o rcomp $(OBJ) $(LIB) $(TMX) $(CE)
	cp rcomp ..

clean:
	@rm -f *.o rcomp
