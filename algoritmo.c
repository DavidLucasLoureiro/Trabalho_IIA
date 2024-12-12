#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "algoritmo.h"
#include "utils.h"
#include "funcao.h"

void gera_vizinho(int solucao[], int nova_solucao[], int n_moedas) //solucao atual, nova solucao vizinha gerada, numero de moedas
{
    for (int i = 0; i < n_moedas; i++)
        nova_solucao[i] = solucao[i]; // Copia a solução atual para a nova solução

    int p = random_l_h(0, n_moedas - 1); // Índice da moeda selecionada aleatoriamente

    nova_solucao[p] = solucao[p] + random_l_h(-1,1); // Altera a quantidade da moeda na posição encontrada de forma controlada

    if (nova_solucao[p] < 0) // Garante que a quantidade de moedas não seja negativa
        nova_solucao[p] = 0;
}

// Gera um vizinho à distância 2 (altera a quantidade de duas moedas)
// Parâmetros de entrada: Solução atual, Solução vizinha, Número de moedas
void gera_vizinho2(int solucao[], int nova_solucao[], int n_moedas)
{
    int p1, p2;

    // Copia a solução atual para a solução vizinha
    for(int i = 0; i < n_moedas; i++)
        nova_solucao[i] = solucao[i];

    // Encontra aleatoriamente a posição de uma moeda
    p1 = random_l_h(0, n_moedas - 1);

    // Altera a quantidade da moeda na posição encontrada de forma controlada
    nova_solucao[p1] = solucao[p1] + random_l_h(-1, 1);

    if (nova_solucao[p1] < 0) // Garante que a quantidade de moedas não seja negativa
        nova_solucao[p1] = 0;

    do // Encontra aleatoriamente a posição de outra moeda, diferente da primeira
        p2 = random_l_h(0, n_moedas - 1);
    while(p2 == p1);

    // Altera a quantidade da moeda na posição encontrada de forma controlada
    nova_solucao[p2] = solucao[p2] + random_l_h(-1, 1);
    if (nova_solucao[p2] < 0) // Garante que a quantidade de moedas não seja negativa
        nova_solucao[p2] = 0;
}

// Parâmetros de entrada: Solução, Valores das moedas, Número de moedas, Valor alvo, Número de iterações
int trepa_colinas(int solucao[], int valores_moedas[], int n_moedas, int valor_alvo, int num_iter, parameters parametro)
{
    int *nova_solucao = malloc(sizeof(int) * n_moedas); // Aloca espaço em memória para guardar a nova solução

    if (nova_solucao == NULL) {
        printf("Erro na alocacao de memoria\n");
        exit(1);
    }

    int custo = calcula_fit(solucao, valores_moedas, n_moedas, valor_alvo); // Calcula o custo inicial

    for (int i = 0; i < num_iter; i++) {

        if(parametro.Args_trepa.n_vizinhancas == 1)
            gera_vizinho(solucao, nova_solucao, n_moedas);
        else
            gera_vizinho2(solucao, nova_solucao, n_moedas);

        gera_vizinho2(solucao, nova_solucao, n_moedas);
        int custo_vizinho = calcula_fit(nova_solucao, valores_moedas, n_moedas, valor_alvo);

        // Fica com a solução vizinha se o custo diminuir
        if(parametro.Args_trepa.custoIgual == 0) {
            if (custo_vizinho < custo) {
                substitui(solucao, nova_solucao, n_moedas);
                custo = custo_vizinho;
            }
            else {
                if (custo_vizinho <= custo){
                    substitui(solucao, nova_solucao, n_moedas);
                    custo = custo_vizinho;
                }
            }
        }
    }

    free(nova_solucao); // Libera a memória alocada para a a nova solução
    return custo; // Retorna o melhor custo encontrado
}

// Preenche uma estrutura com os progenitores da pr�xima gera��o, de acordo com o resultados do torneio binario (tamanho de torneio: 2)
// Par�metros de entrada: popula��o actual (pop), estrutura com par�metros (d) e popula��o de pais a encher
void torneio(pchrom pop, struct info d, pchrom parents)
{
    int i, x1, x2;

    // Realiza popsize torneios
    for (i = 0; i < d.popsize; i++)
    {
        x1 = random_l_h(0, d.popsize - 1);
        do
            x2 = random_l_h(0, d.popsize - 1);
        while (x1 == x2);
        if (pop[x1].fitness < pop[x2].fitness) // Problema de minimização
            parents[i] = pop[x1];
        else
            parents[i] = pop[x2];
    }
}

// Preenche uma estrutura com os progenitores da pr�xima gera��o, de acordo com o resultados do torneio binario (tamanho de torneio: 2)
// Par�metros de entrada: popula��o actual (pop), estrutura com par�metros (d) e popula��o de pais a encher
void torneio_geral(pchrom pop, struct info d, pchrom parents)
{
    int i, j, k, sair, best, *pos;

    pos = malloc(d.tamTor * sizeof(int));
    // Realiza popsize torneios
    for (i = 0; i < d.popsize; i++)
    {
        best = -1;
        // Seleciona tsize soluções diferentes para entrarem em torneio de seleção
        for (j = 0; j < d.tamTor; j++)
        {
            do
            {
                pos[j] = random_l_h(0, d.popsize - 1);
                // Verifica se a nova posição escolhida é igual a alguma das outras posições escolhidas
                sair = 0;
                for (k = 0; k < j; k++)
                {
                    if (pos[k] == pos[j])
                        sair = 1;
                }
            } while (sair);
            // Guarda a posição da melhor solução válida de todas as que entraram em torneio
            if (best == -1 || (pop[pos[j]].valido && pop[pos[j]].fitness > pop[pos[best]].fitness)) // Problema de maximização
                best = j;
        }
        // Se não encontrou nenhum indivíduo válido, seleciona o melhor inválido
        if (best == -1)
        {
            for (j = 0; j < d.tamTor; j++)
            {
                if (best == -1 || pop[pos[j]].fitness > pop[pos[best]].fitness)
                    best = j;
            }
        }
        parents[i] = pop[pos[best]];
    }
    free(pos);
}

// Operadores geneticos a usar na gera��o dos filhos
// Par�metros de entrada: estrutura com os pais (parents), estrutura com par�metros (d), estrutura que guardar� os descendentes (offspring)
void operadores_geneticos(pchrom parents, struct info d, pchrom offspring)
{
    if(d.parametro->Args_evolucao.recombinacao == 1)
        // Recombina��o com um ponto de corte
        crossover(parents, d, offspring);
    else
        // Recombina��o com dois pontos de corte
        recombinacao_dois_pontos_corte(parents, d, offspring);
    if(d.parametro->Args_evolucao.mutacao == 1)
        // Muta��o bin�ria
        mutacao(offspring, d);
    else
        // Muta��o por troca
        mutacao_troca(offspring, d);
}

// Preenche o vector descendentes com o resultado das opera��es de recombina��o
// Par�metros de entrada: estrutura com os pais (parents), estrutura com par�metros (d), estrutura que guardar� os descendentes (offspring)
void crossover(pchrom parents, struct info d, pchrom offspring)
{
    int i, j, point;

    for (i = 0; i < d.popsize; i += 2)
    {
        if (rand_01() < d.pr)
        {
            point = random_l_h(0, d.n_Moedas - 1);
            for (j = 0; j < point; j++)
            {
                offspring[i].p[j] = parents[i].p[j];
                offspring[i + 1].p[j] = parents[i + 1].p[j];
            }
            for (j = point; j < d.n_Moedas; j++)
            {
                offspring[i].p[j] = parents[i + 1].p[j];
                offspring[i + 1].p[j] = parents[i].p[j];
            }
        }
        else
        {
            offspring[i] = parents[i];
            offspring[i + 1] = parents[i + 1];
        }
    }
}

// Preenche o vector descendentes com o resultado da opera��o de recombina��o com dois pontos de corte
// Par�metros de entrada: estrutura com os pais (parents), estrutura com par�metros (d), estrutura que guardar� os descendentes (offspring)
void recombinacao_dois_pontos_corte(pchrom parents, struct info d, pchrom offspring)
{
    int i, j, point1, point2;

    for (i=0; i<d.popsize; i+=2)
    {
        if (rand_01() < d.pr)
        {
            point1 = random_l_h(0, d.n_Moedas-2);
            point2 = random_l_h(point1+1, d.n_Moedas-1);
            for (j=0; j<point1; j++)
            {
                offspring[i].p[j] = parents[i].p[j];
                offspring[i+1].p[j] = parents[i+1].p[j];
            }
            for (j=point1; j<point2; j++)
            {
                offspring[i].p[j]= parents[i+1].p[j];
                offspring[i+1].p[j] = parents[i].p[j];
            }
            for (j=point2; j<d.n_Moedas; j++)
            {
                offspring[i].p[j] = parents[i].p[j];
                offspring[i+1].p[j] = parents[i+1].p[j];
            }
        }
        else
        {
            offspring[i] = parents[i];
            offspring[i+1] = parents[i+1];
        }
    }
}

// Mutação por substituição de um gene por outro valor aleatório.
// Par�metros de entrada: estrutura com os descendentes (offspring) e estrutura com par�metros (d)
void mutacao(pchrom offspring, struct info d)
{
    int i, j;

    for (i = 0; i < d.popsize; i++)
    {
        for (j = 0; j < d.n_Moedas; j++)
        {
            if (rand_01() < d.pm)
            {
                offspring[i].p[j] = random_l_h(0, (int)(d.valor_alvo / d.moedas[j])); // Gera um número aleatório de moedas
            }
        }
    }
}

// Muta��o por troca : troca a quantidade de duas moedas aleatoriamente escolhidas
// Par�metros de entrada: estrutura com os descendentes (offspring) e estrutura com par�metros (d)
void mutacao_troca(pchrom offspring, struct info d)
{
    int i, pos1, pos2, aux;
    for (i = 0; i < d.popsize; i++)
    {
        if (rand_01() < d.pm)
        {
            int attempts = 0;
            do
            {
                pos1 = random_l_h(0, d.n_Moedas - 1);
                attempts++;
            } while (offspring[i].p[pos1] == 1 && attempts < d.n_Moedas);

            attempts = 0;
            do
            {
                pos2 = random_l_h(0, d.n_Moedas - 1);
                attempts++;
            } while (offspring[i].p[pos2] == 0 && attempts < d.n_Moedas);

            if (offspring[i].p[pos1] != 1 && offspring[i].p[pos2] != 0)
            {
                aux = offspring[i].p[pos1];
                offspring[i].p[pos1] = offspring[i].p[pos2];
                offspring[i].p[pos2] = aux;
            }
        }
    }
}

int trepa_colinas_hibrido(int solucao[], struct info d, int num_iter)
{
    int *nova_solucao, custo, custo_viz, i, valido;

    nova_solucao = malloc(sizeof(int) * d.n_Moedas);
    if (nova_solucao == NULL)
    {
        printf("Erro na alocacao de memoria");
        exit(1);
    }

    custo = evolutivo_reparado(solucao, d, &valido);
    for(i = 0; i < num_iter; i++)
    {
        if(d.parametro->Args_trepa.n_vizinhancas == 1)
            gera_vizinho(solucao, nova_solucao, d.n_Moedas);
        else
            gera_vizinho2(solucao, nova_solucao, d.n_Moedas);

        custo_viz = evolutivo_reparado(nova_solucao, d, &valido);
        if(d.parametro->Args_trepa.custoIgual == 0) {
            if (custo_viz < custo) {
                substitui(solucao, nova_solucao, d.n_Moedas);
                custo = custo_viz;
            }
        } else {
            if (custo_viz <= custo)
            {
                substitui(solucao, nova_solucao, d.n_Moedas);
                custo = custo_viz;
            }
        }
    }

    free(nova_solucao);
    return custo;
}