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
#define limite_processos 100


// Estrutura de uma fila
struct Fila {

	int elementos[limite_processos]; // Elementos serao ids dos procesoss (?)
	int primeiro; // primeiro elemento da fila
	int ultimo; // ultimo elemento da fila
};

// Funcoes para as filas

// Cria uma fila
void criarFila( struct Fila *f) { 

	f->primeiro = 0;
	f->ultimo = -1;

};

// Insere elemento em uma fila
void inserir(struct Fila *f, int v) {

	if(f->ultimo == limite_processos - 1)
		f->ultimo = -1;

	f->ultimo++;
	f->elementos[f->ultimo] = v;

};

// Remove elemento em uma fila e o retorna
int remover( struct Fila *f ) { 

	int temp = f->elementos[f->primeiro++];

	if(f->primeiro == limite_processos)
		f->primeiro = 0;
   
	return temp; // retorna o elemento retirado

};

// Remove elemento em uma fila e o retorna
int fila_esta_vazia( struct Fila *f ) { 

	if(f->primeiro == 0){
        return true;
    }
		
	return false;

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
    p ->temp_chegada = temp_chegada;
    p ->temp_restante = temp_chegada;
	p->temp_servico = rand() %150;
	p->inicio_io = rand() % p->temp_servico;
    p-> temp_restante_inicio_io = inicio_io;
    p->tempo_servico_io = temp_entrada;


};


int main() {


    srand(time(NULL)); // para geracao de numero pseudo aleatorio
    
    // Lista com a duracao de cada tipo de I/O
    // 0 -> Disco 1-> Fita Magnetica 2-> Impressora
    int duracao_ios[] = {5,7,12};
    int tipo_io;
    int fatia_tempo = 5; 
    int processos_finalizados = -1; //contador de processos finalizados
    int instante = 0; 
    int id_processo = 0;
    int prox_chegada = 0;
    int tem_io;
    int em_CPU;


    // Processos
    struct Processo processos[limite_processos];  

    // Filas
    struct Fila fila_baixa_prio; 
    struct Fila fila_alta_prio; 
    struct Fila fila_io; 

    criarFila(&fila_alta_prio);
    criarFila(&fila_baixa_prio);
    criarFila(&fila_io);
 
    while(processos_finalizados < 9){
        
        if (instante == prox_chegada && id_processo < 10) {

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
            prox_chegada = instante + rand() %20;

            id_processo += 1;
            processos_finalizados += 1;

        }

        if (fila_esta_vazia(&fila_alta_prio) == true){            
            
            if (fila_esta_vazia(&fila_baixa_prio) == true){
                em_CPU = -1;
            }
            else{
                em_CPU = remover(&fila_baixa_prio);
            }
            
        }
        else{
            em_CPU = remover(&fila_alta_prio);
        }
        
        

        instante += 1;
         
    }

   
}