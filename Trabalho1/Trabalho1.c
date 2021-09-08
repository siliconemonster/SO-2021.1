/*
Trabalho 1
Integrantes:
Aline Freire de Rezende - 116110571;
Gilberto Lopes Inácio Filho - ;
Letícia Tavares da Silva - 117210390; */


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#define limite_processos 3
#define FATIA 5


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
    int inicio_io; // tempo apos o inicio do servico em que o processo saira para i/o
    int temp_restante_inicio_io;
	int tempo_servico_io; // tempo de duracao da i/o do processo
    
};

// Funcoes para os processos
void NovoProcesso( struct Processo *p, int temp_entrada, int id, int temp_chegada) { 
    
    p->pid = id;
    p->temp_chegada = temp_chegada;
    p->temp_servico = 5;
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

int* aloca_processo_CPU(struct Processo *p){
    int tempo_processo_CPU;
    int tipo_saida;
    static int retorno[2];

    // i/o
    if (p->temp_restante_inicio_io < FATIA && p->temp_restante_inicio_io >= 0){
        tempo_processo_CPU = p->temp_restante_inicio_io;
        p->temp_restante = p->temp_restante - tempo_processo_CPU;
        p->temp_restante_inicio_io = -1;
        tipo_saida = 0; // é i/o
    }
    else{
        // acaba
        if (p->temp_restante <= FATIA){
            tempo_processo_CPU = p->temp_restante;
            p->temp_restante = -1;
            tipo_saida = -1; // acaba
        }
        // preempcao
        else{
            tempo_processo_CPU = FATIA;
            p->temp_restante = p->temp_restante - tempo_processo_CPU;
            p->temp_restante_inicio_io = p->temp_restante_inicio_io - tempo_processo_CPU;
            tipo_saida = 1; // é preempcao
        }
    }

    retorno[0] = tempo_processo_CPU;
    retorno[1] = tipo_saida;

    return retorno;
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
    int* tempo_e_tipo;
    int tempo_io;


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
        
        // criação do processo
        if (instante == prox_chegada && id_processo < limite_processos) {

            printf("Id %i \n", id_processo); 

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
            

            id_processo += 1;

        }

        // Se a CPU não está vazia
        if (em_CPU != -1){
            
            tempo_e_tipo[0]--;
            printf("Tempo restante de CPU: %i \n" ,tempo_e_tipo[0]);
            // se o processo perde a CPU, insere na fila de baixa prioridade ou de io ou termina
            if(tempo_e_tipo[0] == 0){ // tempo restante em CPU = 0
                
                
                if (tempo_e_tipo[1] == 1){ // preempção
                    inserir(&fila_baixa_prio, em_CPU);

                }
                else if (tempo_e_tipo[1] == 0){ // i/o
                    inserir(&fila_io, em_CPU);

                }
                else{
                    processos_finalizados += 1;
                }
                em_CPU = -1; // CPU fica vazia
            }
            
        }    

        // se CPU está vazia, verifica se tem processos em outras filas para pegar
        if (em_CPU == -1) {
            if (fila_esta_vazia(&fila_alta_prio) == 1){   
                if (fila_esta_vazia(&fila_baixa_prio) == 1){
                    em_CPU = -1;
                }
                else{
                    em_CPU = remover(&fila_baixa_prio);
                    tempo_e_tipo = aloca_processo_CPU(&processos[em_CPU]);
                }
            }
            else{
                em_CPU = remover(&fila_alta_prio);
                tempo_e_tipo = aloca_processo_CPU(&processos[em_CPU]);
            }
        }  

        // Se io não está sendo utilizada
        if (em_io == -1){
            
            // se fila pra io não está vazia
            if (fila_esta_vazia(&fila_io) == 0){   
                em_io = remover(&fila_io);
                tempo_io = aloca_processo_io(&processos[em_io]);
            }
            
        } 
        else{
            tempo_io--;
            if (tempo_io == 0){

                if(processos[em_io].temp_restante != 0){
                    inserir(&fila_baixa_prio, em_io);
                }
                else{
                    processos_finalizados += 1;
                }
                em_io = -1;
            }
        }       

        instante += 1;
         
    }

   
}