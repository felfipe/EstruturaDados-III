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
#include <limits.h>

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
typedef struct aresta Aresta;
typedef struct vertice Vertice;

struct vertice{
    char cidade[40];
    char estado[3];
    Aresta *aresta;
};
struct aresta{
    Vertice* destino;
    char tempoViagem[10];
    int distancia;
    Aresta* prox;
};
// Estrutura de cabeçalho com 19 bytes
typedef struct{ // Estrutura para manipulação do header
    char status;
    int numero_vertices;
    int numero_arestas;
    char data_ultima_compactacao[11];
} Header;


/******************** FUNÇÕES AUXILIARES *************************/
/*
Insere ordenadamente um elemento do tipo aresta em uma lista
Retorno:
0 - ERRO
1 - OK
*/
int insere_lista_ordenada(Aresta** lista, Aresta elemento){
    if(lista == NULL)
        return 0;
    Aresta *no = (Aresta*) malloc(sizeof(Aresta)); // Aloca o nó
    if(no == NULL)
        return 0;
    no->destino = elemento.destino;
    no->distancia = elemento.distancia;
    strcpy(no->tempoViagem,elemento.tempoViagem);
    if((*lista) == NULL){//lista vazia: insere início
        no->prox = NULL;
        *lista = no;
        return 1;
    }
    else{
        Aresta *ant, *atual = *lista;
        while(atual != NULL && (strcmp(atual->destino->cidade, no->destino->cidade) < 0)){
            ant = atual;
            atual = atual->prox;
        }
        if(atual == *lista){//insere início
            no->prox = (*lista);
            *lista = no;
        }else{
            no->prox = atual;
            ant->prox = no;
        }
        return 1;
    }
}
/*
Libera a lista
*/
void libera_lista(Aresta* li){
        Aresta* no;
        while(li != NULL){
            no = li;
            li = li->prox;
            free(no);
        }
}

/*Função que tenta abrir o arquivo informado. Caso o arquivo não exista ou seu status seja '0',
printa a mensagem de erro e finaliza o programa
*/
FILE *open_file_bin(char* file_name, char* mode){ 
    FILE *file;
    
    file = fopen(file_name,mode);
    if(file == NULL || getc(file) == '0'){
        printf("Falha no carregamento do arquivo.");
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

int read_bin_rnn(FILE* file, int rnn, Route *route){
    clear_route(route); // Limpa o struct
    fseek(file,19,SEEK_SET); // Pula o cabeçalho
    fseek(file,rnn*TAM_REGISTRO,SEEK_CUR); // Vai para o byte offset do registro
    if(fgetc(file) == EOF){ // Se passou do final retorna -1
        return -1;
    }
    
    fseek(file,-1,SEEK_CUR); // volta um byte por causa do fgetc

    // Le os campos do registro    
    fread(route->estadoOrigem,2*sizeof(char),1,file);
    fread(route->estadoDestino,2*sizeof(char),1,file);
    fread(&(route->distancia),sizeof(int),1,file);
    read_variable_string(file,route->cidadeOrigem);
    read_variable_string(file,route->cidadeDestino);
    read_variable_string(file,route->tempoViagem);
    if(route->estadoOrigem[0] == '*') // Se o arquivo está deletado, retorna 2
        return 2;
    return 1;
}

// Função que lê o header do arquivo
void read_header(FILE *file, Header *head){
    fseek(file,0,SEEK_SET);
    fread(&(head->status),sizeof(char),1,file);
    fread(&(head->numero_vertices),sizeof(int),1,file);
    fread(&(head->numero_arestas),sizeof(int),1,file);
    fread(head->data_ultima_compactacao,10*sizeof(char),1,file);
}
void limpa_aresta(Aresta *aresta){
    aresta->destino = NULL;
    aresta->distancia = 0;
    aresta->prox = NULL;
    strcpy(aresta->tempoViagem,"");
}

void printa_grafo(Vertice *cidade, int tam){
    for(int i = 0; i < tam; i++){ // percorre todo vetor de vértices
        printf("%s %s ",cidade[i].cidade, cidade[i].estado);
        for(Aresta *no = cidade[i].aresta; no != NULL; no = no->prox){ // percorre toda lista de cada vértice
            if(strcmp("",no->tempoViagem) == 0)
                printf("%s %s %d ", no->destino->cidade,no->destino->estado,no->distancia);
            else
                printf("%s %s %d %s ", no->destino->cidade,no->destino->estado,no->distancia,no->tempoViagem);
        }
        printf("\n");
    }
}

/*
Insere ordenadamente no vetor de Vértices
*/
int insere_vetor_ordenado(Vertice* cidades, int tam, char* cidade, char* estado){
    int j = 0;
    while(j < tam){
        if(strcmp(cidade,cidades[j].cidade) == 0)
            return 0;
        if(strcmp(cidade,cidades[j].cidade) < 0)
            break;
        j++;   
    }
    for(int i = tam; i > j; i--){
        strcpy(cidades[i].cidade,cidades[i-1].cidade);
        strcpy(cidades[i].estado,cidades[i-1].estado);
    }
    strcpy(cidades[j].cidade,cidade);
    strcpy(cidades[j].estado,estado);
    return 1;
}

void quickSort(Vertice *aux, int inicio, int fim){
    
    int i, j;
    Vertice y;

    i = inicio;
    j = fim;

    int tam = strlen(aux[(inicio + fim) / 2].cidade);

    char x[tam+1];
    strcpy(x, aux[(inicio + fim) / 2].cidade);
     
    while(i <= j) {
        while(strcmp(aux[i].cidade,  x) < 0 && i < fim) {
            i++;
        }
        while(strcmp(aux[j].cidade, x) > 0 && j > inicio) {
            j--;
        }
        if(i <= j) {
            y = aux[i];
            aux[i] = aux[j];
            aux[j] = y;
            i++;
            j--;
        }
    }
     
    if(j > inicio) {
        quickSort(aux, inicio, j);
    }
    if(i < fim) {
        quickSort(aux, i, fim);
    }
}

/**************** FUNÇÃO 9 ********************/
/*
GERA UMA LISTA DE ADJACÊNCIA BIDIRECIONAL
TEM COMO RETORNO O TAMANHO DO VETOR DE VÉRTICES
*/
int gera_lista(FILE* file, Vertice **cidade){
    int i = 0;
    int cont = 0;
    Route route;
    Header header;
    clear_route(&route);
    read_header(file, &header);
    *cidade = (Vertice*)calloc(header.numero_vertices,sizeof(Vertice)); // ALOCA O VETOR DE VÉRTICES
    Vertice* vetor = *cidade;
    while(read_bin_rnn(file,i,&route) != -1){ // Lê todos os registros para a inserção dos vértices ordenadamente
        if(route.estadoOrigem[0] != '*'){
            if(insere_vetor_ordenado(*cidade,cont,route.cidadeOrigem,route.estadoOrigem) == 1)
                cont++;
            if(insere_vetor_ordenado(*cidade,cont,route.cidadeDestino,route.estadoDestino) == 1)
                cont++;
        }
        i++;
    }
    i = 0;
    while(read_bin_rnn(file,i,&route) != -1){ // Lê todos os registros para a inserção das arestas do grafo na lista de adjacência
        if(route.estadoOrigem[0] != '*'){
            for(int j = 0; j < header.numero_vertices; j++){ // Encontra no vetor de vértices a cidade de origem do registro
                if(strcmp(vetor[j].cidade, route.cidadeOrigem) == 0){
                    Aresta aresta;
                    limpa_aresta(&aresta);
                    strcpy(aresta.tempoViagem,route.tempoViagem);
                    aresta.distancia = route.distancia;
                    aresta.prox = NULL;
                    for(int k = 0; k < header.numero_vertices; k++){    // encontra a cidade de destino no vetor de vértices
                        if(strcmp(route.cidadeDestino, vetor[k].cidade) == 0){
                            aresta.destino = &(vetor[j]);
                            insere_lista_ordenada(&(vetor[k].aresta),aresta); // insere o link do destino pra origem
                            aresta.destino = &(vetor[k]);
                            break;
                        }
                    }
                    insere_lista_ordenada(&(vetor[j].aresta),aresta); // insere o link da origem pro destino
                    
                }
            }
        }
        i++;
    }
    if(cont == 0) // Se o arquivo estiver vazio
        printf("Registro inexistente.");
    fclose(file);
    return cont;
}

/**************** FUNÇÃO 10 *******************
void dijkstra(FILE* file){
    Vertice* cidadeRaiz, cidadeAux;
    char tipoCampo[15], valorCampo[40];
    int nVertices; //numero de vertices
    nVertices = gera_lista(file, &cidadeRaiz);
    int D[nVertices]; 
    int ANT[nVertices];

    
    
    
}*/



/**************** FUNÇÃO 11 ********************/
void prim(FILE *file){
    Vertice* lista;
    int num_vertices = gera_lista(file,&lista); // Gera o grafo a partir do arquivo
    Vertice conjunto_b[num_vertices]; // Conjunto de vértices analizados
    Vertice aux; //
    Aresta aresta;
    Vertice vertice_add;
    int tam_conjunto_b=0;
    int min = INT_MAX;
    int indice_vertice;
    char campo[40];
    char vertice_inicial[40];

    scanf("%s", campo);
    scan_quote_string(vertice_inicial);

    int i = 0;
    for(i=0; i<num_vertices; i++){ // Procura o vértice inical e o adiciona no conjunto B
        if(strcasecmp(lista[i].cidade,vertice_inicial) == 0){
            strcpy(conjunto_b[0].cidade,lista[i].cidade);
            strcpy(conjunto_b[0].estado,lista[i].estado);
            conjunto_b->aresta = NULL;
            break;
        }
    }
    if(i == num_vertices){ // Caso a cidade não esteja no gráfico
        printf("Cidade inexistente.");
        exit(0);
    }
    tam_conjunto_b++;

    while(tam_conjunto_b != num_vertices){ // Enquanto B != N
        min = INT_MAX; // min = "infinito"
        for(int cont_vetices = 0; cont_vetices < num_vertices; cont_vetices++){ // Para todos os vértices do grafo
            for(int cont_conj_b = 0; cont_conj_b < tam_conjunto_b; cont_conj_b++){
                if(strcasecmp(lista[cont_vetices].cidade,conjunto_b[cont_conj_b].cidade) == 0){ // Se o vértice estiver no conjunto B
                    aux = lista[cont_vetices];

                    while (aux.aresta != NULL){
                        int esta_conj_N_B = 1;
                        for(int j=0; j<tam_conjunto_b;j++){ // Verifica se está no conjunto N - B
                            if(strcasecmp(aux.aresta->destino->cidade,conjunto_b[j].cidade) == 0) {
                                esta_conj_N_B = 0;
                                break;
                            }
                        }
                        if(esta_conj_N_B){ // Está no conjunto N - B
                            if(min > aux.aresta->distancia){ // Se min > custo da aresta
                                min = aux.aresta->distancia;
                                vertice_add = *aux.aresta->destino;
                                vertice_add.aresta = NULL;
                                indice_vertice = cont_conj_b;
                                limpa_aresta(&aresta);
                                aresta = *aux.aresta;
                                aresta.prox = NULL;
                            }
                        }
                        aux.aresta = aux.aresta->prox;
                    }
                }
            }
        }
        insere_lista_ordenada(&(conjunto_b[indice_vertice].aresta),aresta);
        conjunto_b[tam_conjunto_b] = vertice_add;
        tam_conjunto_b++;
    }
    quickSort(conjunto_b,0,tam_conjunto_b-1);
    printa_grafo(conjunto_b,tam_conjunto_b);
}



int main(){
    int funcao;
    char fileNameBin[20]; //string para leitura do nome do arquivo binario a ser aberto
    FILE *file;

    scanf("%d", &funcao);
    //fflush(stdin);
    scanf("%s", fileNameBin);
    file = open_file_bin(fileNameBin,"rb");
    Vertice *cidade;
    int tam;
    switch(funcao){
        case 9:
            tam = gera_lista(file,&cidade); // gera a lista
            printa_grafo(cidade,tam);
            for(int i = 0; i < tam; i++)    // libera as listas de arestas
                libera_lista(cidade[i].aresta);
            free(cidade); // libera o vetor de vértices
            break;

        case 10:
            break;

        case 11:
            prim(file);
            break;
    }
    return 0;
}