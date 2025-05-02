#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdint.h>
#include <inttypes.h>

#define BUFFSIZE 65536

int64_t countLines(const char *file){
        char buffer[BUFFSIZE];
        ssize_t readBytes;
        int64_t lines = 0;

        //open file
        int f = open(file, O_RDONLY);
        if(f < 0){
                printf("Error opening file");
                return -1;
        }
        //iterate through file and count number of lines
        while((readBytes = read(f, buffer, BUFFSIZE)) > 0){
                for(ssize_t i = 0; i < readBytes; ++i){
                        if(buffer[i] == '\n'){
                                lines++;
                        }
                }
        }
        if(readBytes < 0){
                printf("Error reading file");
                close(f);
                return -1;
        }
        close(f);
        return lines;
}

int main(int argc, char *argv[]){
        int64_t lines = 0;

        if(argc < 2){
                fprintf(stderr, "Usage: %s file1 file2\n", argv[0]);
                return 1;
        }
        for(int i = 1; i < argc; ++i){
                int64_t count = countLines(argv[i]);
                //number of lines in file
                if(count >= 0){
                        printf("%s: %" PRId64 " lines\n", argv[i], count);
                        lines += count;
                }
        }
        //return total number
        if(argc > 2){
                printf("Total lines: %" PRId64 "\n", lines);
        }
        return 0;
}


