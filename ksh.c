#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <bits/waitflags.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h> 

#define KSH_TOK_BUFSIZE 64
#define KSH_TOK_DELIM " \t\r\n\a"
#define KSH_SUCCESS 0
#define KSH_FAILURE 1

char *prompt = "> ";

int kshCd(char **args);
int kshExit(char **args);
int kshHelp(char **args);
int kshPrompt(char **args);

char *builtinStr[] = {
    "cd",
    "exit",
    "help",
    "setprompt"
};

int (*builtinFunc[]) (char **) = {
    &kshCd,
    &kshExit,
    &kshHelp,
    &kshPrompt
};

int kshNumBuiltins() {
  return sizeof(builtinStr) / sizeof(char *);
}

int kshCd(char **args){
    if (args[1] == NULL)
        fprintf(stderr, "ksh: expected argument to \"cd\"\n");
    else if(chdir(args[1]) != 0)
        perror("ksh");
    return KSH_SUCCESS;    
}

int kshExit(char **args)
{
    return KSH_FAILURE;
}

int kshHelp(char **args){
    printf("Kushagra's KSH\n");
    printf("Type program names and arguments, and hit enter.\n");
    printf("The following are built in:\n");

    for (int i = 0; i < kshNumBuiltins(); i++) {
        printf("  %s\n", builtinStr[i]);
    }

    printf("Use the man command for information on other programs.\n");
    return KSH_SUCCESS;
}

int kshPrompt(char **args){
    if (args[1] == NULL){
        fprintf(stderr, "ksh: expected argument to \"setprompt\"\n");
        return KSH_FAILURE;
    } else{
        prompt = strdup(args[1]);
        if(!prompt){
            fprintf(stderr, "ksh: allocation failure");
            return KSH_FAILURE;
        }
        return KSH_SUCCESS;
    }
}

char *kshReadLine(void){
    char *line = NULL;
    __ssize_t bufsize = 0;
    if(getline(&line, &bufsize, stdin) == -1){
        if(feof(stdin))
            exit(EXIT_SUCCESS);
        else{
            perror("ksh: readline");
            exit(EXIT_FAILURE);
        }
    }
    return line;
}

char **kshSplitLine(char *line){
    int bufsize = KSH_TOK_BUFSIZE, position = 0;
    char **tokens = malloc(bufsize * sizeof(char*));
    char *token;

    if(!tokens){
        fprintf(stderr, "ksh: allocation failure\n");
        exit(EXIT_FAILURE);
    }

    token = strtok(line, KSH_TOK_DELIM);
    while (token != NULL){
        tokens[position] = token;
        position++;

        if(position >= bufsize){
            bufsize += KSH_TOK_BUFSIZE;
            tokens = realloc(tokens, sizeof(char*) * bufsize);
            if(!tokens){
                fprintf(stderr, "ksh: allocation failure\n");
                exit(EXIT_FAILURE);
            }
        }
        token = strtok(NULL, KSH_TOK_DELIM);
    }
    tokens[position] = NULL;
    return tokens;
}

int kshLaunch(char **args){
    __pid_t pid, wpid;
    int status;

    pid = fork();
    if(pid == 0){
        if(execvp(args[0], args) == -1)
            perror("ksh");
        exit(EXIT_FAILURE);
    } else if(pid < 0)
        perror("ksh");
    else{
        do {
            wpid = waitpid(pid, &status, WUNTRACED);
        } while (!WIFEXITED(status) && !WIFSIGNALED(status));
    }
    return KSH_SUCCESS;
}

int kshExecute(char **args){
    if(args[0] == NULL)
        return KSH_SUCCESS;
    for (int i = 0; i < kshNumBuiltins(); i++) {
        if (strcmp(args[0], builtinStr[i]) == 0) 
            return (*builtinFunc[i])(args);
    }
    return kshLaunch(args);
}

void kshLoop(void){
    char* line;
    char** args;
    int status;

    do {
        printf("%s", prompt);
        line = kshReadLine();
        args = kshSplitLine(line);
        status = kshExecute(args);

        free(line);
        free(args);
    } while (status == KSH_SUCCESS);
    
}

int main(int argc, char **argv){
    kshLoop();
    return EXIT_SUCCESS;
}