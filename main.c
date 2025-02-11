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

}