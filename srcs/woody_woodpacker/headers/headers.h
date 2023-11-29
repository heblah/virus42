#ifndef HEADERS_H
#define HEADERS_H

#include <fcntl.h>
#include <unistd.h>
#include <sys/types.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/stat.h>
#include <sys/mman.h>

int copy_binary(int source_file);
off_t getFileSize(int fd);

#endif