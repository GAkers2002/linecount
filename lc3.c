#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <inttypes.h>
#include <pthread.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>

#define BUFF_SIZE 65536

int64_t total = 0;
pthread_mutex_t locks = PTHREAD_MUTEX_INITIALIZER;

//define struct for number of lines in file
typedef struct {
        const char *file;
        int64_t lines;
}Info;

void *countLines(void *arg){
        char buffer[BUFF_SIZE];
        ssize_t readBytes;
        int64_t count = 0;
        Info *info = (Info *)arg;

        //open file
        int f = open(info->file, O_RDONLY);
        if(f < 0){
                printf("Error opening file");
                return NULL;
        }
        //update count in file
        while((readBytes = read(f, buffer, sizeof(buffer))) > 0){
                for(ssize_t i = 0; i < readBytes; i++){
                        if(buffer[i] == '\n'){
                                count++;
                        }
                }
        }
        //close file and update the number of lines with count
        close(f);
        info->lines = count;
        printf("%s: %" PRId64 " lines\n", info->file, count);

        //update global total
        pthread_mutex_lock(&locks);
        total += count;
        pthread_mutex_unlock(&locks);
        return NULL;
}

int main(int argc, char *argv[]){
        //if missing arguments show usage
        if(argc < 2){
                fprintf(stderr, "Usage: %s file1 file2\n", argv[0]);
                return 1;
        }

        int fileNum = argc - 1;
        pthread_t *threads = malloc(fileNum * sizeof(pthread_t));
        Info *info = malloc(fileNum * sizeof(Info));
        if(!threads || !info){
                printf("malloc error");
                return 1;
        }
        //create threads
        for(int i = 0; i < fileNum; i++){
                info[i].file = argv[i+1];
                info[i].lines = 0;
                if(pthread_create(&threads[i], NULL, countLines, &info[i]) != 0){
                        printf("Error creating thread");
                        return 1;
                }
        }
        //wait for threads to finish
        for(int i = 0; i < fileNum; i++){
                pthread_join(threads[i], NULL);
        }
        free(threads);
        free(info);

        //print results
        printf("Total lines: %" PRId64 "\n", total);
        return 0;
}

