#ifndef HEADERS_H
#define HEADERS_H

#include <fcntl.h>
#include <unistd.h>
#include <sys/types.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <stdint.h>
#include <elf.h>
#include <string.h>

int copy_binary(int source_file);
off_t getFileSize(int fd);
int	ft_memcmp(const void *s1, const void *s2, size_t n);
int obfuscate(void);

#endif