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
        
        printf("$> ");
        fgets(input, 256, stdin);
        input[strcspn(input, "\n")] = 0;

        command = strtok(input, " ");

        if(strcmp(command,"exit") == 0){
            break;
        }
        
        ret = fork();

        if(ret == 0){
            strcat(path,command);            
            execl(path, command, NULL); 
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