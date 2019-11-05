#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include "funcoes_uteis.h"

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
	char data_ultima_compactacao[10];
} Header;

FILE *open_file(char* file_name, char* mode){
	FILE *file;
	file = fopen(file_name,mode);
	if(file != NULL)
		return file;
	return NULL;
}

int close_file(FILE* file){
	if(file != NULL){
		fclose(file);
		return 1;
	}
	return -1;
	
}

void limpa_string(char* string, int tam){
	for(int i =0; i< tam;i++)
		string[i] = '\0';
}

void clear_route(Route *route){
	limpa_string(route->estadoOrigem,3);
	limpa_string(route->estadoDestino,3);
	limpa_string(route->cidadeOrigem,40);
	limpa_string(route->cidadeDestino,40);
	limpa_string(route->tempoViagem,10);
	route->distancia = 0;
}
int write_header(Header head, FILE *file){
	fwrite(&(head.status),sizeof(int),1,file);
	fwrite(&(head.numero_vertices),sizeof(int),1,file);
	fwrite(&(head.numero_arestas),sizeof(int),1,file);
	fwrite(head.data_ultima_compactacao,10*sizeof(char),1,file);
	return 0;
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
int query_city_file(FILE* cities, char *city){
		char query[40];
		while(!fread(query,40*sizeof(char),1,cities)){
			if(!strcmp(city,query))
				return 0;
		}
		fwrite(city,40*sizeof(char),1,cities);
		return 1;
}
int read_csv(char* csv_name, char* bin_name){
	FILE *csv_file = fopen(csv_name,"r");
	FILE *bin_file = fopen(bin_name,"w+");
	FILE *head_file = fopen("head.bin","w+");
	if(csv_file == NULL || bin_file == NULL)
		return -1;
	char leitura[100];
	Route route;
	Header head;
	char a;
	
	while((a = fgetc(csv_file)) != '\n');
	while((a = fgetc(csv_file)) != '\n');
	while((a = fgetc(csv_file)) != '\n');
	while((a = fgetc(csv_file)) != '\n');
	while((a = fgetc(csv_file)) != '\n');
	while((a = fgetc(csv_file)) != '\n');
	while((a = fgetc(csv_file)) != '\n');
	while((a = fgetc(csv_file)) != '\n');
	while((a = fgetc(csv_file)) != '\n');
	while((a = fgetc(csv_file)) != '\n');
	while((a = fgetc(csv_file)) != '\n');
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
	fclose(bin_file);
	fclose(csv_file);
	fclose(head_file);
	return 0;
}
void read_variable_string(FILE *file_bin,char *string_var){
	int i = 0;
	while(1){
		printf("%c",string_var[i]);
		fread(&string_var[i],sizeof(char),1,file_bin);
		if(string_var[i] == '|')
			break;
		i++;
	}
	string_var[i] = '\0';
}

int read_bin_rnn(FILE* file, int rnn, Route *route){
	if(fseek(file,rnn*TAM_REGISTRO,SEEK_SET) != 0)
		return -1;
	fread(route->estadoOrigem,2*sizeof(char),1,file);
	//printf("%s\n",route->estadoOrigem);
	fread(route->estadoDestino,2*sizeof(char),1,file);
	//printf("%s\n",route->estadoDestino);
	fread(&route->distancia,sizeof(int),1,file);
	//printf("%d\n",route-> distancia);
	read_variable_string(file,route->cidadeOrigem);
	//printf("aaaaa");
	read_variable_string(file,route->cidadeDestino);
    read_variable_string(file,route->tempoViagem);
	return 1;
}
/*
void recover_data(FILE* file){
	int i = 0;
	int num_registros;
	Route route;

	fseek(file,2*sizeof(int)+sizeof(char),SEEK_SET);
	fread(&num_registros,sizeof(int),1,file);
	fseek(file,0,SEEK_CUR);


	for(i=0;i<num_registros;i++){
		read_bin_rnn(file,i,&route);
		printf("%d %s %s %d %s %s %s",i,route.estadoOrigem,route.estadoDestino,route.distancia,route.cidadeOrigem,
		route.cidadeDestino,route.tempoViagem);
	}
}
*/
void recover_data(FILE* file){
	int i = 0;
	int num_registros;
	Route route;

	clear_route(&route);
	while(read_bin_rnn(file,i,&route) != -1&& i != 1){
		printf("%d %s %s %d %s %s %s \n",i,route.estadoOrigem,route.estadoDestino,route.distancia,route.cidadeOrigem,
		route.cidadeDestino,route.tempoViagem);
		i++;
	}
	close_file(file);
}

void recover_search(FILE* file){
	char nome_campo[15]; // nome do tipo do campo do que se busca
	char wntd_data[40]; // conteudo a se buscar
	int tipo_campo; // indica o tipo de campo lido em nome_campo
	Route route;
	clear_route(&route);
	int i=0;
	int flag=0;
	int aux;

	scanf("%s %s", nome_campo, wntd_data); // TALVEZ, pode ser os argv
	if(!strcmp(nome_campo, "estadoOrigem")){
		tipo_campo = 1;
	}
	if(!strcmp(nome_campo, "estadoDestino")){
		tipo_campo = 2;
	}
	if(!strcmp(nome_campo, "distancia")){
		tipo_campo = 3;
	}
	if(!strcmp(nome_campo, "cidadeOrigem")){
		tipo_campo = 4;
	}
	if(!strcmp(nome_campo, "cidadeDestino")){
		tipo_campo = 5;
	}
	if(!strcmp(nome_campo, "tempoViagem")){
		tipo_campo = 6;
	}
	
	while (read_bin_rnn(file,i,&route) != -1)
	{
		switch (tipo_campo)
		{
			case  1 :
				if(!strcmp(nome_campo,route.estadoOrigem)){
					printf("%d %s %s %d %s %s %s \n",i,route.estadoOrigem,route.estadoDestino,route.distancia,route.cidadeOrigem,
					route.cidadeDestino,route.tempoViagem);	
					flag++;
				}
				break;
			
			case 2 :
				if (!strcmp(nome_campo,route.estadoDestino)){
					printf("%d %s %s %d %s %s %s \n",i,route.estadoOrigem,route.estadoDestino,route.distancia,route.cidadeOrigem,
					route.cidadeDestino,route.tempoViagem);	
					flag++;		
				}
				break;

			case 3 :
				aux=atoi(nome_campo);
				if(aux==route.distancia){
					printf("%d %s %s %d %s %s %s \n",i,route.estadoOrigem,route.estadoDestino,route.distancia,route.cidadeOrigem,
					route.cidadeDestino,route.tempoViagem);	
					flag++;
				}
				break;

			case 4 :
				if(!strcmp(nome_campo,route.cidadeOrigem)){
					printf("%d %s %s %d %s %s %s \n",i,route.estadoOrigem,route.estadoDestino,route.distancia,route.cidadeOrigem,
					route.cidadeDestino,route.tempoViagem);	
					flag++;
				}
				break;

			case 5 :
				if(!strcmp(nome_campo,route.cidadeDestino)){
					printf("%d %s %s %d %s %s %s \n",i,route.estadoOrigem,route.estadoDestino,route.distancia,route.cidadeOrigem,
					route.cidadeDestino,route.tempoViagem);	
					flag++;
				}
				break;

			case 6 :
				if(!strcmp(nome_campo,route.tempoViagem)){
					printf("%d %s %s %d %s %s %s \n",i,route.estadoOrigem,route.estadoDestino,route.distancia,route.cidadeOrigem,
					route.cidadeDestino,route.tempoViagem);	
					flag++;
				}
				break;
		
			default:
				printf("Erro na sintaxe do tipo de campo!");
				break;
		}
		i++;
	}
	if(!flag)
		printf("Registro inexistente.");
	fclose(file);
}

void recover_rrn(FILE* file){
	Route route;
	clear_route(&route);
	int rrn;
	scanf(" %d", rrn);
	if(read_bin_rnn(file,rrn,&route)!=(-1)){
		printf("%d %s %s %d %s %s %s \n",rrn,route.estadoOrigem,route.estadoDestino,route.distancia,route.cidadeOrigem,
		route.cidadeDestino,route.tempoViagem);
	}else{
		printf("Registro inexistente");
	}	
	fclose(file);
}


int main(int argc, char *argv[]){
	switch(argv[1][0]){
		case '1':		// LEITURA DE DADOS
			read_csv(argv[2],argv[3]);

		break;
		case '2':		// RECUPERAÇÃO DE TODOS OS REGISTROS
			printf("%d",0x00003209);
			recover_data(open_file(argv[2],"rb"));

		break;
		case '3':		// RECUPERAÇÃO POR BUSCA
			recover_search(open_file(argv[2],"rb"));

		break;
		case '4':		// RECUPERAÇÃO DE REGISTROS POR RRN
			recover_rrn(open_file(argv[2],"rb"));

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