#include "headers.h"

// About off_t type => https://stackoverflow.com/questions/9073667/where-to-find-the-complete-definition-of-off-t-type

off_t getFileSize(int fd) {

   off_t size = lseek(fd, 0, SEEK_END);
   if (size == -1) {
       perror("lseek");
       return -1;
   }

   return size;
}
