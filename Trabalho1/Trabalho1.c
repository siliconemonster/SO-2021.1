/*
Trabalho 1
Integrantes:
Aline Freire de Rezende - 116110571;
Gilberto Lopes Inácio Filho - 115173699;
Letícia Tavares da Silva - 117210390; */


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#define limite_processos 3
#define FATIA 2


// Estrutura de uma fila
struct Fila {

	int elementos[limite_processos]; // Elementos serao ids dos procesoss (?)
	int primeiro; // primeiro elemento da fila
	int ultimo; // ultimo elemento da fila
    int num_Processos; 
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
   
	return temp; // retorna o elemento retirado

};

// Remove elemento em uma fila e o retorna
int fila_esta_vazia( struct Fila *f ) { 

	if(f-> num_Processos == 0){
        return 1;
    }
		
	return 0;

};



// Processos
struct Processo {

    int pid; // identificador do processo
    int temp_chegada; // instante em que o processo chegou
	int temp_servico; // tempo de servico processo chegou
    int temp_restante;
    int temp_restante_inicio_io;
	int tempo_servico_io; // tempo de duracao da i/o do processo
    int tempo_restante_em_CPU; // tempo restante do processo em CPU
    int inicio_io; // tempo apos o inicio do servico em que o processo saira para i/o
    
};

// Funcoes para os processos
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
        p->inicio_io = rand() % p->temp_servico;
        p->temp_restante_inicio_io = p->inicio_io;
        p->tempo_servico_io = temp_entrada;
    }

};

int calcula_tipo_saida_CPU(struct Processo *p){
    int tipo_saida;

    // i/o
    if (p->temp_restante_inicio_io < FATIA && p->temp_restante_inicio_io >= 0){
        tipo_saida = 0; // é i/o
    }
    else{
        // acaba
        if (p->temp_restante <= FATIA){
            tipo_saida = -1; // acaba
        }
        // preempcao
        else{
            tipo_saida = 1; // é preempcao
        }
    }
    
    return tipo_saida;
}

int calcula_tempo_restante_em_CPU(struct Processo *p){
    int tempo_processo_CPU;

    // i/o
    if (p->temp_restante_inicio_io < FATIA && p->temp_restante_inicio_io >= 0){
        tempo_processo_CPU = p->temp_restante_inicio_io;
        p->temp_restante_inicio_io = -1;
        p->temp_restante = p->temp_restante - tempo_processo_CPU;
    }
    else{
        // acaba
        if (p->temp_restante <= FATIA){
            tempo_processo_CPU = p->temp_restante;
            p->temp_restante = -1;
        }
        // preempcao
        else{
            tempo_processo_CPU = FATIA;
            p->temp_restante = p->temp_restante - tempo_processo_CPU;
            p->temp_restante_inicio_io = p->temp_restante_inicio_io - tempo_processo_CPU;
        }
    }

    return tempo_processo_CPU;
}

int aloca_processo_io(struct Processo *p){
    int tempo_io = p -> tempo_servico_io;

    return tempo_io;
}

int main() {


    srand(time(NULL)); // para geracao de numero pseudo aleatorio
    
    // Lista com a duracao de cada tipo de I/O
    // 0 -> Disco 1-> Fita Magnetica 2-> Impressora
    int duracao_ios[] = {5,7,12};
    int tipo_io; 
    int processos_finalizados = 0; //contador de processos finalizados
    int instante = 0;  // Instante inicial tempo = 0s
    int id_processo = 0; // Identificador de cada processo, o primeiro é o 0
    int prox_chegada = 0; // Em quanto tempo chegará o próximo processo
    int tem_io; // Indica se o processo sai para IO
    int em_CPU = -1; // CPU se inicia vazia
    int em_io = -1; // IO se inicia vazia
    int tipo_saida_CPU;
    int tempo_io;
    int cont_print;
    int indice;
    int acabou;

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
        
        
        // criação do processo
        if (instante == prox_chegada && id_processo < limite_processos) {

            tem_io = rand() %2;

            if (tem_io == 1){
                tipo_io = rand() %3;
                NovoProcesso(&processos[id_processo], duracao_ios[tipo_io], id_processo, instante);
            } 

            else{
                NovoProcesso(&processos[id_processo], -1, id_processo, instante);
            }           
            
            inserir(&fila_alta_prio, id_processo);
            while(prox_chegada == instante){
                prox_chegada = instante + rand() %5;
            }

            printf("----------------------------------------\n\n"); 
            printf("Processo Id = %i criado\n", id_processo); 
            printf("Momento Chegada: %i\n", instante);
            printf("Tempo Servico: %i\n", processos[id_processo].temp_servico);
            printf("Tipo de I/O:");
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

        // Se a CPU não está vazia
        if (em_CPU != -1){
            processos[em_CPU].tempo_restante_em_CPU--;
            
            if (processos[em_CPU].temp_restante != -1){ // antes não garantia que o tempo não ia descer além se fosse o instante final
                printf("tempo restante do processo %d::::::::::::::::::: %d\n", em_CPU, processos[em_CPU].temp_restante);
                printf("Processo %i tem %is restante de CPU.\n" , em_CPU, processos[em_CPU].tempo_restante_em_CPU);
                // se o processo perde a CPU, insere na fila de baixa prioridade ou de io ou termina
                if(processos[em_CPU].tempo_restante_em_CPU == 0){ // tempo restante em CPU = 0
                    
                    
                    if (tipo_saida_CPU == 1){ // preempção
                        printf("Processo %i perde CPU e sofre preempcao.\n" , em_CPU);
                        inserir(&fila_baixa_prio, em_CPU);
                    }
                    else if (tipo_saida_CPU == 0){ // i/o
                        printf("Processo %i sai da CPU e vai pra fila de I/O.\n" , em_CPU);
                        inserir(&fila_io, em_CPU);

                    }
                    else{
                        printf("Processo %i finalizado.\n" , em_CPU);
                        processos_finalizados += 1;
                    }
                    em_CPU = -1; // CPU fica vazia
                }
            }
            else{ 
                // processo perde a CPU
                printf("Processo %i finalizado.\n" , em_CPU);
                processos_finalizados += 1;
                em_CPU = -1;

                /* é possível que o erro de repetir processos na fila esteja aqui*/
            }
            
        }    

        // se CPU está vazia, verifica se tem processos em outras filas para pegar
        if (em_CPU == -1) {
            printf("CPU Vazia. \n");

            if (fila_esta_vazia(&fila_alta_prio) == 1){   
                if (fila_esta_vazia(&fila_baixa_prio) == 1){
                    printf("Nao ha processos nas filas, CPU contina vazia.\n");
                    em_CPU = -1;
                }
                else{
                    em_CPU = remover(&fila_baixa_prio);
                    printf("tempo restante do processo %d::::::::::::::::::: %d\n", em_CPU, processos[em_CPU].temp_restante);
                    tipo_saida_CPU = calcula_tipo_saida_CPU(&processos[em_CPU]);
                    processos[em_CPU].tempo_restante_em_CPU = calcula_tempo_restante_em_CPU(&processos[em_CPU]);
                    
                    if (tipo_saida_CPU == -1){
                        processos[em_CPU].temp_restante = -1;
                    }
                    if (tipo_saida_CPU == 0){
                        processos[em_CPU].temp_restante_inicio_io = -1;
                    }

                    printf("Processo %i assume CPU\n", em_CPU);
                }
            }
            else{
                em_CPU = remover(&fila_alta_prio);
                printf("tempo restante do processo %d::::::::::::::::::: %d\n", em_CPU, processos[em_CPU].temp_restante);
                tipo_saida_CPU = calcula_tipo_saida_CPU(&processos[em_CPU]);
                processos[em_CPU].tempo_restante_em_CPU = calcula_tempo_restante_em_CPU(&processos[em_CPU]);
                if (tipo_saida_CPU == -1){
                    processos[em_CPU].temp_restante = -1;
                }
                if (tipo_saida_CPU == 0){
                    processos[em_CPU].temp_restante_inicio_io = -1;
                }
                printf("Processo %i assume CPU\n", em_CPU);
            }

        }  

        // Se io não está sendo utilizada
        if (em_io == -1){
            
            // se fila pra io não está vazia
            if (fila_esta_vazia(&fila_io) == 0){   
                em_io = remover(&fila_io);
                printf("Processo %i entrou em I/O. \n", em_io);
                tempo_io = aloca_processo_io(&processos[em_io]);
            }
            
        } 

        else{
            tempo_io--;
            printf("Processo %i tem %is restante de I/O.\n" , em_io, tempo_io);
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
                em_io = -1;
            }
        }       

        instante += 1;
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

   
}