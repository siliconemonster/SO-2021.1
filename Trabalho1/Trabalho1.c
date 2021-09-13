/*
Trabalho 1
Integrantes:
Aline Freire de Rezende - 116110571;
Gilberto Lopes Inácio Filho - 115173699;
Leticia Tavares da Silva - 117210390; 
*/


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#define limite_processos 4
#define FATIA 2


// Estrutura de uma fila
struct Fila {

	int elementos[limite_processos]; // Elementos serao ids dos procesoss
	int primeiro; // Primeiro elemento da fila
	int ultimo; // Ultimo elemento da fila
    int num_Processos; // Numero de processos na fila
};

// Funcoes para as filas

// Cria uma fila
void criarFila( struct Fila *f) { 

	f->primeiro = 0;
	f->ultimo = -1;
    f-> num_Processos = 0;

};

// Insere elemento em uma fila
void inserir(struct Fila *f, int v) {

	if(f->ultimo == limite_processos - 1)
		f->ultimo = -1;

	f->ultimo++;
	f->elementos[f->ultimo] = v;
    f-> num_Processos++;

};

// Remove elemento em uma fila e o retorna
int remover( struct Fila *f ) { 

	int temp = f->elementos[f->primeiro++];
    f-> num_Processos--;

	if(f->primeiro == limite_processos)
		f->primeiro = 0;
   
	return temp; // Retorna o elemento retirado

};

// Verifica se uma fila estah vazia
// Retorna 1 se sim e 0 se nao
int fila_esta_vazia( struct Fila *f ) { 

	if(f-> num_Processos == 0){
        return 1;
    }
		
	return 0;

};



// Processos
struct Processo {

    int pid; // Identificador do processo
    int temp_chegada; // Instante em que o processo chegou
	int temp_servico; // Tempo de servico do processo
    int temp_restante; // Quanto tempo de CPU ainda falta para o processo finalizar
    int inicio_io; // Tempo apos o inicio do servico em que o processo saira para i/o
    int temp_restante_inicio_io; // Quanto tempo de CPU ainda falta para o processo sair para i/o
	int tempo_servico_io; // Tempo de duracao da i/o do processo
    
};

// Funcoes para os processos
// Cria novo processo
void NovoProcesso( struct Processo *p, int temp_entrada, int id, int temp_chegada) { 
    
    p->pid = id;
    p->temp_chegada = temp_chegada;
    p->temp_servico = rand() %8;
    while (p->temp_servico == 0){
        p->temp_servico = rand() %8;
    }
    p->temp_restante = p->temp_servico;

    if (temp_entrada == -1){
        p->inicio_io = -1;
        p->temp_restante_inicio_io = -1;
        p->tempo_servico_io = -1;
    }
    else{
        p->inicio_io = 0;
        while(p->inicio_io == 0){
            p->inicio_io = rand() % p->temp_servico == 0;
        }

        p->temp_restante_inicio_io = p->inicio_io;
        p->tempo_servico_io = temp_entrada;
    }

};

int main() {


    srand(time(NULL)); // Para geracao de numero pseudo aleatorio
    
    // Lista com a duracao de cada tipo de i/o
    // 0 -> Disco 1-> Fita Magnetica 2-> Impressora
    int duracao_ios[] = {5,7,12};
    int processos_finalizados = 0; //contador de processos finalizados
    int instante = 0;  // Instante inicial tempo = 0s
    int id_processo = 0; // Identificador de cada processo, o primeiro é o 0
    int prox_chegada = 0; // Em quanto tempo chegará o próximo processo
    int tem_io; // Indica se o processo sai para i/o
    int tipo_io; // Indica qual eh o tipo de i/o
    int tempo_io; // Tempo que ainda resta de i/o do processo que esta executando
    int em_CPU = -1; // CPU se inicia vazia
    int em_io = -1; // IO se inicia vazia
    int cont_print; // contador para printar elementos de uma fila
    int indice; // Utilizado para print
    int tempo_restante_cpu; // tempo restante de CPU do processo que a esta usando

    // Processos
    struct Processo processos[limite_processos];  

    // Filas
    struct Fila fila_baixa_prio; 
    struct Fila fila_alta_prio; 
    struct Fila fila_io; 

    criarFila(&fila_alta_prio);
    criarFila(&fila_baixa_prio);
    criarFila(&fila_io);
 
    while(processos_finalizados < limite_processos){

        printf("\n~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n"); 
        printf("Instante %is \n\n", instante); 
        
        
        // Criação do processo
        if (instante == prox_chegada && id_processo < limite_processos) {

            tem_io = rand() %2; // 50% de ter que sair pra i/o

            // Se tiver i/o, gera aleatoriamente o tipo da i/o
            if (tem_io == 1){
                tipo_io = rand() %3;
                NovoProcesso(&processos[id_processo], duracao_ios[tipo_io], id_processo, instante);
            } 

            else{
                NovoProcesso(&processos[id_processo], -1, id_processo, instante);
            }           
            
            // Insere o novo processo na fila de alta prioridade
            inserir(&fila_alta_prio, id_processo);
            while(prox_chegada == instante){
                prox_chegada = instante + rand() %5;
            }
            // Prints
            printf("----------------------------------------\n"); 
            printf("Processo Id = %i criado\n", id_processo); 
            printf("Momento Chegada: %i\n", instante);
            printf("Tempo Servico: %i\n", processos[id_processo].temp_servico);
            printf("Tipo de I/O:");

            // Se o processo tiver i/o, printa qual eh
            if(processos[id_processo].tempo_servico_io != -1){
                if (processos[id_processo].tempo_servico_io == 5){
                printf(" Disco\n");
                }
                else if(processos[id_processo].tempo_servico_io == 7){
                    printf(" Fita Magnetica\n");
                }
                else {
                    printf(" Impressora\n");
                }
                printf("Tempo Servico de I/O: %i\n", processos[id_processo].tempo_servico_io);
                printf("Tempo Saida para I/O: %i\n", processos[id_processo].inicio_io);
            }            
            else{
                printf(" Nao sai pra I/O\n");
            }
            printf("----------------------------------------\n\n");

            id_processo += 1;           

        }

        // Se a CPU nao estah vazia
        if (em_CPU != -1){
            
            // Desconta o instante passado do tempo do Processo
            tempo_restante_cpu--;
            processos[em_CPU].temp_restante--;
            processos[em_CPU].temp_restante_inicio_io--;

            printf("Processo %i tem %is restante de CPU.\n" , em_CPU, tempo_restante_cpu);

            // Se o tempo de CPU do processo acabou
            if (tempo_restante_cpu == 0){

                // O tempo acabou porque o processo finalizou
                if (processos[em_CPU].temp_restante == 0){
                    printf("Processo %i finalizado.\n" , em_CPU);
                    processos_finalizados += 1;                
                }

                // O tempo acabou porque o processo saiu pra i/o
                else if (processos[em_CPU].temp_restante_inicio_io == 0){
                    printf("Processo %i sai da CPU e vai pra fila de I/O.\n" , em_CPU);
                    inserir(&fila_io, em_CPU);
                    processos[em_CPU].temp_restante_inicio_io = -1;                
                }

                // O tempo acabou porque a fatia acabou
                else{
                    printf("Processo %i perde CPU e sofre preempcao.\n" , em_CPU);
                    inserir(&fila_baixa_prio, em_CPU);
                }

                em_CPU = -1; // CPU fica vazia
            }
          
        }    

        // Sse CPU está vazia, verifica se tem processos em outras filas (de alta e de baixa) para pegar
        if (em_CPU == -1) {
            printf("CPU Vazia. \n");

            // Se a fila de alta prioridade esta vazia, pega da baixa prioridade caso ela nao esteja vazia
            // Caso as duas estejam vazias, CPUsegue vazia
            if (fila_esta_vazia(&fila_alta_prio) == 1){   
                if (fila_esta_vazia(&fila_baixa_prio) == 1){
                    printf("Nao ha processos nas filas, CPU contina vazia.\n");
                    
                }
                else{
                    em_CPU = remover(&fila_baixa_prio);
                    if (processos[em_CPU].temp_restante_inicio_io == 0){
                        inserir(&fila_io, em_CPU);
                        processos[em_CPU].temp_restante_inicio_io = -1;   
                        em_CPU = -1;
                    }  
                    else{
                        printf("Processo %i assume CPU\n", em_CPU);  
                    }
                                 
                }
            }
            // Caso a fila de alta prioridade nao esteja vazia, o primeiro da fila assume a CPU
            else{
                em_CPU = remover(&fila_alta_prio);
                if (processos[em_CPU].temp_restante_inicio_io == 0){
                    inserir(&fila_io, em_CPU);
                    processos[em_CPU].temp_restante_inicio_io = -1;   
                    em_CPU = -1;
                }  
                else{
                    printf("Processo %i assume CPU\n", em_CPU);  
                }
            }

            // Se algum processo assumiu a CPU, verifica por quanto tempo ele assumira a CPU
            if (em_CPU != -1){
                
                tempo_restante_cpu = FATIA;
                if (processos[em_CPU].temp_restante < tempo_restante_cpu){
                    tempo_restante_cpu = processos[em_CPU].temp_restante;
                }
                if (processos[em_CPU].temp_restante_inicio_io < tempo_restante_cpu &&  
                processos[em_CPU].temp_restante_inicio_io > 0){
                    tempo_restante_cpu = processos[em_CPU].temp_restante_inicio_io;
                }
                
            }

        }  

        // Se i/o nao estah sendo utilizada
        if (em_io == -1){
            
            // Se fila pra io nao estah vazia, primeiro da fila assume i/o
            if (fila_esta_vazia(&fila_io) == 0){   
                em_io = remover(&fila_io);
                printf("Processo %i entrou em I/O. \n", em_io);
                // Tempo de i/o do preocesso
                tempo_io = processos[em_io].tempo_servico_io;
            }
            
        } 
        
        // Se i/o está sendo utilizada
        else{
            tempo_io--;
            printf("Processo %i tem %is restante de I/O.\n" , em_io, tempo_io);
            
            // Se tempo de i/o do processo acabar, ele volta pra fila de alta ou baixa prioridade
            // Dependendo da i/o em que estava e i/o fica vazia
            if (tempo_io == 0){

                printf("Processo %i saiu de I/O. \n", em_io);

                if(processos[em_io].temp_restante != 0){
                    
                    if(processos[em_io].tempo_servico_io != 5){
                        inserir(&fila_alta_prio, em_io);
                        printf("Processo %i volta pra fila de alta prioridade. \n", em_io);
                    }
                    else{
                        inserir(&fila_baixa_prio, em_io);
                        printf("Processo %i volta pra fila de baixa prioridade. \n", em_io);
                    }
                    
                }
                else{
                    processos_finalizados += 1;
                    printf("Processo %i finalizado. \n", em_io);
                }
                // i/o fica vazia
                em_io = -1;
            }
        }       

        // Aumenta o instante em 1 segundo 
        instante += 1;

        // Printa as filas
        printf("\nFila Alta Prioridade: ");
        indice = fila_alta_prio.primeiro;
        for (cont_print = 0; cont_print < fila_alta_prio.num_Processos; cont_print++){
            printf("%i ", fila_alta_prio.elementos[indice++]);
        }
        printf("\nFila Baixa Prioridade: ");
        indice = fila_baixa_prio.primeiro;
        for (cont_print = 0; cont_print < fila_baixa_prio.num_Processos; cont_print++){
            printf("%i ", fila_baixa_prio.elementos[indice++]);
        }
        printf("\nFila I/O: ");
        indice = fila_io.primeiro;
        for (cont_print = 0; cont_print < fila_io.num_Processos; cont_print++){
            printf("%i ", fila_io.elementos[indice++]);
        }
        printf("\n");
        
    }

    printf("Acabou");

   
}