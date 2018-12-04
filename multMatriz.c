#include <stdio.h>
#include <stdlib.h>
#include <papi.h>
#include <time.h>
#include "arquivo.h"
#include <math.h>
#define MAX 1000

int pot(int base, int exp)
{
	if (exp == 0)
	return 1;

	else if(exp == 1)
	return base;

	return base * pot(base, exp - 1);
}

double **alocarMemoria(int tam)
{
	int i,j;

	// vetor de ponteiro
	double **vetPonteiro;
	//posix_memalign((void**)&matriz, 64, n*n*sizeof(double));
	vetPonteiro = (double**) malloc(tam * sizeof(double*));

	if(vetPonteiro == NULL)
	{
		printf("Nao foi possivel alocar memoria para a matriz\n");
		return 0;
	}
	// pra cada posicao do vetor de ponteiro aloca um vetor de ponteiros
	for (i = 0; i < tam; i++)
	{
		vetPonteiro[i] = (double*) malloc(tam * sizeof(double));
	}

	return vetPonteiro;
}

int liberarMemoria(double **vetPonteiro,int tam)
{
	int i;

	if(vetPonteiro == NULL)
	{
		printf("Nao foi possível liberar a matriz\n");
		return 0;
	}

	for (i = 0; i < tam; i++)
	{
		free(vetPonteiro[i]);
	}

	free(vetPonteiro);
	return 1;
}

void inicializarMatriz(double **matriz,int tam)
{
	int i,j;

	for (i = 0; i < tam; i++)
	{
		for (j = 0; j < tam; j++)
		{
			matriz[i][j] = rand() % MAX;
		}
	}
}

void imprimirMatriz(double **matriz,int tam)
{
	int i,j;

	printf("\nImprimindo Matriz \n");
	for (i = 0; i < tam; i++)
	{
		for (j = 0; j < tam; j++)
		{
			printf("%.1f ",matriz[i][j]);
		}
		printf("\n");
	}
	printf("\n");
}

double **multiplicarMatrizes(double **matriz1,double **matriz2,int tam)
{
	
	double **matrizR = alocarMemoria(tam);

	if(matrizR == NULL){
		printf("Nao foi possivel alocar memoria\n");
		return NULL;
	}

	int i,j,k;
	//float soma;

		for(i=0;i<tam;i++)
		{

			for (j=0;j<tam;j++)
			{	
				//soma = 0;	
				for(k=0;k<tam;k++)
				{
					//soma = soma + matriz1[i][k] * matriz2[k][j];  
					matrizR[i][j] = matrizR[i][j] + matriz1[i][k] * matriz2[k][j];
				}

				//matrizR[i][j] = soma;
			
			}
		}

	return matrizR;
}

int dadosTrafegados(int tam,int tamCache)
{
	return ((tam*tam*sizeof(double)*2)/tamCache);
}

int main(void)
{

	int i,j;
	int tam;
	srand(time(NULL));
	long long s,e;

	float real_time, proc_time,mflops;
  	long long flpops;
  	float ireal_time, iproc_time, imflops;
  	long long iflpops;
  	int retval;

  	float mflips;
    long long flpins;
    float imflips;
    long long iflpins;
	
	FILE *arquivo = fopen("multMatrizflops.txt","w");

	for(j=10;j<=15;j++)
	{

  	tam = pot(2,j)/sizeof(double);
	printf("Tamanho instancia %d\n",tam);
	fprintf(arquivo,"Tamanho instancia %d.\n",tam);

	double **matriz1 = alocarMemoria(tam);
	double **matriz2 = alocarMemoria(tam);
	double **matrizR;
	
	inicializarMatriz(matriz1,tam);
	inicializarMatriz(matriz2,tam);

	PAPI_mh_level_t *L;
	
	PAPI_library_init( PAPI_VER_CURRENT );

	const PAPI_hw_info_t *hwinfo = PAPI_get_hardware_info(  );

	if(hwinfo == NULL)
	{
		printf("a struct é nula !\n");
	}

	L = ( PAPI_mh_level_t * ) ( hwinfo->mem_hierarchy.level );

	for(i=0;i<10;i++)
	{

		s = PAPI_get_real_cyc();

	 	if((retval=PAPI_flops(&ireal_time,&iproc_time,&iflpops,&imflops)) < PAPI_OK)
  		{ 
    		printf("Could not initialise PAPI_flops \n");
    		printf("Your platform may not support floating point operation event.\n"); 
    		printf("retval: %d\n", retval);
    		//exit(1);
  		}

  		matrizR = multiplicarMatrizes(matriz1,matriz2,tam);

  		if((retval=PAPI_flops( &real_time, &proc_time, &flpops, &mflops))<PAPI_OK)
  		{    
   			printf("retval: %d\n", retval);
    		//exit(1);
  		}

  		
  		printf("Real_time: %f Proc_time: %f Total flpops: %lld MFLOPS: %f\n", real_time, proc_time,flpops,mflops);

  		e = PAPI_get_real_cyc();

  		fprintf(arquivo,"teste: %d\n",i);
		fprintf(arquivo,"Flops: %lld\n",flpops);
		fprintf(arquivo,"Tempo Real: %f\n",real_time);
		fprintf(arquivo,"Tempo Decorrido: %f\n",proc_time);
		fprintf(arquivo,"Ciclos : %lld\n",e-s);
		fprintf(arquivo,"Dados Trafegados: %d\n",dadosTrafegados(tam,L[0].cache[0].line_size));
		fprintf(arquivo,"\n");

  		// pegando flips
  		/*
  		if((retval=PAPI_flips(&ireal_time,&iproc_time,&iflpins,&imflips)) < PAPI_OK)
  		{ 
  			printf("\n");
    		printf("Could not initialise PAPI_flips \n");
    		printf("Your platform may not support floating point instruction event.\n");    
    		printf("retval: %d\n", retval);
   	 		//exit(1);
  		}

  		matrizR = multiplicarMatrizes(matriz1,matriz2,tam);
  
  		if((retval=PAPI_flips( &real_time, &proc_time, &flpins, &mflips))<PAPI_OK)
  		{    
    		printf("retval: %d\n", retval);
    		//exit(1);
  		}

  		printf("Total flpins: %lld\n",flpins);

  		e = PAPI_get_real_cyc();

  		fprintf(arquivo,"teste: %d\n",i);
		fprintf(arquivo,"Flips: %lld\n",flpins);
		fprintf(arquivo,"Tempo Real: %f\n",real_time);
		fprintf(arquivo,"Tempo Decorrido: %f\n",proc_time);
		fprintf(arquivo,"Ciclos : %lld\n",e-s);
		fprintf(arquivo,"Dados Trafegados: %d\n",dadosTrafegados(tam,L[0].cache[0].line_size));
		fprintf(arquivo,"\n");
  		*/

  		
  	}
	//imprimirMatriz(matrizR,tam);
  	}

	//liberarMemoria(matriz1,tam);
	//liberarMemoria(matriz2,tam);

	fclose(arquivo);

	return 0;
}
