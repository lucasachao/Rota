#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

typedef struct _estrada{
	int u; //cidade 1
	int v; //cidade 2
	int p; //custo
} estrada;

typedef struct _cidade{
	int n; //cidades
	int m; //estradas
	int c; //cidades na rota de servico
    int k; //cidade origem
	estrada *estradas;
} cidade;

cidade *c;
bool imprimir = false;
int custo = 1e6, destino = 0, **grafo;

void reset()
{
    //free em dados antes de ler outro conjunto
    for(int i = 0; i < c->n; i ++)
        free(grafo[i]);
    free(grafo);

    free(c->estradas);
    free(c);

    //reseta variaveis
    custo = 1e6;
	destino = 0;
}

void rotaServico(int atual, int custo_novo)
{
	//percorre rota de servico ate chegar no destino
    while(atual < destino)
    {
		//verifica se a rota de servico eh valida antes de se mover
        if(grafo[atual][atual+1] == 0)
        {
			if(imprimir)
            	printf("Rota de servico deconexa!\n");
            return;
        }
        else
        {
            custo_novo += grafo[atual][atual+1];
            atual ++;
        }
    }

	if(custo_novo < custo)
    	custo = custo_novo;

	if(imprimir)
        printf("Rota possivel com custo %d (usou servico)\n", custo_novo);
}

void calculaRota(int atual, int custo_novo, bool visitadas[])
{
	//verifica se a posicao atual obriga a pegar a rota de servico
    if(atual < (destino))
		rotaServico(atual, custo_novo);
    else
		//para cada cidade do conjunto
        for(int i = 0; i < c->n; i++)
			//que seja alcancavel e nao visitada
            if(grafo[atual][i] != 0 && visitadas[i] == false)
            {
				//verifica se chegou ao destino
                if(i == destino)
                {
                    if(custo_novo + grafo[atual][i] < custo)
                        custo = custo_novo + grafo[atual][i];

                    if(imprimir)
                        printf("Rota possivel com custo %d\n", custo_novo + grafo[atual][i]);
                }
                else
                {
					//atualiza visitadas antes de continuar recursao
                    visitadas[i] = true;
                    calculaRota(i, custo_novo + grafo[atual][i], visitadas);
					//apaga recem visitada para continuar o loop atual
                    visitadas[i] = false;
                }
            }
}

void constroiGrafo()
{
    grafo = malloc(sizeof(int*)*c->n);
    for(int i = 0; i < c->n; i ++)
        grafo[i] = calloc(c->n,sizeof(int));

    //preenche matriz com valores das rotas
    for(int i = 0; i < c->m; i ++)
    {
        grafo[c->estradas[i].u][c->estradas[i].v] = c->estradas[i].p;
        grafo[c->estradas[i].v][c->estradas[i].u] = c->estradas[i].p;
    }

    if(imprimir)
    {
        printf("\nGrafo\n");
        for(int i = 0; i < c->n; i++)
        {
            for(int j = 0; j < c->n; j++)
                printf("%d\t", grafo[i][j]);
            printf("\n");
        }
        printf("\n");
    }
}

int leEntrada(FILE *p)
{
    c = malloc(sizeof(cidade));

    if(fscanf(p, "%d %d %d %d", &c->n, &c->m, &c->c, &c->k) != 4)
	{
		printf("Erro de leitura nas informacoes da cidade!\n");
		return -1;
	}

	if(getc(p) != '\n')
	{
		printf("Erro de leitura nas informacoes da cidade!\n");
		return -1;
	}

    if((c->n + c->m + c->c + c->k) == 0)
    {
        free(c);
		if(imprimir)
			printf("##########################################\nFim da lista!\n");
        return -2;
    }

	destino = c->c-1;

    c->estradas = malloc(sizeof(estrada)*c->m);

    for(int i = 0; i < c->m; i++)
	{
        if(fscanf(p, "%d %d %d", &c->estradas[i].u, &c->estradas[i].v, &c->estradas[i].p) != 3)
		{
			printf("Erro na leitura das estradas!\n");
			return -1;
		}
		if(getc(p) != '\n')
		{
			printf("Erro de leitura das estradas!\n");
			return -1;
		}
	}

    if(imprimir)
    {
        printf("##########################################\nCONJUNTO\n");
        printf("n = %d m = %d c = %d k = %d\n\n", c->n, c->m, c->c, c->k);
        printf("Rotas\n");
        for(int i = 0; i < c->m; i++)
            printf("u = %d v = %d p = %d\n", c->estradas[i].u, c->estradas[i].v, c->estradas[i].p) ;
    }

    return 0;
}

int main(int argc, char **argv)
{
	char local[40] = {"entrada.txt"};

	for(int i = 0; i < argc; i++)
	{
		//habilita impressao de informacoes durante o processamento
        if(strcmp(argv[i], "0") == 0)
            imprimir = true;
		//muda o arquivo de entrada
        else if(memcmp(argv[i], "-", 1) == 0)
            strcpy(local, argv[i]+1);
	}

	//abre arquivo de entrada
	FILE *p;
	p = fopen(local, "r");
	if (p == NULL){
		printf("Arquivo nao encontrado!\n");
		return EXIT_FAILURE;
	}

    while(true)
    {
		//le arquivo "entrada.txt" e coloca valores em cidade c
        if(leEntrada(p) != 0)
            break;

		//transforma rotas em matriz (representacao de grafo)
        constroiGrafo();

        //prepara array de cidades visitadas
        bool visitadas[c->n];
        for(int i = 0; i < c->n; i++)
            visitadas[i] = false;
        visitadas[c->k] = true;

		//metodo recursivo para encontrar rota
        calculaRota(c->k, 0, visitadas);

		//imprime resultado final
		if(custo != 1e6)
		{
			if(imprimir)
				printf("\nMenor Custo encontrado: ");
        	printf("%d\n", custo);
		}
		else
			printf("Nao foi possivel encontrar uma rota!\n");

        reset();
    }

    free(p);
    return EXIT_SUCCESS;
}
