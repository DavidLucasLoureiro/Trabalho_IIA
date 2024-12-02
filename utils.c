#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "utils.h"

// Lê os dados do ficheiro no formato especificado
// O ficheiro contém o número de moedas (n_moedas), o valor alvo (valor_alvo) e os valores das moedas
double *ler_dados(const char *filename, int *n_moedas, double *valor_alvo)
{
    FILE *file = fopen(filename, "r"); // Abre o ficheiro para leitura
    if (!file)
        return NULL;

    // Lê o número de moedas e o valor alvo
    fscanf(file, "%d %lf", n_moedas, valor_alvo);

    // Aloca memória para o array de valores das moedas
    double *moedas = malloc(sizeof(double) * (*n_moedas));
    if (!moedas) {
        fclose(file);
        return NULL;
    }

    // Lê os valores das moedas
    for (int i = 0; i < *n_moedas; i++)
        fscanf(file, "%lf", &moedas[i]);

    fclose(file); // Fecha o ficheiro
    return moedas; // Retorna o array de valores das moedas
}

// Gera uma solução inicial zerada
// Cada posição do vetor representa a quantidade de uma moeda específica usada
void gera_sol_inicial(int *sol, int n_moedas)
{
    for (int i = 0; i < n_moedas; i++) {
        sol[i] = 0; // Inicialmente, nenhuma moeda é usada
    }
}

// Exibe a solução no formato [qtd_moeda1, qtd_moeda2, ...]
void escreve_sol(int *sol, int n_moedas)
{
    printf("[");
    for (int i = 0; i < n_moedas; i++) {
        printf("%d", sol[i]); // Exibe a quantidade de cada moeda
        if (i < n_moedas - 1)
            printf(", "); // Adiciona separador entre valores
    }
    printf("]");
}

void print_total(int *sol,double *valor_moedas ,int n_moedas){
    double sum;
    for (int i = 0; i < n_moedas; i++) {
        sum += sol[i]*valor_moedas[i]; // Exibe a quantidade de cada moeda
    }

    printf("V final: %.2f\n",sum);
}

// Copia os valores do vetor b para o vetor a (de tamanho n)
void substitui(int a[], int b[], int n)
{
    for (int i = 0; i < n; i++) {
        a[i] = b[i];
    }
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