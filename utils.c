#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "algoritmo.h"
#include "utils.h"

// Função que lê os dados de entrada de um ficheiro
struct info ler_dados(char *filename, int mat[][2])
{
    struct info x; // Estrutura que armazenará os dados do problema
    FILE *f; // Ponteiro para o ficheiro
    int i; // Variável de controlo para o ciclo
    float V_aux; // Variável auxiliar para armazenar o valor alvo
    float moedas_aux[MAX_OBJ]; // Array auxiliar para armazenar os valores das moedas

    // Abre o ficheiro para leitura
    f = fopen(filename, "rt");
    if (!f) // Verifica se o ficheiro foi aberto com sucesso
    {
        printf("Ficheiro não encontrado\n");
        exit(1); // Encerra o programa em caso de erro
    }

    // Lê o número de moedas e o valor alvo do ficheiro
    fscanf(f, "%d %f", &x.n_Moedas, &V_aux);
    x.valor_alvo = (int)(V_aux * 100); // Converte o valor para cêntimos e armazena como inteiro

    // Lê os valores de cada moeda e converte-os para cêntimos
    for (i = 0; i < x.n_Moedas; i++)
    {
        fscanf(f, "%f", &moedas_aux[i]);
        x.moedas[i] = (int)(moedas_aux[i] * 100);
    }

    // Imprime os dados lidos para verificação
    printf("\n");
    printf("Numero de moedas: %d\n", x.n_Moedas);
    printf("Valor desejado: %d (em centimos)\n", x.valor_alvo);
    printf("Moedas: ");
    for (i = 0; i < x.n_Moedas; i++)
        printf("%d ", x.moedas[i]);
    printf("\n");

    // Inicializa parâmetros adicionais do problema
    x.popsize = 100;                // Tamanho da população
    x.pm = 0.01;                    // Probabilidade de mutação
    x.pr = 0.7;                     // Probabilidade de recombinação
    x.tamTor = 2;                   // Tamanho do torneio
    x.n_Geracoes = 5000;            // Número de gerações
    x.ro = 0.0;                     // Constante para avaliação com penalização

    fclose(f); // Fecha o ficheiro
    return x; // Retorna a estrutura
}

// Função para ler os dados de uma instância específica
void ler_instancia(char *nome, int *n, int *v, int **moedas) {
    FILE *f = fopen(nome, "r");
    if (!f) { // Verifica se o ficheiro foi aberto com sucesso
        perror("Erro ao abrir o ficheiro");
        exit(1);
    }

    double aux; // Variável auxiliar para leitura do valor alvo

    // Lê o número de moedas e o valor alvo
    fscanf(f, "%d %lf", n, &aux);
    *v = (int)(aux * 100); // Converte o valor para cêntimos

    *moedas = malloc(sizeof(double) * (*n)); // Aloca memória para armazenar os valores das moedas
    if (*moedas == NULL) { // Verifica se a alocação foi bem-sucedida
        printf("Erro ao alocar memoria\n");
        fclose(f);
        exit(1);
    }

    // Lê os valores das moedas e converte-os para cêntimos
    double *auxiliarMoedas = malloc(sizeof(double) * (*n));
    if (auxiliarMoedas == NULL) { // Verifica se a alocação foi bem-sucedida
        printf("Erro ao alocar memoria\n");
        fclose(f);
        exit(1);
    }
    for (int i = 0; i < *n; i++) {
        fscanf(f, "%lf", &auxiliarMoedas[i]);
        (*moedas)[i] = (int)(auxiliarMoedas[i] * 100);
    }
    free(auxiliarMoedas); // Liberta a memória auxiliar
    fclose(f); // Fecha o ficheiro
}

// Inicializa o gerador de números aleatórios com base no tempo atual
void init_rand()
{
    srand((unsigned)time(NULL)); // Define a semente para a geração de números aleatórios
}

// Gera um número inteiro aleatório entre min e max (inclusive)
int random_l_h(int min, int max)
{
    return min + rand() % (max - min + 1);
}

// Gera um número real aleatório no intervalo [0, 1]
float rand_01()
{
    return ((float)rand()) / RAND_MAX;
}

// Gera uma solução inicial com zero moedas utilizadas
void gera_sol_inicial(int *solucao, int n_moedas)
{
    for (int i = 0; i < n_moedas; i++) {
        solucao[i] = 0; // Inicialmente, nenhuma moeda é usada
    }
}

// Escreve a solução atual (quantidade de cada moeda) no formato de lista
void escreve_solucao(int *solucao, int *valores_moedas, int n_moedas)
{
    printf("[");
    for (int i = 0; i < n_moedas; i++) {
        printf("%d", solucao[i]); // Exibe a quantidade de cada moeda
        if (i < n_moedas - 1)
            printf(", "); // Adiciona separador entre valores
    }
    printf("]");
}

// Substitui uma solução por outra
void substitui(int solucao[], int nova_solucao[], int n_moedas)
{
    for (int i = 0; i < n_moedas; i++) {
        solucao[i] = nova_solucao[i];
    }
}

// Inicializa uma população de cromossomas
pchrom init_populacao(struct info d)
{
    int i, j;
    pchrom indiv = malloc(sizeof(chrom) * d.popsize);
    if (indiv == NULL) {
        printf("Erro na alocação de memoria\n");
        exit(1);
    }

    for (i = 0; i < d.popsize; i++) {
        for (j = 0; j < d.n_Moedas; j++) {
            // Gera uma quantidade aleatória para cada moeda
            indiv[i].p[j] = random_l_h(0, (int)(d.valor_alvo / d.moedas[j]));
        }

        // Aplica pesquisa local se o tipo de algoritmo for híbrido
        if(d.parametro->type == 2)
            trepa_colinas_hibrido(indiv[i].p, d, 100);
    }

    return indiv; // Retorna a população inicializada
}

// Retorna o melhor indivíduo de uma população
chrom get_best(pchrom pop, struct info d, chrom best)
{
    for (int i = 0; i < d.popsize; i++) {
        if (best.fitness > pop[i].fitness) // Critério de minimização
            best = pop[i];
    }
    return best;
}

// Escreve as informações do melhor indivíduo encontrado
void write_best(chrom x, struct info d)
{
    printf("\nMelhor individuo: %4.1f\n", x.fitness);
    for (int i = 0; i < d.n_Moedas; i++) {
        printf("Moeda %d (Valor: %d): %d\n", i, d.moedas[i], x.p[i]);
    }
}