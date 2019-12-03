#include "RawFile.h"
#include "BTileMap.h"
#include "TileMap.h"
#include "stdio.h"

#define DEBUGME
#undef DEBUGME

struct LayerInfo {
    TBool used;
    RawFile *map,
            *map_attributes,
            *object,
            *object_attributes;

    LayerInfo() {
        used = EFalse;
        map = ENull;
        map_attributes = ENull;
        object = ENull;
        object_attributes = ENull;
    }
};

TInt ParseLayer(const char *str) {
    const char *pos = strcasestr(str, "Level_");
    if (pos == ENull) {
        printf("Level_ is required in filename (%s)\n", str);
        exit(1);
    }
    pos = &pos[6];
    TInt level = 0;
    while (isdigit(*pos)) {
        level *= 10;
        level += *pos - '0';
        pos++;
    }
    return level;
}

TileMap::TileMap(const char *path, const char *filename) {
    this->filename = strdup(filename);
    this->mapAttributes = ENull;
    this->bmp = ENull;
    for (TInt i = 0; i < MAX_LEVELS; i++) {
        mLevels[i] = new LayerInfo;
    }

    char work[MAX_STRING_LENGTH], resourceFn[MAX_STRING_LENGTH];
    sprintf(work, "%s/%s", path, filename);

    RawFile txt(work);
    if (!txt.alive) {
        Panic("TileMap: Can't open %s for reading\n", work);
    }
    while (txt.ReadLine(work)) {
#ifdef DEBUG
        printf("  Processing %s\n", work);
#endif
        if (!strlen(work)) {
            continue;
        }
        //
        char *ptr = strrchr(work, '\\');
        if (ptr == ENull) {
            ptr = work;
        } else {
            ptr++;
        }
        strcpy((char *)filename, (const char *)ptr);

        sprintf(resourceFn, "%s/%s", resourceFile.path, ptr);
//    printf("LAYER %s\n", resourceFn);
        const char *extension = &ptr[strlen(ptr) - 3];
        if (!strcasecmp(extension, "bmp")) {
#ifdef DEBUGME
            printf("-> %s\n", ptr);
#endif
            this->bmp = new BMPFile(resourceFn);
        } else if (!strcasecmp(extension, "tlc")) {
            printf("  TLC FILE %s\n", ptr);
            this->mapAttributes = new RawFile(resourceFn);
        } else if (!strcasecmp(extension, "stm")) {
            if (strcasestr(ptr, "MAP_LAYER") != ENull) {
                TInt level = ParseLayer(ptr);
                printf("  MAP LAYER %s %d\n", ptr, level);
                mLevels[level]->used = ETrue;
                mLevels[level]->map = new RawFile(resourceFn);
            } else if (strcasestr(ptr, "MAP_ATTRIBUTES_LAYER") != 0 || strcasestr(ptr, "MAP_ATTRIBUTE_LAYER") == 0) {
                TInt level = ParseLayer(ptr);
                printf("  MAP ATTRIBUTES %s %d\n", ptr, level);
                mLevels[level]->used = ETrue;
                mLevels[level]->map_attributes = new RawFile(resourceFn);
            } else if (strcasestr(ptr, "OBJECT_LAYER")) {
                TInt level = ParseLayer(ptr);
                printf("  OBJECT LAYER %s %d\n", ptr, level);
                mLevels[level]->used = ETrue;
                mLevels[level]->object = new RawFile(resourceFn);
            } else if (strcasestr(ptr, "OBJECT_ATTRIBUTES_LAYER") != 0 || strcasestr(ptr, "OBJECT_ATTRIBUTE_LAYER") == 0) {
                TInt level = ParseLayer(ptr);
                printf("  OBJECT ATTRIBUTES LAYER %s %d\n", ptr, level);
                mLevels[level]->used = ETrue;
                mLevels[level]->object_attributes = new RawFile(resourceFn);
            } else {
                printf("-> %s IGNORED (for now)\n", ptr);
            }
        } else {
            Panic("*** TileMap: unknown extension: %s\n", ptr);
        }
    }
}

TileMap::~TileMap() {
    //
    delete this->filename;
    for (TInt i=0; i<MAX_LEVELS; i++) {
        delete mLevels[i];
    }
}

struct MapData {
    char token[4];
    TUint16 width, height;
    TUint32 data[];
};

static void make_filename(char *dst, const char *src) {
    while (*src) {
        if (!strncasecmp(src, "FILELIST.TXT", 12)) {
            src += 12;
        } else {
            *dst++ = *src++;
        }
    }
    *dst = '\0';
}

void TileMap::Write(ResourceFile &resourceFile) {
    if (!mapAttributes) {
        Panic("*** TileMap (%s) missing TLC\n:", filename);
    }
    if (!bmp) {
        Panic("*** TileMap (%s) missing BMP\n:", filename);
    }

    // write out the BMP
    char work[MAX_STRING_LENGTH];
    make_filename(work, filename);
    strcat(work, "BMP");
    TUint16 bmp_id = resourceFile.StartResource(work);
    bmp->Write(resourceFile);

    auto *tlc = (TUint16 *) mapAttributes->data;
    for (TInt i = 0; i < MAX_LEVELS; i++) {
        LayerInfo *layer = mLevels[i];
        if (!layer->used) {
            continue;
        }
        if (!layer->map || !layer->map_attributes || !layer->object || !layer->object_attributes) {
            Panic("*** Level %s %d is missing layers\n", work, i);
        }
        auto *map = (MapData *) layer->map->data;
        auto *map_attributes = (MapData *) layer->map_attributes->data;
        auto *object = (MapData *) layer->object->data;
        auto *object_attributes = (MapData *) layer->object_attributes->data;
        TInt32 width = map->width,
                height = map->height;

        TUint32 *map_data = &map->data[0],
                *map_data_attributes = &map_attributes->data[0];
        TUint32 *object_data = &object->data[0],
                *object_data_attributes = &object_attributes->data[0];

//    printf("  LAYER %s Level %d is %dx%d", filename, i, map->width, map->height);

        // We set the attributes bits in the map so the game can fiddle the individual tiles' bits during play.
        for (TInt n = 0; n < map->width * map->height; n++) {
            map_data[n] |= TUint32(tlc[map_data[n]] << TUint32(16));
            map_data[n] |= TUint32(tlc[map_data_attributes[n]]) << TUint16(16);
        }

        TUint16 objectCount = 0;
        for (TInt row = 0; row < height; row++) {
            for (TInt col = 0; col < width; col++) {
                const TInt index = row * width + col;
                const TUint32 tile = LOWORD(object_data[index]);
                const TUint32 attr1 = tlc[tile];
                const TUint32 tile2 = LOWORD(object_data_attributes[index]);
                const TUint32 attr2 = tlc[tile2];
                const TUint32 attr = attr2 << TUint32(16) | attr1;
                if (tile && attr) {
                    objectCount++;
                }
            }
        }

//    printf("  (Found %d objects)\n", objectCount);

        BObjectProgram objectProgram[objectCount],
                *ip = &objectProgram[0];

        if (objectCount) {
            for (TInt row = 0; row < height; row++) {
                for (TInt col = 0; col < width; col++) {
                    const TInt index = row * width + col;
                    const TUint32 tile = LOWORD(object_data[index]);
                    const TUint32 attr1 = tlc[tile];
                    const TUint32 tile2 = LOWORD(object_data_attributes[index]);
                    const TUint32 attr2 = tlc[tile2];
                    const TUint32 attr = attr2 << TUint32(16) | attr1;

                    if (tile && attr) {
#ifdef DEBUGME
                        printf("found tile:%d tile2:%d at row,col:%d,%d attr1:%d attr2:%d attr:%08x\n", tile, tile2, row, col,
                   attr1, attr2, attr);
#endif
                        ip->mCode = attr;
                        ip->mRow = row;
                        ip->mCol = col;
                        ip++;
                    }
                }
            }
        }

        make_filename(work, filename);
        sprintf(&work[strlen(work)], "LEVEL%d_MAP", i);

        resourceFile.StartResource(work);
        resourceFile.Write(&bmp_id, sizeof(bmp_id));
        resourceFile.Write(&objectCount, sizeof(objectCount));
        resourceFile.Write(&objectProgram[0], sizeof(BObjectProgram) * objectCount);

        resourceFile.Write(&map->width, sizeof(map->width));
        resourceFile.Write(&map->height, sizeof(map->height));
        resourceFile.Write(&map_data[0], width * height * sizeof(TUint32));
    }
//  printf("objectProgram %d\n", sizeof(objectProgram));
}
