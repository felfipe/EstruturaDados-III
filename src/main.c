/*
    TRABALHO DE ESTRUTURA DE DADOS III - PARTE 1

    Grupo: André Baconcelo Prado Furlnaetti - 10748305
           Marcelo Magalhães Coelho         - 10716633
           Victor Felipe do Amaral          - 10696506
*/


#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<time.h>
#include "funcoes_uteis.h"

#define TAM_REGISTRO 85 // Define para o tamanho do registro
#define TAM_ESTADO 2 // Tamanho fixo da string estado

/*********************** STRUCTS *****************************/
typedef struct{ // Estrutura do resgistro para ser manipulado
    char estadoOrigem[3];
    char estadoDestino[3];
    int  distancia;
    char cidadeOrigem[40];
    char cidadeDestino[40];
    char tempoViagem[10];
} Route;


typedef struct{
    char cidade[40];
    char estado[3];
    Aresta *aresta;
}Vertice;
typedef struct{
    Vertice *destino;
    char tempoViagem[10];
    int distancia;
}Aresta;
// Estrutura de cabeçalho com 19 bytes
typedef struct{ // Estrutura para manipulação do header
    char status;
    int numero_vertices;
    int numero_arestas;
    char data_ultima_compactacao[11];
} Header;


/******************** FUNÇÕES AUXILIARES *************************/

//Função que tenta abrir o arquivo informado. Caso o arquivo não exista ou seu status seja '0',
//printa a mensagem de erro e finaliza o programa
FILE *open_file_bin(char* file_name, char* mode){ 
    FILE *file;
    file = fopen(file_name,mode);
    if(file == NULL || getc(file) == '0'){
        printf("Falha na execução da funcionalidade.");
        exit(0);
    }
    fseek(file,0,SEEK_CUR); // Volta no começo do arquivo
    return file;
}

// Função que limpa a string recebida
void limpa_string(char* string, int tam){
    for(int i =0; i< tam;i++)
        string[i] = '\0';
}

// Limpa a struct Route
void clear_route(Route *route){
    limpa_string(route->estadoOrigem,3);
    limpa_string(route->estadoDestino,3);
    limpa_string(route->cidadeOrigem,40);
    limpa_string(route->cidadeDestino,40);
    limpa_string(route->tempoViagem,10);
    route->distancia = 0;
}

// Le uma string de tamanho variável do arquivo
void read_variable_string(FILE *file_bin,char *string_var){
    int i = 0;
    while(1){ // vai lendo até achar |
        string_var[i] = fgetc(file_bin);
        if(string_var[i] == '|')
            break;
        i++;
    }
    string_var[i] = '\0';
}

int dictionary_field(char *field_name){
    char dictionary[6][15] = {"estadoOrigem","estadoDestino","distancia","cidadeOrigem","cidadeDestino","tempoViagem"};
    for(int i = 0; i <6;i++){
        if(!strcmp(field_name,dictionary[i]))
            return i+1;
    }
    return -1;
}

// Função que lê o header do arquivo
void read_header(FILE *file, Header *head){
    fseek(file,0,SEEK_SET);
    fread(&(head->status),sizeof(char),1,file);
    fread(&(head->numero_vertices),sizeof(int),1,file);
    fread(&(head->numero_arestas),sizeof(int),1,file);
    fread(head->data_ultima_compactacao,10*sizeof(char),1,file);
}


/***************** FUNÇÃO 2 ***********************/

// Printa na tela os registro de um arquivo bin
int recover_data(FILE* file){
    int i = 0;
    int cont = 0;
    Route route;
    clear_route(&route);
    while(read_bin_rnn(file,i,&route) != -1){ // Lê todos os registros
        if(route.estadoOrigem[0] != '*'){
            printf("%d %s %s %d %s %s %s \n",i,route.estadoOrigem,route.estadoDestino,route.distancia,route.cidadeOrigem,
                   route.cidadeDestino,route.tempoViagem);
            cont++;
        }
        i++;
    }
    if(cont == 0) // Se o arquivo estiver vazio
        printf("Registro inexistente.");
    fclose(file);
    return 0;
}

/**************** FUNÇÃO 9 ********************/
int gera_lista(FILE* file, Vertice **cidade){
    int i = 0;
    int cont = 0;
    Route route;
    Header header;
    clear_route(&route);
    read_header(file, &header);
    *cidade = (Vertice*)calloc(header.numero_vertices,sizeof(Vertice));
    
    while(read_bin_rnn(file,i,&route) != -1){ // Lê todos os registros
        if(route.estadoOrigem[0] != '*'){
                   
        }
        i++;
    }
    if(cont == 0) // Se o arquivo estiver vazio
        printf("Registro inexistente.");
    fclose(file);
    return header.numero_vertices;
}

/**************** FUNÇÃO 10 ********************/
void dijkstra(FILE* file){
    Vertice* cidadeRaiz, cidadeAux;
    char tipoCampo[15], valorCampo[40];
    int nVertices; //numero de vertices
    nVertices = gera_lista(file, &cidadeRaiz);
    int D[nVertices]; 
    int ANT[nVertices];

    
    
    
}






/**************** FUNÇÃO 11 ********************/
void prim(FILE *file){
    Vertice vertice;
    Vertice* conjunto_b;
    Vertice* conjunto_n;
    
}



int main(){
    char funcao;
    char fileNameBin[20]; //string para leitura do nome do arquivo binario a ser aberto
    FILE *file;

    scanf("%c", &funcao);
    scanf("%s", fileNameBin);
    file = open_file_bin(fileNameBin,"rb");

    switch(funcao){
        case '9':
            
            break;

        case '10':
            
            break;

        case '11':
            break;
    }
    return 0;
}