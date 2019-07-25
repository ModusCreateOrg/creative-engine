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

#ifndef O_BINARY
#define O_BINARY (0)
#endif

void usage(int ac, char *av[]) {
  printf("Usage:\n");
  printf("\t%s <stm file>\n", av[0]);
  printf("Dumps an stm tile map file\n");
  exit(1);
}

int main(int ac, char *av[]) {
  if (ac < 2) {
    usage(ac, av);
  }

  int fd = open(av[1], O_RDONLY | O_BINARY);
  ssize_t size = lseek(fd, 0, 2);
  lseek(fd, 0, 0);
  unsigned char data[size];
  read(fd, &data[0], size);
  close(fd);

  struct STMPFILE {
    char stmp[4];
    short width;
    short height;
    unsigned long entries[];
  } *map = (STMPFILE *) &data[0];

  if (strncmp(map->stmp, "STMP", 4)) {
    printf("%s is not an stmp file\n", av[1]);
    exit(1);
  }

  printf("%s is %d by %d tiles\n", av[1], map->width, map->height);

  unsigned long *entries = &map->entries[0];
  for (int h = 0; h < map->height; h++) {
    for (int w = 0; w < map->width; w++) {
     printf("%04x ", *entries++);
    }
    printf("\n");
  }
  exit(0);
}
