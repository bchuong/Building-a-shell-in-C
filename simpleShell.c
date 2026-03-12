#include <stdio.h>	
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>


void shell_loop();
char* read_line();
char** split_command(char* command);
int execute_command(char** args);
int launch_shell(char** args);

int main(int argc, char **argv){
    shell_loop();
    return 0;
}

void shell_loop(){
    int status = 1;
    char* command;
    char** args;
    char shellName[10] = "Csh> ";
    do{
        write(STDERR_FILENO, shellName, sizeof(shellName));
        command = read_line();
        args = split_command(command);
        status = execute_command(args);

        for (int i = 0; args[i] == NULL; i++){
            free(args[i]);
        }
        free(args);
        free(command);
    }while (status);
}

char* read_line(){ //TODO: try implementing getline from scratch
    ssize_t count = 0;
    char* buffer = NULL;
    if (getline(&buffer, &count, stdin) == -1){
        if (feof(stdin)){
            exit(EXIT_SUCCESS);
        }else{
            perror("readline");
            exit(EXIT_FAILURE);
        }
    }
    //delete /n at the end of the string output
    ssize_t bufferSize = strlen(buffer);
    if (bufferSize > 0 && buffer[bufferSize - 1] == '\n'){
        buffer[bufferSize - 1] = '\0';
    }

    return buffer;
}

char** split_command(char* command){
    int commandSize = 64;
    //initialize argument string vector
    char** args = malloc(commandSize * sizeof(char*));
    for (int i = 0; i < commandSize; i++){
        //initialize each args[i] is an array of string
        args[i] = malloc(commandSize * sizeof(char));
        args[i][0] = '\0';
    }
    //split command into string vector
    int indexCommand = 0, j = 0;
    int lenCommand = strlen(command);
    for (int i = 0; i < lenCommand; i++){
        if (command[i] != ' '){
        //if not whitespace, add command into args array
            args[indexCommand][j] = command[i];
            j++;
        }else {
            if (j > 0){ //Avoid empty \0
                args[indexCommand][j] = '\0';
                indexCommand++;
                j = 0;
            }
            //Delete all the next whitespace
            while (i + 1 < lenCommand && command[i + 1] == ' '){
                i++; 
            }
        }
    }
    //add \0 for the last word
    if (j > 0){
        args[indexCommand][j] = '\0';
    }
    //Mark the end of the CommandArray
    args[indexCommand + 1] = NULL;
    //print tokenizer
    for (int i = 0; args[i] != NULL; i++){
        printf("args[%d] = %s\n", i, args[i]);
    }
    return args;
}
int launch_shell(char** args){
    __pid_t pid, wpid;
    int status;
    pid = fork();
    if (pid == 0){
        //child process
        if(execvp(args[0], args) == -1){ 
            //execvp only return value if meet error
            perror("execvp");
        }
        exit(EXIT_FAILURE);
    }else if (pid == -1){
        perror("Failed to create child process");
    }else { //pid > 1
        do {
            wpid = waitpid(pid, &status, WUNTRACED);
    } while (!WIFEXITED(status) && !WIFSIGNALED(status));
    }
    return 1;
}
int execute_command(char** args){
    return launch_shell(args);

}
int returnOne(){ return 1;}
