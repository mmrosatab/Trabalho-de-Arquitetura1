#include <stdio.h>
#include <stdlib.h>
#include <papi.h>
#include <math.h>
#include <time.h>
#define MAX 1000

int pot(int base, int exp)
{
	if (exp == 0)
	return 1;

	else if(exp == 1)
	return base;

	return base * pot(base, exp - 1);
}

long int * alocarMemoria(long int tam)
{
	int i;

	// vetor 
	long int *vetor;

	vetor = (long int*) malloc(tam* sizeof(long int));

	if(vetor == NULL)
	{
		printf("Nao foi possivel alocar memoria para o vetor\n");
		return 0;
	}

	return vetor;
}

void liberarMemoria(long int *vetor)
{
	free(vetor);
}

void inicializarVetor(long int *vetor, int tam)
{
	int i;

	for(i = 0; i < tam; i++)
	{
		vetor[i] = rand() % MAX;
	}
}
void heap(long int *vetor, int inicio, int fim)
{
	int aux   = vetor[inicio];
	int filho = (inicio*2)+1;

	while(filho <= fim)
	{
		if(filho < fim)
		{
			if(vetor[filho] < vetor[filho+1])
				filho++;
		}
		if(aux < vetor[filho])
		{
			vetor[inicio] = vetor[filho];
			inicio = filho;
			filho = (inicio*2)+1;
		}
		else
		{
			filho = fim+1;
		}

		vetor[inicio] = aux;
	} 
}

void heapSort(long int *vetor, int tam)
{
	int meio = tam/2;
	int i, aux;

	for(i = meio; i >= 0; i--)
	{
		heap(vetor, i, tam-1);
	}
	for(i = tam-1; i >=0; i--)
	{
		aux = vetor[0];
		vetor[0] = vetor[i];
		vetor[i] = aux;
		heap(vetor,0, i - 1);
	}
}

void imprimirVetor(long int *vetor, int n)
{
	int i;

	for(i = 0; i < n; i++)
	{
		printf("%ld ", vetor[i]);
	}

	printf("\n");
	printf("\n");
}

int dadosTrafegados(int tam,int tamCache)
{
	return ((tam*tam*sizeof(double))/tamCache);
}

int main(void)
{

	srand(time(NULL));
	
	int j,i;
	int tam;
	
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

	FILE *arquivo = fopen("heapsortflops.txt","w");

	for(j=10;j<=30;j++)
	{

	tam = pot(2,j)/sizeof(long int);

	printf("Tamanho instancia %d\n",tam);
	fprintf(arquivo,"Tamanho instancia %d.\n",tam);
	

  	long int *vetor = alocarMemoria(tam);

	inicializarVetor(vetor,tam);

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

  		heapSort(vetor,tam);

  		if((retval=PAPI_flops( &real_time, &proc_time, &flpops, &mflops))<PAPI_OK)
  		{    
   			printf("retval: %d\n", retval);
    		//exit(1);
  		}
	

  		printf("Teste numero %i Real_time: %f Proc_time: %f Total flpops: %lld MFLOPS: %f\n",i+1, real_time, proc_time,flpops,mflops);

  		e = PAPI_get_real_cyc();
  		
  		fprintf(arquivo,"Teste: %d\n",i);
		fprintf(arquivo,"Flops: %lld\n",flpops);
		fprintf(arquivo,"Tempo Real em segundos: %f\n",real_time);
		fprintf(arquivo,"Tempo Decorrido em segundos: %f\n",proc_time);
		fprintf(arquivo,"Ciclos : %lld\n",e-s);
		fprintf(arquivo,"Dados Trafegados: %d\n",dadosTrafegados(tam,L[0].cache[0].line_size));
		fprintf(arquivo,"\n");

		

		/*

  		if((retval=PAPI_flips(&ireal_time,&iproc_time,&iflpins,&imflips)) < PAPI_OK)
  		{ 
  			printf("\n");
    		printf("Could not initialise PAPI_flips \n");
    		printf("Your platform may not support floating point instruction event.\n");    
    		printf("retval: %d\n", retval);
   	 		//exit(1);
  		}
  			heapSort(vetor,tam);
  
  		if((retval=PAPI_flips( &real_time, &proc_time, &flpins, &mflips))<PAPI_OK)
  		{    
    		printf("retval: %d\n", retval);
    		//exit(1);
  		}

  		printf("Total flpins: %lld ", flpins);

  		e = PAPI_get_real_cyc();

  		fprintf(arquivo,"Teste: %d\n",i);
		fprintf(arquivo,"Flips: %lld\n",flpins);
		fprintf(arquivo,"Tempo Real: %f\n",real_time);
		fprintf(arquivo,"Tempo Decorrido: %f\n",proc_time);
		fprintf(arquivo,"Ciclos : %lld\n",e-s);
		fprintf(arquivo,"Dados Trafegados: %d\n",dadosTrafegados(tam,L[0].cache[0].line_size));
		fprintf(arquivo,"\n");
		*/
  		
	}

	//imprimirVetor(vetor,tam);
	}
	//liberarMemoria(vetor);

	fclose(arquivo);

	return 0;
}
