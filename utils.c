#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "algoritmo.h"
#include "utils.h"

// Lê os dados do ficheiro no formato especificado
// O ficheiro contém o número de moedas, o valor alvo, os valores das moedas
struct info ler_dados(char *filename, int mat[][2])
{
    struct info x;
    FILE *f;
    int i;
    float V_aux;
    float moedas_aux[MAX_OBJ];

    f = fopen(filename, "rt");
    if (!f)
    {
        printf("Ficheiro nao encontrado\n");
        exit(1);
    }

    // Leitura dos parâmetros do problema
    fscanf(f, "%d %f", &x.n_Moedas, &V_aux);
    x.valor_alvo = (int)(V_aux * 100); // Multiplica por 100 e converte para int

    for (i = 0; i < x.n_Moedas; i++)
    {
        fscanf(f, "%f", &moedas_aux[i]);
        x.moedas[i] = (int)(moedas_aux[i] * 100); // Multiplica por 100 e converte para int
    }

    printf("Numero de moedas: %d\n", x.n_Moedas);
    printf("Valor desejado: %d\n", x.valor_alvo);
    printf("Moedas: ");
    for (i = 0; i < x.n_Moedas; i++)
        printf("%d ", x.moedas[i]);
    printf("\n");


    // Definir outros parâmetros
    x.popsize = 100; // Exemplo de tamanho da população
    x.pm = 0.01;     // Exemplo de probabilidade de mutação
    x.pr = 0.7;      // Exemplo de probabilidade de recombinação
    x.tamTor = 2;     // Exemplo de tamanho do torneio
    x.n_Geracoes = 5000; // Exemplo de número de gerações
    x.ro = 0.0;      // Exemplo de constante para avaliação com penalização

    fclose(f);
    return x;
}

// Inicializa o gerador de números aleatórios
void init_rand()
{
    srand((unsigned)time(NULL)); // Define a semente com base no tempo atual
}

// Retorna um valor inteiro aleatório entre min e max (inclusive)
int random_l_h(int min, int max)
{
    return min + rand() % (max - min + 1);
}

// Retorna um valor real aleatório no intervalo [0, 1]
float rand_01()
{
    return ((float)rand()) / RAND_MAX;
}

// Simula o lancamento de uma moeda, retornando o valor 0 ou 1
int flip()
{
    if ((((float)rand()) / RAND_MAX) < 0.5)
        return 0;
    else
        return 1;
}

// Gera a solução inicial
// Parâmetros de entrada: Solução, Número de moedas
void gera_sol_inicial(int *solucao, int n_moedas)
{
    for (int i = 0; i < n_moedas; i++) {
        solucao[i] = 0; // Inicialmente, nenhuma moeda é usada
        //sol[i] = random_l_h(0, 10); // Gera uma quantidade aleatória de cada moeda
    }
}


// Escreve a solução no ecrã
// Exibe a solução no formato [qtd_moeda1, qtd_moeda2, ...]
// Parâmetros de entrada: Solução, Valores das moedas, Número de moedas
void escreve_solucao(int *solucao, int *valores_moedas, int n_moedas)
{
    int valor_soma = 0; // Soma acumulada para validar a solução
    int total_moedas = 0; // Contagem do número total de moedas

    printf("[");
    for (int i = 0; i < n_moedas; i++) {
        printf("%d", solucao[i]); // Exibe a quantidade de cada moeda
        if (i < n_moedas - 1)
            printf(", "); // Adiciona separador entre valores
    }
    printf("]");
}


void print_total(int *solucao, double *valor_moedas ,int n_moedas){
    double sum;
    for (int i = 0; i < n_moedas; i++) {
        sum += solucao[i]*valor_moedas[i]; // Exibe a quantidade de cada moeda
    }

    printf("V final: %.2f\n",sum);
}

void substitui(int solucao[], int nova_solucao[], int n_moedas)
{
    for (int i = 0; i < n_moedas; i++) {
        solucao[i] = nova_solucao[i];
    }
}

// Criacao da populacao inicial. O vector e alocado dinamicamente
// Parametro de entrada: Estrutura com parametros
pchrom init_populacao(struct info d)
{
    int i, j;
    pchrom indiv;

    indiv = malloc(sizeof(chrom) * d.popsize);
    if (indiv == NULL)
    {
        printf("Erro na alocacao de memoria\n");
        exit(1);
    }

    for (i = 0; i < d.popsize; i++)
    {
        for (j = 0; j < d.n_Moedas; j++)
        {
            indiv[i].p[j] = random_l_h(0, (int)(d.valor_alvo / d.moedas[j])); // Gera um número aleatório de moedas
        }
        //A pesquisa local pode ser usada para criar as soluções da população inicial;
        if(d.parametro->type == 2)
            trepa_colinas_hibrido(indiv[i].p, d, 100); // Aplica trepa-colinas
    }

    return indiv;
}

// Atualiza a melhor solucao encontrada
// Parametro de entrada: populacao actual, estrutura com parametros e a melhor solucao encontrada ate a geracao imediatamente anterior
// Parametro de saida: a melhor solucao encontrada ate a geracao actual
chrom get_best(pchrom pop, struct info d, chrom best)
{
    int i;

    for (i = 0; i < d.popsize; i++)
    {
        if (best.fitness > pop[i].fitness) // Problema de minimização
            best = pop[i];
    }
    return best;
}

// Escreve uma solucao na consola
// Parametro de entrada: populacao actual e estrutura com parametros
void write_best(chrom x, struct info d)
{
    printf("\nMelhor individuo: %4.1f\n", x.fitness);
    for (int i = 0; i < d.n_Moedas; i++)
        printf("Moeda %d (Valor: %d): %d\n", i, d.moedas[i], x.p[i]);
}


// Leitura do ficheiro de input
// Par�metros de entrada: Nome do ficheiro, nome, N�mero de v�rtices, n, N�mero de itera��es, iter
// Par�metros de sa�da: Matriz de adjac�ncias, p
void ler_instancia(char *nome, int *n, int *v, int **moedas) {
    FILE *f = fopen(nome, "r");
    if (!f) {
        perror("Erro ao abrir o ficheiro");
        exit(1);
    }

    double aux;
    // Lê o número de moedas e o valor-alvo
    fscanf(f, "%d %lf", n, &aux);
    *v = (int)(aux * 100);

    *moedas = malloc(sizeof(double) * (*n));
    if (*moedas == NULL) {
        printf("Erro ao alocar memória\n");
        fclose(f);
        exit(1);
    }

// Lê os valores das moedas
    double *auxiliarMoedas = malloc(sizeof(double) * (*n));
    if (auxiliarMoedas == NULL) {
        printf("Erro ao alocar memória\n");
        fclose(f);
        exit(1);
    }
    for (int i = 0; i < *n; i++) {
        fscanf(f, "%lf", &auxiliarMoedas[i]);
        (*moedas)[i] = (int)(auxiliarMoedas[i] * 100);
    }
    free(auxiliarMoedas);

    fclose(f);
}


