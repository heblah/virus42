#include "headers.h"

// About off_t type => https://stackoverflow.com/questions/9073667/where-to-find-the-complete-definition-of-off-t-type

off_t getFileSize(int fd) {
//    int fd = open(filename, O_RDONLY);
//    if (fd == -1) {
//        perror("open");
//        return -1;
//    }

   off_t size = lseek(fd, 0, SEEK_END);
   if (size == -1) {
       perror("lseek");
       printf("debug lseek %ld\n", size);
       printf("test lseek\n");
       return -1;
   }

//    if (close(fd) == -1) {
//        perror("close");
//        return -1;
//    }

   return size;
}
