#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "algoritmo.h"
#include "utils.h"

#define DEFAULT_RUNS 10

int main(int argc, char *argv[])
{
    char nome_fich[100]; // Nome do ficheiro de entrada

    int n_moedas, n_repeticoes, cntReps; // Número de moedas, número de repetições e contador de repetições

    double valor_alvo, custo, melhor_custo, mbf = 0.0; // Valor a alcançar, custo atual, melhor custo e média de custos (MBF)

    double *valores_moedas; // Array que armazena os valores das moedas

    int *solucao, *melhor_solucao; // Arrays para a solução atual e a melhor solução encontrada

    // Processa argumentos da linha de comando ou solicita o nome do ficheiro
    if (argc >= 2) {
        strcpy(nome_fich, argv[1]); // Copia o nome do ficheiro fornecido como argumento
        n_repeticoes = (argc == 3) ? atoi(argv[2]) : DEFAULT_RUNS;
    } else {
        printf("Nome do Ficheiro: ");
        gets(nome_fich); // Lê o nome do ficheiro via entrada padrão
        n_repeticoes = DEFAULT_RUNS; // Usa o número padrão de repetições
    }

    if (n_repeticoes <= 0) // Valida o número de repetições
        return 0;

    init_rand(); // Inicializa o gerador de números aleatórios

    // Lê os dados do ficheiro: número de moedas, valor alvo, e valores das moedas
    valores_moedas = ler_dados(nome_fich, &n_moedas, &valor_alvo);
    if (!valores_moedas) {
        printf("Erro ao carregar os dados do ficheiro\n");
        return 1;
    }

    // Aloca memória para os vetores de solução
    solucao = malloc(sizeof(int) * n_moedas); // Solução atual
    melhor_solucao = malloc(sizeof(int) * n_moedas); // Melhor solução encontrada
    if (solucao == NULL || melhor_solucao == NULL) {
        printf("Erro na alocacao de memoria\n");
        free(valores_moedas);
        return 1;
    }

    // Inicializa o melhor custo com um valor muito alto
    melhor_custo = 1e9;

    // Parâmetros do Algoritmo Evolutivo
    int tam_pop = 50;         // Tamanho da população
    int geracoes = 100;       // Número de gerações
    float taxa_mut = 0.2;     // Taxa de mutação (20%)
    float taxa_cross = 0.7;   // Taxa de crossover (70%)

    // Executa o algoritmo para o número de repetições definido
    for (cntReps = 0; cntReps < 20; cntReps++) {
        gera_sol_inicial(solucao, n_moedas); // Gera uma solução inicial aleatória ou zerada
        //custo = trepa_colinas(solucao, valores_moedas, n_moedas, valor_alvo, 50000); // Executa o algoritmo de Trepa-Colinas
        custo = algoritmo_evolutivo(solucao, valores_moedas, n_moedas, valor_alvo, tam_pop, geracoes, taxa_mut, taxa_cross); //Executa o algoritmo de Evolutivo
        //custo = algoritmo_hibrido(solucao, valores_moedas, n_moedas, valor_alvo, tam_pop, geracoes, taxa_mut, taxa_cross);  //Executa o algoritmo de Algoritmo-Hibrido

        // Exibe a solução e o custo para esta repetição
        printf("\nRepeticao %d: Solucao: ", cntReps);
        escreve_sol(solucao, n_moedas);
        printf("\n Custo final: %.2f\n", custo);
        print_total(melhor_solucao,valores_moedas , n_moedas);

        // Atualiza o MBF e verifica se a solução é a melhor encontrada até o momento
        mbf += custo;
        if (cntReps == 0 || custo < melhor_custo) {
            melhor_custo = custo;
            substitui(melhor_solucao, solucao, n_moedas); // Substitui a melhor solução pela atual
        }
    }

    // Exibe os resultados
    printf("\n\nMBF: %.2f\n", mbf / n_repeticoes);
    //printf("\nMelhor solucao encontrada (Pesquisa Local):");
    printf("\nMelhor solucao encontrada (Evolutivo):");
    //printf("\nMelhor solucao encontrada (Hibrido):");
    escreve_sol(melhor_solucao, n_moedas);
    //printf("\nCusto final (Pesquisa Local): %.2f\n", melhor_custo);
    printf("\nCusto final (Evolutivo): %.2f\n", melhor_custo);
    //printf("\nCusto final (Hibrido): %.2f\n", melhor_custo);
    printf("\nValor alvo: %.2f\n", valor_alvo);
    print_total(melhor_solucao, valores_moedas, n_moedas);

    // Libera a memória alocada
    free(valores_moedas);
    free(solucao);
    free(melhor_solucao);
    return 0;
}