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

// Estrutura de cabeçalho com 19 bytes
typedef struct{ // Estrutura para manipulação do header
    char status;
    int numero_vertices;
    int numero_arestas;
    char data_ultima_compactacao[11];
} Header;

//Função que tenta abrir o arquivo informado. Caso o arquivo não exista ou seu status seja '0',
//printa a mensagem de erro e finaliza o programa
FILE *open_file_bin(char* file_name, char* mode, int message){ 
    FILE *file;
    file = fopen(file_name,mode);
    if(file == NULL || getc(file) == '0'){
        if(message == 1) { // Qual mensagem deve ser printada
            printf("Falha no processamento do arquivo.");
        }
        if(message == 2){
            printf("Falha no carregamento do arquivo.");
        }
        exit(0);
    }
    fseek(file,0,SEEK_CUR); // Volta no começo do arquivo
    return file;
}

/******************** FUNÇÕES DE LIMPEZA ********************************************/

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

// Limpa a struct Header
void limpa_header(Header *head){
    limpa_string(head->data_ultima_compactacao,11);
    head->numero_arestas = 0;
    head->numero_vertices = 0;
    head->status = '0';
}

/***************** FUNÇÕES DE ESCRITA **********************************/

// Escreve o Header no arquivo bin
int write_header(Header head, FILE *file){
    head.status = '1'; // Coloca o status 1
    fseek(file,0,SEEK_SET);
    fwrite(&(head.status),sizeof(char),1,file);
    fwrite(&(head.numero_vertices),sizeof(int),1,file);
    fwrite(&(head.numero_arestas),sizeof(int),1,file);
    fwrite(head.data_ultima_compactacao,10*sizeof(char),1,file);
    return 0;
}

// Escreve o registro no arquivo
int write_register(FILE* file, Route reg, int j){
    fwrite(reg.estadoOrigem,TAM_ESTADO*sizeof(char),1,file);
    fwrite(reg.estadoDestino,TAM_ESTADO*sizeof(char),1,file);
    fwrite(&(reg.distancia),sizeof(int),1,file);
    fwrite(reg.cidadeOrigem,strlen(reg.cidadeOrigem)*sizeof(char),1,file);
    fwrite("|",sizeof(char),1,file);
    fwrite(reg.cidadeDestino,strlen(reg.cidadeDestino)*sizeof(char),1,file);
    fwrite("|",sizeof(char),1,file);
    fwrite(reg.tempoViagem,strlen(reg.tempoViagem)*sizeof(char),1,file);
    fwrite("|",sizeof(char),1,file);
    // Espaço que sobrou no registro
    int size = TAM_REGISTRO - 2*TAM_ESTADO - sizeof(int) - strlen(reg.cidadeOrigem) - strlen(reg.cidadeDestino) - strlen(reg.tempoViagem) - 3;
    if (j){ // Caso seja a função 7, não pode ser preenchido com #
        for(int i = 0; i < size; i++)
            fwrite("#",sizeof(char),1,file);
    }
    return 1;
}

/**************** FUNÇÕES DE LEITURA ********************************/

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

// Le um registro do arquivo pelo seu rnn
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

/******************** FUNÇÕES AUXILIARES *************************/

// Arquivo auxiliar para contar o número de vértices
int query_city_file(FILE* cities, char *city){
    char query[40];
    fseek(cities,0,SEEK_SET);
    while(fgetc(cities) != EOF){ // Enquanto não terminar o arquivo
        fseek(cities,-1,SEEK_CUR);
        fread(query,40*sizeof(char),1,cities);
        if(!strcmp(city,query)) // Vê se a cidade recebida já existe no arquivo
            return 0;
    }
    fwrite(city,40*sizeof(char),1,cities); // Se não, escreve ela
    return 1;
}

// Gera o cabeçalho do arquivo
int make_header(Header *head, FILE *file){
    Route route;
    int j = 0;
    strcpy(head->data_ultima_compactacao, "##########");
    head->status = '1';
    FILE *cities = fopen("temp","wb+");
    if(cities == NULL)
        return -1;
    while(read_bin_rnn(file,j,&route) != -1){ // Enquanto não terminar o arquivo
        if(route.estadoOrigem[0] != '*'){ // Se o registro não estiver deletado
            head->numero_arestas++;  // Conta o número de arestas
            if(query_city_file(cities,route.cidadeOrigem))
                head->numero_vertices++; // Conta o número dos vértices
            if(query_city_file(cities,route.cidadeDestino))
                head->numero_vertices++; // Conta o número dos vértices
        }
        j++;
    }
    return 1;
}

// Pega a data atual
void get_current_date(char *date){
    time_t now;
    time(&now);
    struct tm *local = localtime(&now);
    char dia[3];
    char mes[3];
    if(local->tm_mday < 10)
        sprintf(dia,"0%d",local->tm_mday);
    else
        sprintf(dia,"%d",local->tm_mday);
    if(local->tm_mon+1 < 10)
        sprintf(mes,"0%d",local->tm_mon+1);
    else
        sprintf(mes,"%d",local->tm_mon+1);

    sprintf(date,"%s/%s/%4d",dia,mes,local->tm_year+1900);
}

// Retorna um inteiro correspondente com campo recebido
int dictionary_field(char *field_name){
    char dictionary[6][15] = {"estadoOrigem","estadoDestino","distancia","cidadeOrigem","cidadeDestino","tempoViagem"};
    for(int i = 0; i <6;i++){
        if(!strcmp(field_name,dictionary[i]))
            return i+1;
    }
    return -1;
}

// Exclui um registro baseado no rnn recebido
int remove_rrn(FILE *file, int rrn){
    fseek(file,19,SEEK_SET);
    fseek(file,rrn*TAM_REGISTRO,SEEK_CUR);
    if(fgetc(file) == EOF) // Se o arquivo acabou
        return -1;
    fseek(file,-1,SEEK_CUR);
    fwrite("*",sizeof(char),1,file); // coloca * no primeiro byte do registro
    return 0;
}

/************************** FUNÇÃO 1 ****************************/

// Função que lê um arquivo CSV e copia os arquivos para um binário, conforme as especificações
int read_csv(char* csv_name, char* bin_name){
    FILE *csv_file = fopen(csv_name,"rb+");
    FILE *bin_file = fopen(bin_name,"wb+");
    FILE *head_file = fopen("head.bin","wb+"); // Arquivo auxiliar para o header
    if(csv_file == NULL || bin_file == NULL || head_file == NULL){ // Caso algum dos arquivos não abra
        printf("Falha no carregamento do arquivo.");
        return -1;
    }
    Route route;
    Header head;
    limpa_header(&head);
    strcpy(head.data_ultima_compactacao,"##########");
    fwrite("#",sizeof(char),19,bin_file); // RESERVA 19 BYTES PARA CABEÇALHO
    char a = '\0';
    while(a != '\n'){ // Pula a primeira linha do CSV
        a = fgetc(csv_file);
    }
    while(fgetc(csv_file) != EOF){
        char dist[10];

        fseek(csv_file,-1,SEEK_CUR); // Volta um byte por causa do fgetc
        
        clear_route(&route);
        limpa_string(dist,10);
        
        // Lê os campos de temanho fixo
        fread(route.estadoOrigem,2*sizeof(char),1,csv_file);
        fgetc(csv_file);
        fread(route.estadoDestino,2*sizeof(char),1,csv_file);

        //Lê os campos de tamanho variável
        fgetc(csv_file);
        for(int i = 0; a!= ',';i++){
            dist[i]= a;
            a = fgetc(csv_file);
        }
        route.distancia = atoi(dist);
        a = fgetc(csv_file);
        for(int i = 0;a != ',';i++){
            route.cidadeOrigem[i] = a;
            a = fgetc(csv_file);
        }
        a = fgetc(csv_file);
        for(int i = 0;a != ',';i++){
            route.cidadeDestino[i] = a;
            a = fgetc(csv_file);
        }

        a = fgetc(csv_file);
        for(int i = 0; a != '\n';i++){
            route.tempoViagem[i] = a;
            a = fgetc(csv_file);
        }

        write_register(bin_file,route,1); // Escreve no arquivo bin
    }
    
    // Cria o Header
    make_header(&head,bin_file);
    write_header(head,bin_file);

    // Fecha os arquivos
    fclose(bin_file);
    fclose(csv_file);
    fclose(head_file);
    return 1;
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

/************************* FUNÇÃO 3 ******************************/

// Busca registro que possuem o mesmo conteudo de um campo informado
void recover_search(FILE* file){
    char nome_campo[15]; // nome do tipo do campo do que se busca
    char wntd_data[40]; // conteudo a se buscar
    int tipo_campo; // indica o tipo de campo lido em nome_campo
    Route route;
    clear_route(&route);
    int i=0;
    int flag=0;
    int aux;

    scanf("%s", nome_campo);
    scan_quote_string(wntd_data);
    tipo_campo = dictionary_field(nome_campo); // pega o indece do campo correspondente
    
    while (read_bin_rnn(file,i,&route) != -1)
    {
        switch (tipo_campo) // Printa todos os registros que possuem o campo informado
        {
            case  1 : // Estado de Origem
                if(!strcmp(wntd_data,route.estadoOrigem)){
                    printf("%d %s %s %d %s %s %s \n",i,route.estadoOrigem,route.estadoDestino,route.distancia,route.cidadeOrigem,
                           route.cidadeDestino,route.tempoViagem);
                    flag++;
                }
                break;

            case 2 : // Estado destino
                if (!strcmp(wntd_data,route.estadoDestino)){
                    printf("%d %s %s %d %s %s %s \n",i,route.estadoOrigem,route.estadoDestino,route.distancia,route.cidadeOrigem,
                           route.cidadeDestino,route.tempoViagem);
                    flag++;
                }
                break;

            case 3 : // Distância
                aux=atoi(wntd_data);
                if(aux==route.distancia){
                    printf("%d %s %s %d %s %s %s \n",i,route.estadoOrigem,route.estadoDestino,route.distancia,route.cidadeOrigem,
                           route.cidadeDestino,route.tempoViagem);
                    flag++;
                }
                break;

            case 4 : // Cidade Origem
                if(!strcmp(wntd_data,route.cidadeOrigem)){
                    printf("%d %s %s %d %s %s %s \n",i,route.estadoOrigem,route.estadoDestino,route.distancia,route.cidadeOrigem,
                           route.cidadeDestino,route.tempoViagem);
                    flag++;
                }
                break;

            case 5 : // Cidade Destino
                if(!strcmp(wntd_data,route.cidadeDestino)){
                    printf("%d %s %s %d %s %s %s \n",i,route.estadoOrigem,route.estadoDestino,route.distancia,route.cidadeOrigem,
                           route.cidadeDestino,route.tempoViagem);
                    flag++;
                }
                break;

            case 6 : // Tempo de viagem
                if(!strcmp(wntd_data,route.tempoViagem)){
                    printf("%d %s %s %d %s %s %s \n",i,route.estadoOrigem,route.estadoDestino,route.distancia,route.cidadeOrigem,
                           route.cidadeDestino,route.tempoViagem);
                    flag++;
                }
                break;

            default: // Se o campo não for correspondente com os dos registros
                printf("Erro na sintaxe do tipo de campo!");
                break;
        }
        i++;
    }
    if(!flag) // Se não achar nenhum registro que possui a característica informada
        printf("Registro inexistente.");
    fclose(file);
}

/******************************* FUNÇÃO 4 *******************************/

// Busca registro pelo rnn
void recover_rrn(FILE* file){
    Route route;
    clear_route(&route);
    int rrn;
    scanf("%d", &rrn);
    if(read_bin_rnn(file,rrn,&route)!=-1){
        printf("%d %s %s %d %s %s %s \n",rrn,route.estadoOrigem,route.estadoDestino,route.distancia,route.cidadeOrigem,
               route.cidadeDestino,route.tempoViagem);
    }else{ // Se não achou
        printf("Registro inexistente.");
    }
    fclose(file);
}

/********************** FUNÇÃO 5 ************************/

// Remove todos os registros que possuem o conteudo de um campo igual ao informado
int remove_record(char *file_name){
    FILE* file = open_file_bin(file_name, "rb+",1);
    int num_reg;
    int distance;
    scanf("%d",&num_reg);
    for(int i =0; i< num_reg; i++) { // Número de registros digitados pelo usuário
        fflush(stdin);
        int j = 0;
        char field_name[15];
        char field_value[40];
        limpa_string(field_name,15);
        limpa_string(field_value,40);
        Route route;
        clear_route(&route);
        scanf("%s", field_name);
        scan_quote_string(field_value);
        if (!strcmp(field_value, "NULO")) // Se for nulo o conteudo a ser buscado for vazio
            strcpy(field_value, "");
        int field_type = dictionary_field(field_name); // pega o indece correspondente ao campo
        if (field_type == 3) { // Se for a distância
            if (!strcmp(field_value, ""))
                distance = 0;
            else
                distance = atoi(field_value);
        }
        while (read_bin_rnn(file, j, &route) != -1) {
            if (route.estadoOrigem[0] != '*') { // Se o registro está removido
                switch (field_type) {
                    case 1: // Estado Origem
                        if (!strcmp(field_value, route.estadoOrigem))
                            remove_rrn(file, j);
                        break;
                    case 2: // Estado Destino
                        if (!strcmp(field_value, route.estadoDestino))
                            remove_rrn(file, j);
                        break;
                    case 3: // Distância
                        if (distance == route.distancia)
                            remove_rrn(file, j);
                        break;
                    case 4: // Cidade origem
                        if (!strcmp(field_value, route.cidadeOrigem))
                            remove_rrn(file, j);
                        break;
                    case 5: // Cidade destino
                        if (!strcmp(field_value, route.cidadeDestino))
                            remove_rrn(file, j);
                        break;
                    case 6: // Tempo de viagem
                        if (!strcmp(field_value, route.tempoViagem))
                            remove_rrn(file, j);
                        break;
                }

            }
            j++;
        }
    }
    Header head;
    limpa_header(&head);
    make_header(&head, file); // Refaz o cabeçalho
    write_header(head, file);
    fclose(file);
    return 0;
}

/************************* FUNÇÃO 6 ****************************/

//insere novos registros ao final do arquivo binário
void insert_regs(char* fileName, int n){
    int i=0;
    char aux[40]; //string para a leitura do conteudo dos campos do novo registro
    Route route; //auxiliar com estrutura de registro
    FILE* file = open_file_bin(fileName, "rb+",1);

    fseek(file,19,SEEK_SET); //pula o cabeçalho

    while(getc(file) != EOF){ //pula para o fim do arquivo
        fseek(file,TAM_REGISTRO-1,SEEK_CUR);
        i++;
    }

    for(i=0; i<n; i++){ //garante que serao lidos os n registros a serem adicionados
        clear_route(&route); //limpa o auxiliar

        //inicia-se a leitura de cada campo do novo registro da seguinte forma:
        scan_quote_string(aux); //lê-se o conteúdo cada cada campo do novo registro
        if(strcmp(aux, "NULO")){ //confere se o campo será nulo
            strcpy(route.estadoOrigem, aux); //caso contrario, adiciona o conteúdo do campo lido ao registro auxiliar
        }
        //o método é repetido para todos os campos
        scan_quote_string(aux);
        if(strcmp(aux, "NULO")){
            strcpy(route.estadoDestino, aux);
        }
        scan_quote_string(aux);
        if(strcmp(aux, "NULO")){
            route.distancia = atoi(aux);
        }
        scan_quote_string(aux);
        if(strcmp(aux, "NULO")){
            strcpy(route.cidadeOrigem, aux);
        }
        scan_quote_string(aux);
        if(strcmp(aux, "NULO")){
            strcpy(route.cidadeDestino, aux);
        }
        scan_quote_string(aux);
        if(strcmp(aux, "NULO")){
            strcpy(route.tempoViagem, aux);
        }
        write_register(file, route,1); //o registro lido é escrito no final do arquivo
    }
    //atualiza-se o cabeçalho
    Header head;
    limpa_header(&head);
    make_header(&head, file);
    write_header(head, file);
    fclose(file);
}

/******************** FUNÇÃO 7 *****************************/

//substitui o conteúdo de campos especificados de registros indicados pelo rrn e reescreve os registros
//no arquivo sem alterar sua posição 
int update_field_rrn(char *fileName, int n){
    FILE *file = open_file_bin(fileName, "rb+",1);
    int i, tipo; //i é uma flag, tipo ajuda na leitura do tipo do campo a ser atualizado
    int rrn;
    char tipoCampo[15]; //string para leitura do tipo do campo a ser atualizado
    char novoCampo[40]; //string para leitura do conteudo a ser colocado no campo do registro de interesse
    Route reg; //auxiliar do tipo registro
    clear_route(&reg); //limpa o auxiliar
    for(i=0; i<n; i++){
        scanf ("%d %s", &rrn, tipoCampo);
        scan_quote_string(novoCampo);
        fseek(file,19,SEEK_SET); //leva a leitura do arquivo para após o cabeçalho
        fseek(file,rrn*TAM_REGISTRO,SEEK_CUR); //leva a leitura até o registro que se deseja atualizar

        tipo = dictionary_field(tipoCampo); //indica qual campo atualizar
        if(read_bin_rnn(file, rrn, &reg) == -1){ //indica se houve algum erro
            continue;
        }
        switch (tipo) //atualiza o campo desejado, conferindo se passará a ser um campo nulo
        {
            case 1:
                if(strcmp(novoCampo, "NULO")){
                    strcpy(reg.estadoOrigem, novoCampo);
                }else{
                    limpa_string(reg.estadoOrigem,3);
                }
                break;

            case 2:
                if(strcmp(novoCampo, "NULO")){
                    strcpy(reg.estadoDestino, novoCampo);
                }else{
                    limpa_string(reg.estadoDestino,3);
                }
                break;

            case 3:
                if(strcmp(novoCampo, "NULO")){
                    reg.distancia=atoi(novoCampo);
                }else{
                    reg.distancia=0;
                }
                break;

            case 4:
                if (strcmp(novoCampo, "NULO")){
                    strcpy(reg.cidadeOrigem, novoCampo);
                }else{
                    limpa_string(reg.cidadeOrigem,40);
                }
                break;

            case 5:
                if (strcmp(novoCampo, "NULO")){
                    strcpy(reg.cidadeDestino, novoCampo);
                }else{
                    limpa_string(reg.cidadeDestino,40);
                }
                break;

            case 6:
                if (strcmp(novoCampo, "NULO")){
                    strcpy(reg.tempoViagem, novoCampo);
                }else{
                    limpa_string(reg.tempoViagem, 10);
                }
                break;
        }
        fseek(file,19,SEEK_SET);
        fseek(file,rrn*TAM_REGISTRO,SEEK_CUR); //retorna a leitura para o inicio do registro que esta sendo atualizado
        write_register(file, reg, 0); //escreve o registro atualizado por cima de sua versao anterior
        
        //OBS: é passado o terceiro parametro "0" para que no caso em que o campo atualizado seja menor 
        //que o conteudo anterior, os bytes do final do registro nao sejam todos substituidos por "#". 
    }
    //proximas 4 linhas: atualização do cabeçalho
    Header head; 
    limpa_header(&head);
    make_header(&head, file);
    write_header(head, file);
    fclose(file);
    return 1;
}

/******************* FUNÇÃO 8 ************************/

//cria um arquivo possuindo somente os registros não-removidos
int compact_file(char *file_name, char* compacted_file_name){
    FILE * file = open_file_bin(file_name, "rb+",2);
    FILE * compacted_file = fopen(compacted_file_name, "wb+");
    int rrn = 0; //garante que os registros serao lidos a partir do começo
    Route route;
    Header head;
    limpa_header(&head);
    clear_route(&route);
    read_header(file,&head);
    get_current_date(head.data_ultima_compactacao);
    write_header(head,compacted_file);
    while(read_bin_rnn(file,rrn,&route) != -1){ //enquanto nao chegar ao fim do arquivo binario
        if(route.estadoOrigem[0] != '*'){ //caso o registro atual nao tenha sido removido
            write_register(compacted_file,route,1); //ele é adicionado ao arquivo compactado
        }
        rrn++;
    }
    fclose(file);
    fclose(compacted_file);
    return 0;
}

int main(){
    char funcao;
    char fileNameCSV[20]; //string para leitrua do nome do arquivo csv a ser aberto
    char fileNameBin[20]; //string para leitura do nome do arquivo binario a ser aberto
    char compacted_file_name[40]; //string para leitura do nome do arquivo compactado a ser aberto
    int n; //ferramenta para as funcionalidades 6 e 7

    scanf("%c", &funcao);

    switch(funcao){
        case '1':		// LEITURA DE DADOS
            scanf("%s %s", fileNameCSV,fileNameBin);
            if(read_csv(fileNameCSV,fileNameBin) != -1) //confere se houve algum erro
                binarioNaTela1(fileNameBin);

            break;
        case '2':		// RECUPERAÇÃO DE TODOS OS REGISTROS
            scanf("%s", fileNameBin);
            recover_data(open_file_bin(fileNameBin, "rb",1));

            break;
        case '3':		// RECUPERAÇÃO POR BUSCA
            scanf("%s", fileNameBin);
            recover_search(open_file_bin(fileNameBin, "rb",1));

            break;
        case '4':		// RECUPERAÇÃO DE REGISTROS POR RRN
            scanf("%s", fileNameBin);
            recover_rrn(open_file_bin(fileNameBin, "rb",1));

            break;
        case '5':		// REMOÇÃO DE REGISTROS
            scanf("%s", fileNameBin);
            if(remove_record(fileNameBin) != -1) //confere se houve algum erro
                binarioNaTela1(fileNameBin);

            break;
        case '6':		// INSERÇÃO DE REGISTROS ADICIONAIS
            scanf("%s %d", fileNameBin, &n); //n é o numero de registros que serao adicionados ao arquivo
            insert_regs(fileNameBin, n);
            binarioNaTela1(fileNameBin);
            break;

        case '7':		// ATUALIZAÇÃO DE REGISTRO POR RRN
            scanf("%s %d", fileNameBin, &n); //n é o numero de atualizações, sendo que cada atualização altera apenas 1 campo
            update_field_rrn(fileNameBin, n);
            binarioNaTela1(fileNameBin);
            break;

        case '8':		// COMPACTAÇÃO DO ARQUIVO
            scanf("%s", fileNameBin);
            scanf("%s", compacted_file_name);
            compact_file(fileNameBin,compacted_file_name);
            binarioNaTela1(compacted_file_name);
            break;


    }
    return 0;
}