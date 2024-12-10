#include <stdio.h>
#include <stdlib.h>
#include "algoritmo.h"
#include "utils.h"
#include "funcao.h"

// Gera uma solução vizinha a partir da solução atual
void gera_vizinho(int solucao[], int nova_solucao[], int n_moedas) //solucao atual, nova solucao vizinha gerada, numero de moedas
{
    for (int i = 0; i < n_moedas; i++)
        nova_solucao[i] = solucao[i]; // Copia a solução atual para a nova solução

    int p = random_l_h(0, n_moedas - 1); // Índice da moeda selecionada aleatoriamente

    // Adiciona ou remove moedas aleatoriamente
    if (rand_01() < 0.5 && nova_solucao[p] > 0) {
        nova_solucao[p]--; // Remove uma moeda, se possível
    } else {
        nova_solucao[p]++; // Adiciona uma moeda
    }

    // Garante que a quantidade de moedas não fique negativa
    if (nova_solucao[p] < 0) nova_solucao[p] = 0;
}

// Implementa o algoritmo de Trepa-Colinas
double trepa_colinas(int solucao[], double valores_moedas[], int n_moedas, double valor_alvo, int num_iter)
{
    int *nova_solucao = malloc(sizeof(int) * n_moedas); // Aloca espaço para a solução vizinha

    if (nova_solucao == NULL) {
        printf("Erro na alocacao de memoria\n");
        exit(1);
    }

    double custo = calcula_fit(solucao, valores_moedas, n_moedas, valor_alvo); // Calcula o custo inicial

    for (int i = 0; i < num_iter; i++) {
        gera_vizinho(solucao, nova_solucao, n_moedas); // Gera uma solução vizinha
        double novo_custo = calcula_fit(nova_solucao, valores_moedas, n_moedas, valor_alvo); // Avalia o custo da solução vizinha

        // Aceita a solução vizinha se for melhor
        if (novo_custo < custo) {
            substitui(solucao, nova_solucao, n_moedas); // Substitui a solução atual pela vizinha
            custo = novo_custo;
        }
    }

    free(nova_solucao); // Libera a memória alocada para a solução vizinha
    return custo; // Retorna o melhor custo encontrado
}

double algoritmo_evolutivo(double valores_moedas[], int n_moedas, double valor_alvo, int tam_pop, int geracoes, float taxa_mut, float taxa_cross) {
    // Inicializar população
    int **populacao = malloc(tam_pop * sizeof(int *));
    for (int i = 0; i < tam_pop; i++) {
        populacao[i] = malloc(n_moedas * sizeof(int));
        gera_sol_inicial(populacao[i], n_moedas);
    }

    double *fitness = malloc(tam_pop * sizeof(double));
    for (int i = 0; i < tam_pop; i++) {
        fitness[i] = calcula_fit(populacao[i], valores_moedas, n_moedas, valor_alvo);
    }

    int *melhor_solucao = malloc(n_moedas * sizeof(int));
    double melhor_custo = 1e9;

    // Iterar por gerações
    for (int g = 0; g < geracoes; g++) {
        // Avaliação e atualização da melhor solução
        for (int i = 0; i < tam_pop; i++) {
            if (fitness[i] < melhor_custo) {
                melhor_custo = fitness[i];
                substitui(melhor_solucao, populacao[i], n_moedas);
            }
        }

        // Nova geração
        int **nova_populacao = malloc(tam_pop * sizeof(int *));
        for (int i = 0; i < tam_pop; i++) {
            nova_populacao[i] = malloc(n_moedas * sizeof(int));
            int pai1 = selecao_torneio(populacao, fitness, tam_pop);
            int pai2 = selecao_torneio(populacao, fitness, tam_pop);

            if (rand_01() < taxa_cross) {
                crossover_um_ponto(populacao[pai1], populacao[pai2], nova_populacao[i], n_moedas);
            } else {
                substitui(nova_populacao[i], populacao[pai1], n_moedas);
            }

            if (rand_01() < taxa_mut) {
                mutacao_aleatoria(nova_populacao[i], n_moedas);
            }
        }

        // Substituir a população antiga
        for (int i = 0; i < tam_pop; i++) {
            free(populacao[i]);
        }
        free(populacao);
        populacao = nova_populacao;

        // Recalcular fitness
        for (int i = 0; i < tam_pop; i++) {
            fitness[i] = calcula_fit(populacao[i], valores_moedas, n_moedas, valor_alvo);
        }
    }

    // Liberação de memória
    for (int i = 0; i < tam_pop; i++) {
        free(populacao[i]);
    }
    free(populacao);
    free(fitness);

    double melhor_custo_final = melhor_custo;
    free(melhor_solucao);
    return melhor_custo_final;
}