#include <stdio.h>
#include <stdlib.h>
#include <papi.h>

// gcc LU.c -o LU -I/usr/local/share/papi/testlib /usr/local/share/papi/testlib/libtestlib.a /usr/local/lib/libpapi.a

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
	int i,j, k;

	for (i = 0; i < tam; i++)
	{
		for (j = 0; j < tam; j++)
		{
			//matriz[i][j] = rand() % MAX;
			k = i - j;

			if(k < 0)
				k = k * -1;

			if(k >= 2)
				matriz[i][j] = 0;
			else if(i == j)
				matriz[i][j] = 6.283034;
			else
				matriz[i][j] = -3.141517;
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
			printf("%.5f ",matriz[i][j]);
		}
		printf("\n");
	}
	printf("\n");
}


void Identidade(double **matrizIdentidade,int tam)
{
	int i,j;

	for (i = 0; i < tam; i++)
	{
		for (j = 0; j < tam; j++)
		{
			if(i==j)
			{
				matrizIdentidade[i][j] = 1;
			}else
			{
				matrizIdentidade[i][j] = 0;
			}
		}
	}
}


void fatoracaoLu(double **L, double **U,int tam)
{
	int j,k,i;

	for (k=0;k<tam;k++)
	{
		for (j=k+1;j<=tam-1;j++)
		{
			L[j][k] = U[j][k]/U[k][k];

			for (i=k;i<tam;i++)
			{
				U[j][i] = U[j][i] - L[j][k]*U[k][i];

			}

		}

	}

}

int dadosTrafegados(int tam,int tamCache)
{
	return ((tam*tam*sizeof(double)*2)/tamCache);
}

int main(void)
{

	int i,j;

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

	FILE *arquivo = fopen("luflops.txt","w");

	for(j=10;j<=15;j++)
	{

	tam = pot(2,j)/sizeof(double);
	
	printf("Tamanho instancia %d\n",tam);
	fprintf(arquivo,"Tamanho instancia %d.\n",tam);
	
	double **L = alocarMemoria(tam);
	double **U = alocarMemoria(tam);
	
	Identidade(L,tam);
	inicializarMatriz(U,tam);

	// L = indentidade e U = A

	// matriz que o professor colocou no quadro
	/*
	U[0][0] = 6.283034; 
	U[0][1] = -3.141517;
	U[0][2] = 0;
	U[0][3] = 0;
	
	U[1][0] = -3.141517;
	U[1][1] = 6.283034;
	U[1][2] = -3.141517;
	U[1][3] = 0;

	U[2][0] = 0;
	U[2][1] = -3.141517;
	U[2][2] = 6.283034;
	U[2][3] = -3.141517;

	U[3][0] = 0;
	U[3][1] = 0;
	U[3][2] = -3.141517;
	U[3][3] = 6.283034;
	*/

	PAPI_mh_level_t *linha;
	
	PAPI_library_init( PAPI_VER_CURRENT );

	const PAPI_hw_info_t *hwinfo = PAPI_get_hardware_info(  );

	if(hwinfo == NULL)
	{
		printf("a struct é nula !\n");
	}

	linha = ( PAPI_mh_level_t * ) ( hwinfo->mem_hierarchy.level );

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
  	

  		fatoracaoLu(L,U,tam);

  		if((retval=PAPI_flops( &real_time, &proc_time, &flpops, &mflops))<PAPI_OK)
  		{    
   			printf("retval: %d\n", retval);
    		//exit(1);
  		}


  		printf("Real_time: %f Proc_time: %f Total flpops: %lld MFLOPS: %f\n", 
         real_time, proc_time,flpops,mflops);

  		e = PAPI_get_real_cyc();

  		fprintf(arquivo,"Teste: %d\n",i);
		fprintf(arquivo,"Flops: %lld\n",flpops);
		fprintf(arquivo,"Tempo Real em segundos: %f\n",real_time);
		fprintf(arquivo,"Tempo Decorrido em segundos: %f\n",proc_time);
		fprintf(arquivo,"Ciclos : %lld\n",e-s);
		fprintf(arquivo,"Dados Trafegados: %d\n",dadosTrafegados(tam,L[0].cache[0].line_size));
		fprintf(arquivo,"\n");

  		/*
  		// pegando flips

  		if((retval=PAPI_flips(&ireal_time,&iproc_time,&iflpins,&imflips)) < PAPI_OK)
  		{ 
  			printf("\n");
    		printf("Could not initialise PAPI_flips \n");
    		printf("Your platform may not support floating point instruction event.\n");    
    		printf("retval: %d\n", retval);
   	 		//exit(1);
  		}

  		fatoracaoLu(L,U,tam);
  
  		if((retval=PAPI_flips( &real_time, &proc_time, &flpins, &mflips))<PAPI_OK)
  		{    
    		printf("retval: %d\n", retval);
    	//exit(1);
  		}

  		printf("Total flpins: %lld ", flpins);

  		e = PAPI_get_real_cyc();

  		fprintf(arquivo,"Teste: %d\n",i);
		fprintf(arquivo,"Flips: %lld\n",flpins);
		fprintf(arquivo,"Tempo Real em segundos: %f\n",real_time);
		fprintf(arquivo,"Tempo Decorrido em segundos: %f\n",proc_time);
		fprintf(arquivo,"Ciclos : %lld\n",e-s);
		fprintf(arquivo,"Dados Trafegados: %d\n",dadosTrafegados(tam,L[0].cache[0].line_size));
		fprintf(arquivo,"\n");
		

		*/

	
  	}
	//imprimirMatriz(L,tam);
	//imprimirMatriz(U,tam);

	//liberarMemoria(L,tam);
	//liberarMemoria(U,tam);

	fclose(arquivo);
	}

	return 0;
}
