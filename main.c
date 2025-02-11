#include <stdlib.h>
#include <stdio.h>

#define KSH_RL_BUFSIZE 1024

char *kshReadLine(void){
    int bufsize = KSH_RL_BUFSIZE;
    int position = 0;
    char* buffer = malloc(sizeof(char) * bufsize);
    int c;

    if(!buffer){
        printf("ksh: \nerror: memory allocation failure");
        exit(EXIT_FAILURE);
    }

    while (1)
    {
        c = getchar();
        if(c == EOF || c == "\n"){
            buffer[position] = "\0";
            return buffer;
        }
        else{
            buffer[position] = c;
        }
        position++;
        if (position >= bufsize)
        {
            bufsize += KSH_RL_BUFSIZE;
            buffer = realloc(buffer, bufsize);
             if(!buffer){
                printf("ksh: \nerror: memory allocation failure");
                exit(EXIT_FAILURE);
            }
        }
        
    }
    
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