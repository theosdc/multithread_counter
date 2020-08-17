/******************************************************************************/
/* Theo Soares de Camargo - 206191 - LAB.5                                    */
/* EA876 - 1S2020 - 11 08 2020                                                */
/******************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
/*============================================================================*/
/*= Definicoes                                                               =*/
/*============================================================================*/
#define N_MAX 30		/* Numero maximo de itens no thread pool */
#define T_MAX 4			/* Numero maximo de threads */

typedef enum {			/* Define um tipo para verdadeiro e falso */
    TRUE,
    FALSE
} bool;


/*============================================================================*/
/*= Variaveis globais                                                        =*/
/*============================================================================*/
unsigned int completos[N_MAX];	 /* Indica quais numeros ja foram analisados */
unsigned long long int numeros[N_MAX];/* Numeros a serem analisados */
unsigned int primos = 0;	 /* Contador de numeros primos */

pthread_mutex_t trava;

/*============================================================================*/
/*= Funcoes                                                                  =*/
/*============================================================================*/

int ini_thread_pool(void) {
/*============================================================================*/
/*= Incializa os vetores para o thread pool                                  =*/
/*= 1.) Coloca os dados da entrada no vetor 'numeros';                       =*/
/*= 2.) Coloca zero em todos os elementos de vetor 'completos';              =*/
/*= 3.) Retorna o numero de elementos do vetor 'numeros'.                    =*/
/*= Obs:                                                                     =*/
/*=   O numero maximo de entradas eh especificado por N_MAX (ver definicoes) =*/
/*============================================================================*/

    /* Leitura da entrada padrao -------------------------------------------- */
    int n = 0;
    char aux;

    do {			/* Gera um vetor com os numeros da entrada */
	scanf("%lld%c", &numeros[n], &aux);
	n++;
    }while(aux != '\n');

    /* Inicializacao do vetor de numeros analisados ------------------------- */
    for (int i = 0; i < N_MAX; i++) {
	completos[i] = 0;
    }
    return(n);
}

bool primo_teste(unsigned long long int n) {
/*============================================================================*/
/*= Teste de primalidade com otimizacao  6k+-1                               =*/
/*= Disponivel em: <https://en.wikipedia.org/wiki/Primality_test>            =*/
/*= Obs: n eh um numero inteiro                                              =*/
/*============================================================================*/

    if ( n <= 1) {		/* Caso n menor ou igual a 1 */
	return(FALSE);

    } else if ( n <= 3) {	/* Caso 1 < n <= 3 */
	return(TRUE);

    }else if ( (n % 2 == 0) || (n % 3 == 0) ) { /* Caso n seja divisivel por */
	return(FALSE);				/* 2 ou por 3 */

    } else {
	unsigned long long int i = 5;

	while (i*i <= n) {	/* Otimização 6k+1 */
	    if ((n % i == 0) || (n % (i+2) == 0)) {
		return(FALSE);
	    }

	    i += 6;
	}

	return(TRUE);
    }
}

void* funcao_thread(void* arg) {
/*============================================================================*/
/*= Threads                                                                  =*/
/*=                                                                          =*/
/*= Utilizando o padrao 'Thread Pool', as threads dividem o processamento    =*/
/*= dos numeros primos. Cada thread recebe como argumento o numero total de  =*/
/*= elementos do vetor 'numeros'.                                            =*/
/*============================================================================*/

    int* n;			/* Conversao do argumento */
    n = (int*) arg;

    int instancia = 0;
    for( ; ;) {

	/* Regiao critica - leitura do vetor 'completos' */
	pthread_mutex_lock(&trava);

	/*Percorre o vetor dos numeros ja analisados para achar uma posicao
	  livre */
	while((completos[instancia] != 0) && (instancia < *n)) {
	    instancia++;
	}

	/* Verifica se acabou o vetor 'completos' */
	if(instancia >= *n) {
	    pthread_mutex_unlock(&trava);
	    break;
	}
	completos[instancia] = 1;	/* Indica que o numero correspondente
					   sera analisado por essa thread  */
	pthread_mutex_unlock(&trava);

	 /* Verifica a primaridade */
	if (primo_teste(numeros[instancia]) == TRUE) {
	    primos++;
	}
    }
    return(NULL);
}

/******************************************************************************/
/* Programa Principal                                                         */
/******************************************************************************/
int main(int argc, char **argv) {

    int n;			/* Numero elementos no vetor de entrada */
    n = ini_thread_pool();	/* Inicializa a Thread Pool */

    /* Calculo do numero de threads ----------------------------------------- */

    int n_threads;
    if (n > T_MAX) {		/* Limita o numero de Threads */
	n_threads = T_MAX;
    } else {
	n_threads = n;
    }

    /* Disparo das threads -------------------------------------------------- */
    pthread_t threads[n_threads];

    void* arg;
    arg = (void*) &n;		/* O argumento eh numero de elementos do */
                                /* vetor de 'numeros' */

    for(int i = 0; i < n_threads; i++) {
	pthread_create(&threads[i], NULL, funcao_thread, arg);
    }

    /* Espera as threads terminarem ----------------------------------------- */
    for (int i = 0; i < n_threads; i++) {
	pthread_join(threads[i], NULL);
    }

    /* Imprime o resultado -------------------------------------------------- */
    printf("%d\n", primos);
    return(0);
}
