#include <stdio.h>
#include <string.h>
#include <wait.h>
#include <unistd.h>
#include <stdlib.h>

void main(){
    int ret, status;
    char path[] = "/bin/";

    while(1){
        char input[256];
        char *command;
        char *token;
        char *words[256];
        char **arguments;

        printf("$> ");        
        fgets(input, 256, stdin);
        input[strcspn(input, "\n")] = 0;

        token = strtok(input, " ");
        int count = 0;
        words[count] = token;

        while(token != NULL){
            token = strtok(NULL, " ");
            count++;
            words[count] = token;
        }

        arguments = (char **) malloc((count+1) * sizeof(char *));
        command = strtok(input, " ");
        arguments[0] = command;

        for (int j = 0; j < count+1; j++){
            arguments[j] = words[j];
        }

        if(strcmp(command,"exit") == 0){
            break;
        }
        
        ret = fork();

        if(ret == 0){
            strcat(path,command);
            execv(path, arguments); 
            exit(127);          
        }
        else{
            wait(&status);
            if(WIFEXITED(status)){
                
                int status_ = WEXITSTATUS(status);
                if(status_ == 0){
                    printf("Executado com sucesso.\n");
                }
                else if(status_ == 127){
                    printf("Comando não encontrado.\n");
                }
                else{
                    printf("Código de retorno = %d\n", status_);
                }
            }
        }
        
    }   

}