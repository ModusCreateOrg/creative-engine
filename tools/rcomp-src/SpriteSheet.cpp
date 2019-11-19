#include "SpriteSheet.h"
#include "../../src/BResourceManager.h"
#include "../../src/BSpriteSheet.h"

#define DEBUGME
#undef DEBUGME

static struct ImageType {
  TInt w, h;
  TInt16 type;
} image_types[] = {
  {8,   8,   IMAGE_8x8,},
  {16,  16,  IMAGE_16x16,},
  {32,  32,  IMAGE_32x32,},
  {64,  64,  IMAGE_64x64,},
  {128, 128, IMAGE_128x128,},
  {256, 256, IMAGE_256x256,},

  {8,   16,  IMAGE_8x16,},
  {16,  8,   IMAGE_16x8,},
  {8,   32,  IMAGE_8x32,},
  {32,  8,   IMAGE_32x8,},
  {8,   64,  IMAGE_8x64,},
  {64,  8,   IMAGE_64x8,},
  {8,   128, IMAGE_8x128,},
  {128, 8,   IMAGE_128x8,},
  {8,   256, IMAGE_8x256,},
  {256, 8,   IMAGE_256x8,},

  {16,  32,  IMAGE_16x32,},
  {32,  16,  IMAGE_32x16,},
  {16,  64,  IMAGE_16x64,},
  {64,  16,  IMAGE_64x16,},
  {16,  128, IMAGE_16x128,},
  {128, 16,  IMAGE_128x16,},
  {16,  256, IMAGE_16x256,},
  {256, 16,  IMAGE_256x16,},

  {32,  64,  IMAGE_32x64,},
  {64,  32,  IMAGE_64x32,},
  {32,  128, IMAGE_32x128,},
  {128, 32,  IMAGE_128x32,},
  {32,  256, IMAGE_32x256,},
  {256, 32,  IMAGE_256x32,},

  {64,  128, IMAGE_64x128,},
  {128, 64,  IMAGE_128x64,},
  {64,  256, IMAGE_64x256,},
  {256, 64,  IMAGE_256x64,},
  {128, 256, IMAGE_128x256,},
  {256, 128, IMAGE_256x128,},
};
const TInt NUM_TYPES = sizeof(image_types) / sizeof(struct ImageType);

SpriteSheet::SpriteSheet(char *aDimensions, char *aBitmapFilename) : bmp(ENull) {
  // parse dimensions
  char *src = aDimensions;
  TInt sw = 0, sh = 0;
  // width
  while (isdigit(*src)) {
    sw = sw * 10;
    sw += *src - '0';
    src++;
  }
  if (sw == 0 || tolower(*src) != 'x') {
    Panic("*** SpriteSheet: invalid dimension (width): %s\n", aDimensions);
  }

  src++; // skip the x

  // height
  while (isdigit(*src)) {
    sh = sh * 10;
    sh += *src - '0';
    src++;
  }
  if (sh == 0) {
    Panic("*** SpriteSheet: invalid dimension (height): %s\n", aDimensions);
  }

  // get IMAGE_type
  image_type = -1;
  for (TInt i = 0; i < NUM_TYPES; i++) {
    if (image_types[i].w == sw && image_types[i].h == sh) {
      image_type = image_types[i].type;
      break;
    }
  }
  if (image_type == -1) {
    Panic("*** SpriteSheet: invalid dimension (IMAGE_type): %s\n", aDimensions);
  }

#ifdef DEBUGME
  printf("Sprites are %dx%d\n", sw, sh);
#endif

  // init BBitmap
  bmp = new BMPFile(aBitmapFilename);
  bmp->Dump("  ");
#ifdef DEBUGME
  printf("Bitmap is %dx%d\n", bmp->width, bmp->height);
#endif


  // set up color palette in BBitmap, retain transparent color
  TInt transparent = -1;
  for (TInt c = 0; c < 256; c++) {
    TRGB color(bmp->palette[c * 3 + 0], bmp->palette[c * 3 + 1], bmp->palette[c * 3 + 2]);
    if (color.r == 255 && color.g == 0 && color.b == 255) {
      transparent = c;
    }
  }

  // number of sprites on sprite sheet
  TInt wide = bmp->width / sw,
    high = bmp->height / sh;

  num_sprites = wide * high;

#ifdef DEBUGME
  printf("  %d sprites wide, %d sprites high, %d total sprites %d\n", wide, high, num_sprites, sizeof(BSpriteInfo));
#endif

  BSpriteInfo spriteInfo[num_sprites],
    *si = &spriteInfo[0];
  info = si;

  for (TInt h = 0; h < high; h++) {
    for (TInt w = 0; w < wide; w++) {
      TInt ulx = w * sw,
          uly = h * sh;

      TRect r(ulx, uly, ulx + sw - 1, uly + sh - 1);
      // remember top corner, to calculate dx,dy later on
      // move left side in, looking for non transparent pixel
      {
        TBool found = EFalse;
        while (r.x1 < r.x2) {
          for (TInt y = r.y1; y <= r.y2; y++) {
            TUint8 pix = bmp->ReadPixel(r.x1, y);
            if (pix != transparent) {
              found = ETrue;
              break;
            }
          }
          if (found) {
            break;
          }
          r.x1++;
        }
      }

      // move right side in
      {
        TBool found = EFalse;
        while (r.x2 > r.x1) {
          for (TInt y = r.y1; y <= r.y2; y++) {
            TUint8 pix = bmp->ReadPixel(r.x2, y);
            if (pix != transparent) {
              found = ETrue;
              break;
            }
          }
          if (found) {
            break;
          }
          r.x2--;
        }
      }

      // move top side down
      {
        TBool found = EFalse;
        while (r.y1 < r.y2) {
          for (TInt x = r.x1; x < r.x2 && !found; x++) {
            if (bmp->ReadPixel(x, r.y1) != transparent) {
              found = ETrue;
              break;
            }
          }
          if (found) {
            break;
          }
          r.y1++;
        }
      }

      // move bottom side up
      {
        TBool found = EFalse;
        while (r.y2 > r.y1) {
          for (TInt x = r.x1; x < r.x2 && !found; x++) {
            if (bmp->ReadPixel(x, r.y2) != transparent) {
              found = ETrue;
              break;
            }
          }
          if (found) {
            break;
          }
          r.y2--;
        }
      }


      // fill in the BSpriteInfo struct
      si->dx = r.x1 - ulx;
      si->dy = r.y1 - uly;
      si->x1 = r.x1;
      si->y1 = r.y1;
      si->x2 = r.x2 + 1;
      si->y2 = r.y2 + 1;
//      info[0].Dump();
//      si->Dump();
#ifdef DEBUGME
//      si->Dump();
#endif
      si++;
    }
  }
}

SpriteSheet::~SpriteSheet() {
  delete bmp;
}

void SpriteSheet::Write(ResourceFile &resourceFile) {
//  info[0].Dump("a");
  char buf[4096];
  char *ptr = bmp->filename, *slash = bmp->filename;
  while (*ptr) {
    if (*ptr == '/') {
      slash = ptr;
    }
    ptr++;
  }
  if (*slash == '/') {
    slash++;
  }
  strcpy(buf, slash);
  TUint16 bmpId = resourceFile.StartResource(buf);
  bmp->Write(resourceFile);
  strcat(buf, ".SPRITES");
  TUint16 id = resourceFile.StartResource(buf);
  resourceFile.Write(&bmpId, sizeof(TUint16));
  resourceFile.Write(&image_type, sizeof(TUint16));
  resourceFile.Write(&num_sprites, sizeof(TUint16));
  for (TInt i=0; i<num_sprites; i++) {
    resourceFile.Write(&info[i], sizeof(BSpriteInfo));
  }
}
