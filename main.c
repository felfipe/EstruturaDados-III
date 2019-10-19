#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#define TAM_REGISTRO 85
#define TAM_ESTADO 2


typedef struct{
	char estadoOrigem[3];
	char estadoDestino[3];
	int  distancia;
	char cidadeOrigem[40];
	char cidadeDestino[30];
	char tempoViagem[10];
} Route;

// Estrutura de cabeçario com 19 bytes
typedef struct{
	int status;
	int numero_vertices;
	int numero_arestas;
	char data_ultima_compactacao;
} Header;

/*FILE *open_file(char* file_name, char* mode){
	FILE *file;
	file = fopen(file_name,mode);
	if(file != NULL)
		return file;
	return NULL;
}*/
void limpa_string(char* string, int tam){
	for(int i =0; i< tam;i++)
		string[i] = '\0';
}
int write_register(FILE* file, Route reg){
	fwrite(reg.estadoOrigem,TAM_ESTADO*sizeof(char),1,file);
	fwrite(reg.estadoDestino,TAM_ESTADO*sizeof(char),1,file);
	fwrite(&(reg.distancia),sizeof(int),1,file);
	fwrite(reg.cidadeOrigem,strlen(reg.cidadeOrigem)*sizeof(char),1,file);
	fwrite("|",sizeof(char),1,file);
	fwrite(reg.cidadeDestino,strlen(reg.cidadeDestino)*sizeof(char),1,file);
	fwrite("|",sizeof(char),1,file);
	fwrite(reg.tempoViagem,strlen(reg.tempoViagem)*sizeof(char),1,file);
	fwrite("|",sizeof(char),1,file);
	int size = TAM_REGISTRO - 2*TAM_ESTADO - sizeof(int) - strlen(reg.cidadeOrigem) - strlen(reg.cidadeDestino) - strlen(reg.tempoViagem) - 3;
	for(int i = 0; i < size; i++)
		fwrite("#",sizeof(char),1,file);
	return 0;
}
int read_csv(char* csv_name, char* bin_name){
	FILE *csv_file = fopen(csv_name,"r");
	FILE *bin_file = fopen(bin_name,"w+");
	if(csv_file == NULL || bin_file == NULL)
		return -1;
	char leitura[100];
	Route route;
	
	char a;
	while((a = fgetc(csv_file)) != '\n');
	while(fgetc(csv_file) != EOF){
		char dist[10];
		fseek(csv_file,-1,SEEK_CUR);
		limpa_string(route.cidadeOrigem,40);
		limpa_string(route.cidadeDestino,40);
		limpa_string(route.tempoViagem,10);
		limpa_string(dist,10);
		fread(route.estadoOrigem,2*sizeof(char),1,csv_file);
		fgetc(csv_file);
		fread(route.estadoDestino,2*sizeof(char),1,csv_file);
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
		write_register(bin_file,route);
	}
	return 0;
}

char *read_variable_string(FILE *file_bin,char *string_var){
	int i = 0;
	fread(&string_var[i],sizeof(char),1,file_bin);
	while (string_var[i] != '|'){
		fread(&string_var[i],sizeof(char),1,file_bin);
		i++;
	}
	string_var[i] = '\0';
	return string_var;
}

int read_bin_rnn(char* bin_name, int rnn, Route *route){
	int i = 0;
	FILE *file_bin = fopen(bin_name,"wb");
	fseek(file_bin,19+rnn*TAM_REGISTRO,SEEK_SET);
	fread(route->estadoOrigem,sizeof(char),2,file_bin);
	fread(route->estadoDestino,sizeof(char),2,file_bin);
	fread(&route->distancia,sizeof(int),1,file_bin);
	strcpy(route->cidadeOrigem,read_variable_string(file_bin,route->cidadeOrigem));
	strcpy(route->cidadeDestino,read_variable_string(file_bin,route->cidadeDestino));
    strcpy(route->tempoViagem,read_variable_string(file_bin,route->tempoViagem));
	return 1;
}


/*

void recupera_busca(){
	char nome_campo[15]; // nome do tipo do campo do que se busca
	char wntd_data[40]; // conteudo a se buscar
	int tipo_campo; // indica o tipo de campo lido em nome_campo

	scanf("%s %s", nome_campo, wntd_data);
	if(!strcmp(nome_campo, 'estadoOrigem')){
		tipo_campo = 1;
	}
	if(!strcmp(nome_campo, 'estadoDestino')){
		tipo_campo = 2;
	}
	if(!strcmp(nome_campo, 'distancia')){
		tipo_campo = 3;
	}
	if(!strcmp(nome_campo, 'cidadeOrigem')){
		tipo_campo = 4;
	}
	if(!strcmp(nome_campo, 'cidadeDestino')){
		tipo_campo = 5;
	}
	if(!strcmp(nome_campo, 'tempoViagem')){
		tipo_campo = 6;
	}
	
	
	return;
}

*/
int main(int argc, char *argv[]){
	switch(argv[1][0]){
		case '1':		// LEITURA DE DADOS
			read_csv(argv[2],argv[3]);

		break;
		case '2':		// RECUPERAÇÃO DE TODOS OS REGISTROS


		break;
		case '3':		// RECUPERAÇÃO POR BUSCA


		break;
		case '4':		// RECUPERAÇÃO DE REGISTROS POR RRN


		break;
		case '5':		// REMOÇÃO DE REGISTROS


		break;
		case '6':		// INSERÇÃO DE REGISTROS ADICIONAIS


		break;
		case '7':		// ATUALIZAÇÃO DE REGISTRO POR RRN

		break;
		case '8':		// COMPACTAÇÃO DO ARQUIVO


		break;


	}



	return 0;
}