#include "headers.h"

int copy_binary(int source_file) {

    // const char* source_file = argv[1];
    const char* destination_file = "woody";

   // Open the source file
//    int source_fd = open(source_file, O_RDONLY);
//    if (source_fd == -1) {
//        perror("open");
//        return 1;
//    }

   // Get the size of the source file
//    struct stat st;
//    if (fstat(source_fd, &st) == -1) {
//        perror("fstat");
//        return 1;
//    }

    // printf("debug source fd: %d\n", source_file);
   ssize_t file_size = getFileSize(source_file);

   // Map the source file into memory
   void* source_data = mmap(NULL, file_size, PROT_READ, MAP_PRIVATE, source_file, 0);
   if (source_data == MAP_FAILED) {
       perror("mmap");
       return 1;
   }

   // Close the source file
   close(source_file);

   // Open the destination file
   int destination_fd = open(destination_file, O_WRONLY | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR);
   if (destination_fd == -1) {
       perror("open");
       return 1;
   }

   // Write the source data to the destination file
   if (write(destination_fd, source_data, file_size) != file_size) {
       perror("write");
       return 1;
   }

   // Close the destination file
   close(destination_fd);

   // Unmap the source data
   if (munmap(source_data, file_size) == -1) {
       perror("munmap");
       return 1;
   }

   return 0;
}