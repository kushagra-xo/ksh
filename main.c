#include <stdlib.h>
#include <stdio.h>

char *kshReadLine(void){
    char *line = NULL;
    __ssize_t bufsize = 0;
    if(getline(&line, &bufsize, stdin) == -1){
        if(feof(stdin))
            exit(EXIT_SUCCESS);
        else{
            perror("readline");
            exit(EXIT_FAILURE);
        }
    }
    return line;
}

void kshLoop(void){
    char* line;
    char** args;
    int status;

    do
    {
        printf("> ");
        line = kshReadLine();
        args = kshSplitLine(line);
        status = kshExecute(args);

        free(line);
        free(args);
    } while (status);
    
}

int main(int argc, char **argv){
    kshLoop();
    return EXIT_SUCCESS;
}