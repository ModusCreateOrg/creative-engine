/** @file
 * \brief Resource compiler for game engine.
 *
 * Reads in PNG files and generates a packed resource file along with a
 * Resources.h file. The Resources.h file contains #defines for each resource,
 * which are ultimately indexes into an array of BBitmaps in the game engine.
 *
 * TODO:
 *  Support song data, raw 8 bit samples, etc.
 */

#include <cstdio>
#include <sys/types.h>
#include <cstdarg>
#include <cstdint>
#include <unistd.h>
#include <cerrno>
#include <fcntl.h>
#include <cstdlib>
#include <cstring>
#include <cctype>
#include <libgen.h>
//
#include "../src/BTypes.h"
#include "../src/Memory.h"

#ifndef O_BINARY
#define O_BINARY (0)
#endif

// maximum number of resources allowed in a .bin file
#define MAX_RESOURCE_FILES 1024

static void abort(const char *message, ...) {
  va_list args;
  va_start(args, message);
  vfprintf(stderr, message, args);
  va_end(args);
  exit(EXIT_FAILURE);
}

void HexDump(TUint8 *ptr, int length) {
  TUint32  addr = 0;
  TInt count = 0;
  while (length > 0) {
    printf("%08x ", addr);
    for (int i = 0; i < 8 && --length > 0; i++) {
      printf("%02x ", *ptr++);
      count++;
      if (count > 7) {
        count = 0;
        addr += 8;
        break;
      }
    }
    printf("\n");
  }
}

struct RawFile {
  RawFile(const char *aFilename) {
    filename = strdup(aFilename);
    alive    = EFalse;
    int fd   = open(filename, O_RDONLY | O_BINARY);
    if (fd < 1) {
      return;
    }
    this->size = (TUint32) lseek(fd, 0, 2);
    lseek(fd, 0, 0);
    this->data = new TUint8[size];
    read(fd, this->data, size);
    close(fd);
    alive = ETrue;
  }

  TUint32 OutputSize() {
    return sizeof(size) + size;
  }

  TBool   alive;
  char    *filename;
  TUint32 size;
  TUint8  *data;
};

class RawBitmap {
public:
  const char *filename;
  uint16_t   width, height, depth, bytesPerRow;
  uint16_t   palette_size; // number of colors
  uint8_t    *palette;
  uint8_t    *pixels;

public:
  uint32_t BytesInBitmap() {
    return (uint32_t(bytesPerRow) * uint32_t(height));
  }

  uint32_t OutputSize() {
    uint32_t size = 0;
    size += sizeof(width);
    size += sizeof(height);
    size += sizeof(depth);
    size += sizeof(bytesPerRow);
    size += sizeof(palette_size);
    size += palette_size * 3;
    size += BytesInBitmap();
    return size;
  }

public:
  void Dump() {
    printf("%-32.32s %dx%d (%d bytes per row) %d-bit %dc %d "
           "output size\n",
           filename, width, height, bytesPerRow, depth, palette_size,
           OutputSize());
#if 0
    for (int i = 0; i < palette_size; i++) {
      printf("%02x: %02x %02x %02x\n", i, palette[i * 3 + 0],
             palette[i * 3 + 1], palette[i * 3 + 2]);
    }
#endif
#if 0
    printf("Pixels:\n");
    for (int y = 0; y < height; y++) {
      printf("%4d    ", y);
      for (int resources = 0; resources < width; resources++) {
        printf("%02x ", pixels[y * width + resources]);
      }
      printf("\n\n");
    }
#endif
  }
};

struct RGBQUAD {
  TUint8 blue, green, red, reserved;
};

// This struct assumes x86 (little endian)
struct BMPHEADER {
  // struct BMPFILEHDR {
  TUint8  bfType1;      // type of file (must be BM)
  TUint8  bfType2;      // type of file (must be BM)
  TUint32 bfSize;      // size of file in bytes (lo word)
  TUint16 bfReserved1; // set to zero
  TUint16 bfReserved2; // set to zero
  TUint32
          bfOffBits;   // offset from BITMAPFILEHEADER struct to actual bitmap data
  TInt32  biSize;   // sizeof(BITMAPINFOHEADER) = 40
  TInt32  biWidth;  // width of bitmap in pixels
  TInt32  biHeight; // height of bitmap in pixels
  TInt16  biPlanes; // number of bitplanes (must be 1)
  TInt16  biBitCount;      // bits per pixel (1, 4, 8, 16, or 24)
  TInt32  biCompression;   // type of compression 0 = none
  TInt32  biSizeImage;     // size of image, in bytes.  0 if no compression
  TInt32  biXPelsPerMeter; // pixels per meter (0)
  TInt32  biYPelsPerMeter; // pixels per meter (0)
  TInt32  biClrUsed;       // number of color entries in color table
  TInt32  biClrImportant;  // number of colors that are used in the color table

  RGBQUAD palette[256];

  TUint8 bits[4]; // WARNING!!! SUBTRACT 4 FROM SIZE
} __attribute__((packed));


#ifndef O_BINARY
#define O_BINARY 0
#endif

class BMPFile : public RawBitmap {
private:
  TUint8    *rawFile;
  BMPHEADER *bmp;

public:
  BMPFile(const char *filename) {
    //
    int fd = open(filename, O_RDONLY | O_BINARY);
    if (fd < 0) {
      abort("BMPFile: could not open %s\n", filename);
    }
    off_t size = lseek(fd, 0, 2);
    lseek(fd, 0, 0);
    rawFile = new TUint8[size];
    if (read(fd, rawFile, size) != size) {
      abort("read error\n");
    }
    close(fd);
    bmp = (BMPHEADER *) rawFile;
    if (bmp->bfType1 != 'B' || bmp->bfType2 != 'M') {
      abort("Expected BM at start of header\n");
    }
    if (bmp->biCompression != 0) {
      abort("Compressed BMP not supported (%d)\n", bmp->biCompression);
    }
    const int num_colors = bmp->biClrImportant ? bmp->biClrImportant : 256;
    this->filename     = strdup(filename);
    this->width        = bmp->biWidth;
    this->height       = bmp->biHeight;
    this->depth        = bmp->biBitCount;
    this->bytesPerRow  = this->width;
    this->palette_size = num_colors;
    this->palette      = new uint8_t[num_colors * 3];
    // copy palette
    for (int i     = 0; i < num_colors; i++) {
      this->palette[i * 3 + 0] = bmp->palette[i].red;
      this->palette[i * 3 + 1] = bmp->palette[i].green;
      this->palette[i * 3 + 2] = bmp->palette[i].blue;
    }
    // copy pixels
    uint8_t  *src  = &rawFile[bmp->bfOffBits];
    int      pitch = (bmp->biWidth + 3) & ~0x03;       // round up to 4 byte boundary
    this->pixels = new uint8_t[this->width * this->height];
    uint8_t  *dst = this->pixels;
    for (int y    = 0; y < this->height; y++) {
      for (int x = 0; x < this->width; x++) {
//        dst[y*width +resources] = src[y * pitch + resources];
        dst[(this->height - y - 1) * this->width + x] =
          src[y * pitch + x];
      }
    }
#ifdef VERBOSE
    printf("%u bits per pixel\n", bmp->biBitCount);
    printf("%d size\n", bmp->bfSize);
    printf("%dx%d pixels\n", bmp->biWidth, bmp->biHeight);
    printf("offset to pixels: %d\n", bmp->bfOffBits);
    printf("color table entries: %d\n", bmp->biClrUsed);
    printf("color table entries used: %d\n", bmp->biClrImportant);
#endif
  }

  ~BMPFile() {
    //
  }
};

void usage() {
  printf("Usage: rcomp <option(s)> <files>\n");
  printf(" Compiles resources specified in <files> into one packed binary "
         "file, Resources.bin.\n\n");
  printf(" The <files> are resource 'source' files, which contain one "
         "'resource' filename per line.\n\n");
  printf(" The compiler also generates a .h file, Resources.h, with\n");
  printf(" a #define for each of the input resources, an index into the "
         "offset\n");
  printf(" table generated at the head of the packed binary file.\n\n");
  printf(" Bitmaps are converted from BMP format into a simpler raw format\n");
  printf(" that allows the game/application to process the graphics with\n");
  printf(" less overhead.\n");

  exit(1);
}

// return pointer to bit of string after blanks.
// char *skipbl(char *p) {
//  while (isspace(*p)) {
//    p++;
//  }
//  return p;
//}

// trim leading and trailing whitespace
char *trim(char *p) {
  while (isspace(*p))
    p++;
  for (char *end = &p[strlen(p) - 1]; end > p && isspace(*end);) {
    *end-- = '\0';
  }

  return p;
}

char path[2048] = "";
char work[2048], base[2048]; // misc work buffers

FILE *defines;
FILE *bin;

int main(int ac, char *av[]) {
  if (ac < 2) {
    usage();
  }

  defines = fopen("Resources.h", "w");
  if (!defines) {
    printf("Can't open output file Resources.h (%d)\n", errno);
    exit(1);
  }

  bin = fopen("Resources.bin", "w");
  if (!bin) {
    printf("Can't open binary file Resources.bin (%d)\n", errno);
    exit(1);
  }
  uint32_t index       = 0; // index into offset table at beginning of compiled data
  uint32_t offset      = 0;
  uint32_t offsets[MAX_RESOURCE_FILES];
  uint8_t  *output     = (uint8_t *) malloc(4096);
  uint32_t output_size = 4096;

  // process all files on command line
  for (int i = 1; i < ac; i++) {
    FILE *fp = fopen(av[i], "rb");
    if (!fp) {
      printf("Can't open input file %s (%d)\n", av[i], errno);
      exit(1);
    }

    sprintf(work, "%s.h", av[i]);

    size_t len    = 0, read;
    char   *input = nullptr;
    while ((read = getline(&input, &len, fp)) != -1) {
      char *line = strdup(trim(input));

      for (int i = 0; i < read; i++) {
        if (i && line[i-1] != ' ' && line[i] == '#') {
          continue;
        }
        if (line[i] == '#' || line[i] == '\n') {
          line[i] = '\0';
          break;
        }
      }
//      printf("line: %s\n", line);

      if (!strlen(line)) {
        continue;
      } else if (!strncasecmp(line, "PATH", 4)) {

        strcpy(work, trim(&line[4]));
        // dirname and basename may modify their buffers
        strcpy(base, trim(&line[4])); 
        sprintf(path, "%s/%s", dirname(work), basename(base));
        // assure path ends with /
        printf("Processing resources in path (%s)\n", path);

      } else if (!strncasecmp(line, "RAW", 3)) {

        // this code is common with BITMAP logic
        strcpy(base, trim(&line[3]));
        sprintf(work, "%s/%s", path, base);
        for (int i = 0; base[i]; i++) {
          switch (base[i]) {
            case '.':
            case '#':
            case '~':
            case ' ':
            case '-':
              base[i] = '_';
              break;
            default:
              base[i]   = (char) toupper(base[i]);
          }
        }
        fprintf(defines, "#define %-64.64s %d\n", base, index);

        // OUTPUT format is TUInt32 size, TUint8 data[size]
        RawFile *r = new RawFile(work);
        if (!r->alive) {
          abort("Can't open %s\n", work);
        }
        printf("%s: %d bytes\n", r->filename, r->size);

        uint32_t next_offset = offset + r->OutputSize();
        if (next_offset > output_size) {
          output_size = next_offset;
          output      = (uint8_t *) realloc(output, output_size);
        }
        offsets[index++] = offset;

        memcpy(&output[offset], (void *) &r->size, sizeof(TUint32));
        offset += sizeof(TUint32);
        memcpy(&output[offset], (void *) r->data, r->size);
//        HexDump(&output[offset], r->size);
        offset += r->size;

      } else if (!strncasecmp(line, "BITMAP", 6)) {

        strcpy(base, trim(&line[6]));
        sprintf(work, "%s/%s", path, base);
          for (int i = 0; base[i]; i++) {
              switch (base[i]) {
                  case '.':
                  case '#':
                  case '~':
                  case ' ':
                  case '-':
                      base[i] = '_';
                      break;
                  default:
                      base[i]   = (char) toupper(base[i]);
              }
          }
        fprintf(defines, "#define %-64.64s %d\n", base, index);

        BMPFile b(work);
        b.Dump();

        uint32_t next_offset = offset + b.OutputSize();
        if (next_offset > output_size) {
          output_size = next_offset;
          output      = (uint8_t *) realloc(output, output_size);
        }

        offsets[index++] = offset;

        // copy the bitmap values into output
        memcpy(&output[offset], (void *) &b.width, sizeof(b.width));
        offset += sizeof(b.width);

        memcpy(&output[offset], (void *) &b.height, sizeof(b.height));
        offset += sizeof(b.height);

        memcpy(&output[offset], (void *) &b.depth, sizeof(b.depth));
        offset += sizeof(b.depth);

        memcpy(&output[offset], (void *) &b.bytesPerRow, sizeof(b.bytesPerRow));
        offset += sizeof(b.bytesPerRow);

        memcpy(&output[offset], (void *) &b.palette_size,
               sizeof(b.palette_size));
        offset += sizeof(b.palette_size);

        memcpy(&output[offset], (void *) b.palette, 3 * b.palette_size);
        offset += 3 * b.palette_size;

        memcpy(&output[offset], (void *) b.pixels, b.BytesInBitmap());
        offset += b.BytesInBitmap();
      }
    }
    //      printf("line: %s\n", line);
    // done with input and header file
    fclose(fp);
    free(input);
  }
// write out the packed binary data
  fwrite(&index, sizeof(index), 1, bin);
  fwrite(offsets, index, sizeof(offsets[0]), bin);
  fwrite(output, offset, 1, bin);
  fprintf(defines, "\n#define %-64.64s %d\n", "NUM_RESOURCES", index);
  fclose(defines);
  fclose(bin);

//  BMPFile b("playernew.bmp");
//  b.Dump();
  return 0;
}
