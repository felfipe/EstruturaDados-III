/*
    TRABALHO DE ESTRUTURA DE DADOS III - PARTE 3

    Grupo: André Baconcelo Prado Furlnaetti - 10748305
           Marcelo Magalhães Coelho         - 10716633
           Victor Felipe do Amaral          - 10696506
    
    Runcodes trabalho para correção: André Baconcelo Prado Furlnaetti - 10748305
*/

#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<time.h>
#include <limits.h>
#include "funcoes_uteis.h"

#define TAM_REGISTRO 85 // Define para o tamanho do registro
#define TAM_ESTADO 2 // Tamanho fixo da string estado

/*********************** STRUCTS *****************************/
typedef struct{ // Estrutura do resgistro para ser manipulado
    char estadoOrigem[3]; // String estado de origem
    char estadoDestino[3]; // String estado de destino
    int  distancia; // distância
    char cidadeOrigem[40]; // String cidade origem
    char cidadeDestino[40]; // String cidade destino
    char tempoViagem[10]; // String tempo de viagem
} Route; // Nome da strucit

typedef struct aresta Aresta; // Typedef da struct da aresta
typedef struct vertice Vertice; // Typedef da struct do vértice

struct vertice{ // Struct que representa os vértices do grafo
    char cidade[40]; // String cidade
    char estado[3]; // String estado
    Aresta *aresta; // Cabeça da lista de arestas pertencentes ao vértice
};
struct aresta{ // Struct que representa as arestas do grafo
    Vertice* destino; // vértice do destino da aresta
    char tempoViagem[10]; // String tempo de viagem
    int distancia; // distância
    Aresta* prox; // Ponteiro q aponta para a próxima arestas
};
// Estrutura de cabeçalho com 19 bytes
typedef struct{ // Estrutura para manipulação do header
    char status; // Status do header
    int numero_vertices; // número de vértices
    int numero_arestas; // número de arestas
    char data_ultima_compactacao[11]; // Data da última compactação
} Header; // Nome da strutura

//estrutura usada no vetor de distancias no Dijkstra
typedef struct{
    int menor_distancia; //menor distancia até o vertice origem
    int status; //indicador se o vertice já foi totalmente analisado
} m_distancias;


/******************** FUNÇÕES AUXILIARES *************************/
/*
Insere ordenadamente um elemento do tipo aresta em uma lista de forma ordenada
Retorno:
0 - ERRO
1 - OK
*/
int insere_lista_ordenada(Aresta** lista, Aresta elemento){ // Recebe a cabeça da lista de arestas e a aresta que será inserida
    if(lista == NULL) // Se a cabeça da lista não foi alocada
        return 0;
    Aresta *no = (Aresta*) malloc(sizeof(Aresta)); // Aloca o nó
    if(no == NULL) // Se der erro
        return 0;
    no->destino = elemento.destino; // coloca os atributos do elemento no nó
    no->distancia = elemento.distancia;
    strcpy(no->tempoViagem,elemento.tempoViagem);
    if((*lista) == NULL){//lista vazia: insere início
        no->prox = NULL;
        *lista = no; // cabeça da lista
        return 1; // OK
    }
    else{ // procura onde deve ser inserido e reorganiza os ponteiros
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
    Aresta* no; // para auxiliar ao percorrer a lista
    while(li != NULL){
        no = li;
        li = li->prox;
        free(no); // libera
    }
}

/*
    Função que tenta abrir o arquivo informado. Caso o arquivo não exista ou seu status seja '0',
    printa a mensagem de erro e finaliza o programa
*/
FILE *open_file_bin(char* file_name, char* mode){  // Recebe o nome do arquivo e o modo que será aberto
    FILE *file; // Ponteiro
    file = fopen(file_name,mode); // abre o arquivo
    if(file == NULL || getc(file) == '0'){ // verifica se realmente abriu e se o status é válido
        printf("Falha no carregamento do arquivo.");
        exit(0); // encerra o programa
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
    while(1){ // vai lendo até achar '|'
        string_var[i] = fgetc(file_bin); // avança um byte
        if(string_var[i] == '|')
            break;
        i++;
    }
    string_var[i] = '\0';
}

// Le um registro pelo respectivo rnn
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

// Limpa a struct aresta
void limpa_aresta(Aresta *aresta){
    aresta->destino = NULL;
    aresta->distancia = 0;
    aresta->prox = NULL;
    strcpy(aresta->tempoViagem,"");
}

// Printa o gráfico gerado por uma lista de adjacencia
void printa_grafo(Vertice *cidade, int tam){ // Recebe o vetor de vértices e o tamanho
    for(int i = 0; i < tam; i++){ // percorre todo vetor de vértices
        printf("%s %s ",cidade[i].cidade, cidade[i].estado);
        for(Aresta *no = cidade[i].aresta; no != NULL; no = no->prox){ // percorre toda lista de cada vértice
            if(strcmp("",no->tempoViagem) == 0) // Se estiver vazio
                printf("%s %s %d ", no->destino->cidade,no->destino->estado,no->distancia);
            else
                printf("%s %s %d %s ", no->destino->cidade,no->destino->estado,no->distancia,no->tempoViagem);
        }
        printf("\n");
    }
}

// Insere ordenadamente no vetor de Vértices
// Recebe o vetor de vértices, o tamanho, a cidade e o estado
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

// Ordena o vetor de vértices pelo método quicksort
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
    Route route; // struct que representa o registro no disco
    Header header; // struct q representa o header salvo no disco
    clear_route(&route); // Limpa o struct
    read_header(file, &header); // le o header do arquivo
    *cidade = (Vertice*)calloc(header.numero_vertices,sizeof(Vertice)); // ALOCA O VETOR DE VÉRTICES
    Vertice* vetor = *cidade;
    while(read_bin_rnn(file,i,&route) != -1){ // Lê todos os registros para a inserção dos vértices ordenadamente
        if(route.estadoOrigem[0] != '*'){ // Se não estiver apagado
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
                    limpa_aresta(&aresta); // Limpa a struct aresta
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

/**************** FUNÇÃO 10 ********************/
void dijkstra(FILE* file){
    Vertice verticeAux; //auxiliar
    Vertice* vetorCidades; //ponteiro que se tornar um vetor de vertices
    Vertice verticeOrigem; //ponteiro que recebera o endereco do vertice origem sugerido pelo usuario
    char tipoCampo[15];
    char valorCampo[40];
    int nVertices;  //numero de vertices
    nVertices = gera_lista(file, &vetorCidades);  
    m_distancias D[nVertices]; //vetor de distâncias
    int ANT[nVertices]; //vetor de antecessores, indicando-os pela sua posição na lista de vértices
    int i, j;
    int nao_achou = 1;
    int indice;
    int indiceProx;
    int distancia_atual=0;
    int distanciaAux;
    int distanciaProx;
    Aresta* arestaAux;
    scanf("%s", tipoCampo);
    if (strcmp(tipoCampo, "cidadeOrigem")){ //confere se o campo lido é o cidadeOrigem
        printf("Falha na execução da funcionalidade.");
        return;
    }
    scan_quote_string(valorCampo); //le o nome da cidade que sera o Vertice Origem
    for(i=0; i<nVertices; i++){
        verticeAux = vetorCidades[i];
        D[i].menor_distancia = -1; //preenche as distancias do vetor o com valor invalido -1
        D[i].status = 0; //preenche os status do vetor com 0 (vertice nao totalmente verificado) 
        ANT[i] = -1; //preenche o vetor de antecessores com -1
        if (nao_achou && !strcmp(verticeAux.cidade, valorCampo))
        {
            nao_achou = 0; //indica que o vertice correspondente à cidadeOrigem lida foi encontrado
            verticeOrigem = verticeAux;
            D[i].menor_distancia = 0;
            D[i].status = 1;
            indice=i;
        }
    }
    if(nao_achou){
        printf("Cidade inexistente.");
        return;
    }
    arestaAux=verticeOrigem.aresta;
    j=0;
    while(j<(nVertices-1)){
        if(arestaAux!=NULL){
            distanciaProx=-1;
            distanciaAux=arestaAux->distancia+distancia_atual;
            for (i = 0; i < nVertices; i++){
                if (!D[i].status){
                    if (!strcmp(vetorCidades[i].cidade, arestaAux->destino->cidade)){
                        if(D[i].menor_distancia == -1 || distanciaAux<D[i].menor_distancia){
                            ANT[i]=indice;
                            D[i].menor_distancia=distanciaAux;
                        }
                    }
                    if (D[i].menor_distancia!= -1){
                        if (distanciaProx== -1 || D[i].menor_distancia < distanciaProx){
                            distanciaProx=D[i].menor_distancia;
                            indiceProx=i;
                            }
                    }
                }
            }
            arestaAux=arestaAux->prox;
        }else{
            indice=indiceProx;
            distancia_atual=distanciaProx;
            arestaAux=vetorCidades[indice].aresta;
            D[indice].status=1;
            j++;
        }
    }
    for(i=0; i<nVertices; i++){
        if (ANT[i]!=-1){ //verifica se nao é o vertice origem
            printf("%s %s %s %s %d %s %s\n", verticeOrigem.cidade, verticeOrigem.estado, vetorCidades[i].cidade, 
            vetorCidades[i].estado, D[i].menor_distancia, vetorCidades[(ANT[i])].cidade, vetorCidades[(ANT[i])].estado);
        }
    }
}



/**************** FUNÇÃO 11 ********************/
// Função que recebe um ponteiro de arquivo, utiliza a função descrita no item 9 para gerar o gráfo,
// a partir deste, encontra a árvore geradora mínima conforme o algorítimo de Prim

void prim(FILE *file){
    Vertice* vet_vertices; // vetor de vértices, representa o conjunto de todos os vértices, chamado de N
    int num_vertices = gera_lista(file,&vet_vertices); // Gera o grafo a partir do arquivo
    Vertice conjunto_b[num_vertices]; // Conjunto de vértices analizados, chamado de B
    Vertice aux; // auxilia pra percorrer as listas
    Aresta aresta; // aresta que será incerida no vértice correspondente do conjunto B
    Vertice vertice_add; // Vértice que possui o meno peso, será inserido no conjunto B
    int tam_conjunto_b=0; // tamanho do conjunto B
    int min = INT_MAX; // Peso das arestas (distâncias), INT_MAX esta representando o "infinito"
    int indice_vertice; // Índice do vértice em q a aresta será inserida
    char campo[40]; // Campo de entrada
    char vertice_inicial[40]; // Nome da cidade do vértice inicial

    scanf("%s", campo); // Recebe o campo
    if(strcmp(campo,"cidadeOrigem") != 0){
        printf("Falha na execução da funcionalidade.");
        exit(0); // encerra o programa
    }

    scan_quote_string(vertice_inicial); // Recebe o nome da cidade do vértice inicial

    int i = 0;
    for(i=0; i<num_vertices; i++){ // Procura o vértice inical e o adiciona no conjunto B
        if(strcasecmp(vet_vertices[i].cidade,vertice_inicial) == 0){
            strcpy(conjunto_b[0].cidade,vet_vertices[i].cidade);
            strcpy(conjunto_b[0].estado,vet_vertices[i].estado);
            conjunto_b->aresta = NULL;
            break;
        }
    }
    if(i == num_vertices){ // Caso a cidade não esteja no gráfico
        printf("Cidade inexistente.");
        exit(0);
    }
    tam_conjunto_b++;

    while(tam_conjunto_b != num_vertices){ // Enquanto B != N, condição de parada
        min = INT_MAX; // min = "infinito"
        for(int cont_vetices = 0; cont_vetices < num_vertices; cont_vetices++){ // Para percorrer todos os vértices do grafo
            for(int cont_conj_b = 0; cont_conj_b < tam_conjunto_b; cont_conj_b++){ // Percorre o vetor do conjunto B
                if(strcasecmp(vet_vertices[cont_vetices].cidade,conjunto_b[cont_conj_b].cidade) == 0){ // Se o vértice estiver no conjunto B
                    aux = vet_vertices[cont_vetices]; // Recebe o vértice encontrado
                    while (aux.aresta != NULL){ // Se possuir arestas
                        int esta_conj_N_B = 1; // Flag para verificar se o vértice está no conjunto N - B
                        for(int j=0; j<tam_conjunto_b;j++){ // Verifica se está no conjunto N - B
                            if(strcasecmp(aux.aresta->destino->cidade,conjunto_b[j].cidade) == 0) {
                                esta_conj_N_B = 0; // Se não estiver
                                break;
                            }
                        }
                        if(esta_conj_N_B){ // Está no conjunto N - B
                            if(min > aux.aresta->distancia){ // Se min > peso da aresta
                                min = aux.aresta->distancia; // Recebe o novo mínimo
                                vertice_add = *aux.aresta->destino; // recebe o vértice
                                vertice_add.aresta = NULL;
                                indice_vertice = cont_conj_b; // o indíce no conjunto_b
                                limpa_aresta(&aresta); // Limpa a aresta
                                aresta = *aux.aresta; // recebe a resta
                                aresta.prox = NULL;
                            }
                        }
                        aux.aresta = aux.aresta->prox; // vai para a próxima aresta
                    }
                }
            }
        }
        // Após percorrer todos os vértices
        insere_lista_ordenada(&(conjunto_b[indice_vertice].aresta),aresta); // insere a aresta no vértice do conjunto B
        conjunto_b[tam_conjunto_b] = vertice_add; // Adiciona o novo vértice
        tam_conjunto_b++; // mais um no tamanho do conjunto B
    }
    quickSort(conjunto_b,0,tam_conjunto_b-1); // Ordena o conjunto (vetor) B
    printa_grafo(conjunto_b,tam_conjunto_b); // Printa o gráfico (árvore geradora mínima) encontrado
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
            dijkstra(file);
            break;

        case 11:
            prim(file);
            break;
    }
    return 0;
}