#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <string.h>
#include <stdint.h>
#include <inttypes.h>

#define MAX_OUTPUT 1024

int64_t lineCount(const char *line){
        int64_t count = 0;
        sscanf(line, "%*[^:]: %" SCNd64, &count);
        return count;
}

int main(int argc, char *argv[]){
        int fileNum = argc - 1;
        int pipes[fileNum][2];
        pid_t pids[fileNum];
        int lines = 0;

        if(argc < 2){
                fprintf(stderr, "Usage: %s file1 file2\n", argv[0]);
                return 1;
        }
        for(int i = 0; i < fileNum; ++i){
                if(pipe(pipes[i]) < 0){
                        printf("Error with pipe");
                        return 1;
                }
                //create fork
                pids[i] = fork();
                if(pids[i] < 0){
                        printf("Error with fork");
                        return 1;
                }
                if(pids[i] == 0){
                        //close pipes and redirect
                        close(pipes[i][0]);
                        dup2(pipes[i][1], STDOUT_FILENO);
                        close(pipes[i][1]);
                        execl("./lc1", "lc1", argv[i+1], (char *)NULL);
                        printf("Error");
                        exit(1);
                }
                //close parent process
                else{
                        close(pipes[i][1]);
                }
        }

        for(int i = 0; i < fileNum; ++i){
                char buffer[MAX_OUTPUT];
                FILE *stream = fdopen(pipes[i][0], "r");
                //update and print
                if(fgets(buffer, sizeof(buffer), stream) != NULL){
                        fputs(buffer, stdout);
                        int64_t count = lineCount(buffer);
                        lines += count;
                }
                fclose(stream);
        }
        for(int i = 0; i < fileNum; ++i){
                waitpid(pids[i], NULL, 0);
        }
        //print total number of lines in all files
        printf("Total lines: %" PRId64 "\n", lines);
        return 0;
}