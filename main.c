#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "algoritmo.h"
#include "funcao.h"
#include "utils.h"

#define DEFAULT_RUNS 10

parameters parametro;

int main(int argc, char *argv[])
{
    char nome_fich[100];
    int runs;

    // Define o tipo do algoritmo a ser usado
    // 0 - trepa colinas
    // 1 - algoritmo evolutivo
    // 2 - Hibrido, pesquisa local é usada para criar as soluções da população inicial
    // 3 - Hibrido, pesquisa local é usada para refinar as soluções da última população
    parametro.type = 0;


    // Configuração do trepa-colinas
    parametro.Args_trepa.n_Interacoes = 5000;     // Número de iterações

    // 1 - usa geravizinho1
    // 2 - usa geravizinho2
    parametro.Args_trepa.n_vizinhancas = 2;       // Método de geração de vizinhos

    // 0 - não permite custo igual
    // 1 - permite custo igual
    parametro.Args_trepa.custoIgual = 0;        // Permite custo igual


    // Configuração do algoritmo evolutivo
    parametro.Args_evolucao.pop = 100;            // Tamanho da população

    // 1 - Recombinação com um ponto de corte
    // 2 - Recombinação com dois pontos de corte
    parametro.Args_evolucao.recombinacao = 1;         // Método de recombinação
    parametro.Args_evolucao.probRecombinacao = 0.5;   // Probabilidade de recombinação

    // 1 - Mutaçao binária
    // 2 - Mutaçao por troca
    parametro.Args_evolucao.mutacao = 1;         // Método de mutação
    parametro.Args_evolucao.probMutacao = 0.05;  // Probabilidade de mutação

    // 1 - Seleção por torneio
    // 2 - Seleção por torneio generalizado
    parametro.Args_evolucao.selecao = 2;

    // 1 - Penalização
    // 2 - Reparação
    parametro.Args_evolucao.tipo = 2;        // Tipo (reparação ou penalização)

    // Processamento dos argumentos da linha de comando
    if (argc == 3) {
        runs = atoi(argv[2]);
        strcpy(nome_fich, argv[1]);
    } else if (argc == 2) {
        runs = DEFAULT_RUNS;
        strcpy(nome_fich, argv[1]);
    } else {
        runs = DEFAULT_RUNS;
        printf("Nome do Ficheiro: ");
        gets(nome_fich);
    }

    if (runs <= 0)
        return 0;

    if (parametro.type >= 1) {
        struct info EA_param;
        pchrom pop = NULL, parents = NULL;
        chrom best_run, best_ever;
        int gen_actual, r, i, inv;
        float mbf = 0.0;

        // Inicializa os dados do problema
        init_rand();
        EA_param = ler_dados(nome_fich, NULL);
        EA_param.parametro = &parametro;
        EA_param.popsize = parametro.Args_evolucao.pop;
        EA_param.pm = parametro.Args_evolucao.probMutacao;
        EA_param.pr = parametro.Args_evolucao.probRecombinacao;
        EA_param.tamTor = 50;

        for (r = 0; r < runs; r++) {
            printf("\nRepeticao %d:\n", r + 1);
            pop = init_populacao(EA_param);
            evolutivo(pop, EA_param);
            gen_actual = 1;
            best_run = pop[0];
            best_run = get_best(pop, EA_param, best_run);
            parents = malloc(sizeof(chrom) * EA_param.popsize);
            if (parents == NULL) {
                printf("Erro na alocacao de memoria\n");
                exit(1);
            }

            while (gen_actual <= EA_param.n_Geracoes) {
                if (parametro.Args_evolucao.selecao == 1)
                    torneio(pop, EA_param, parents);
                else
                    torneio_geral(pop, EA_param, parents);
                operadores_geneticos(parents, EA_param, pop);
                evolutivo(pop, EA_param);
                best_run = get_best(pop, EA_param, best_run);
                gen_actual++;
            }

            if (parametro.type == 3)
                trepa_colinas_hibrido(best_run.p, EA_param, 100); // Refina a melhor solução com trepa-colinas

            for (inv = 0, i = 0; i < EA_param.popsize; i++)
                if (pop[i].valido == 0)
                    inv++;

            write_best(best_run, EA_param);
            printf("\nPercentagem Invalidos: %f\n", 100 * (float)inv / EA_param.popsize);
            mbf += best_run.fitness;
            if (r == 0 || best_run.fitness < best_ever.fitness)
                best_ever = best_run;

            free(parents);
            free(pop);
        }

        printf("\n\nMBF: %f\n", mbf / runs);
        printf("\nMelhor solucao encontrada\n");
        write_best(best_ever, EA_param);

    } else {
        int *moedas;
        int V;
        int n;

        ler_instancia(nome_fich, &n, &V, &moedas);

        int num_iter = parametro.Args_trepa.n_Interacoes;
        int *sol = malloc(sizeof(int) * n);
        int *best = malloc(sizeof(int) * n);
        int custo, melhor_custo;
        float mbf = 0.0;

        if (sol == NULL || best == NULL) {
            printf("Erro na alocacao de memoria\n");
            exit(1);
        }

        for (int k = 0; k < runs; k++) {
            gera_sol_inicial(sol, n);
            custo = trepa_colinas(sol, moedas, n, V, num_iter, parametro);

            printf("\nRepeticao %d:", k);
            escreve_solucao(sol, moedas, n);
            printf("Custo final: %2d\n", custo);
            mbf += custo;

            if (k == 0 || melhor_custo > custo) {
                melhor_custo = custo;
                substitui(best, sol, n);
            }
        }

        printf("\n\nMBF: %f\n", mbf / runs);
        printf("\nMelhor solucao encontrada\n");
        for (int i = 0; i < n; i++) {
            printf("Moeda %d: %d\n", moedas[i], best[i]);
        }
        printf("Custo final: %d\n", melhor_custo);
        free(sol);
        free(best);
    }
    return 0;
}
