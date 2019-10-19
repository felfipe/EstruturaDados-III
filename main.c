#include<stdio.h>
#include<stdlib.h>
typedef struct{
	char estadoOrigem[2];
	char estadoDestino[2];
	int  distancia;
	char cidadeOrigem[30];
	char cidadeDestino[30];
	char tempoViagem[10];
} Route;

typedef struct{
	int status;
	int numero_vertices;
	int numero_arestas;
	char data_ultima_compactacao[10];
} Header;


int read_csv(char* csv_name, char* bin_name){
	FILE *csv_file = fopen(csv_name,"r");
	FILE *bin_file = fopen(bin_name,"w");
	if(csv_file == NULL || bin_file == NULL)
		return -1;
	char route[30];
	printf("%s",route);
	return 0;
}



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