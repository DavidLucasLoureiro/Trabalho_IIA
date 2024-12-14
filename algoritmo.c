#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "algoritmo.h"
#include "utils.h"
#include "funcao.h"

// Gera um vizinho à distância (altera a quantidade de uma moeda)
void gera_vizinho(int solucao[], int nova_solucao[], int n_moedas) // solucao atual, nova solucao vizinha gerada, numero de moedas
{
    // Copia a solução atual para a nova solução
    for (int i = 0; i < n_moedas; i++)
        nova_solucao[i] = solucao[i];

    // Seleciona aleatoriamente um índice de moeda
    int p = random_l_h(0, n_moedas - 1);

    // Altera a quantidade da moeda no índice selecionado
    nova_solucao[p] = solucao[p] + random_l_h(-1,1);

    // Garante que a quantidade de moedas não seja negativa
    if (nova_solucao[p] < 0)
        nova_solucao[p] = 0;
}

// Gera um vizinho à distância (altera a quantidade de duas moedas)
void gera_vizinho2(int solucao[], int nova_solucao[], int n_moedas)
{
    int p1, p2;

    // Copia a solução atual para a nova solução
    for (int i = 0; i < n_moedas; i++)
        nova_solucao[i] = solucao[i];

    // Seleciona aleatoriamente a posição de uma moeda
    p1 = random_l_h(0, n_moedas - 1);

    // Altera a quantidade da moeda na posição encontrada
    nova_solucao[p1] = solucao[p1] + random_l_h(-1, 1);

    // Garante que a quantidade de moedas não seja negativa
    if (nova_solucao[p1] < 0)
        nova_solucao[p1] = 0;

    // Seleciona aleatoriamente a posição de outra moeda, diferente da primeira
    do
        p2 = random_l_h(0, n_moedas - 1);
    while(p2 == p1);

    // Altera a quantidade da moeda na nova posição selecionada
    nova_solucao[p2] = solucao[p2] + random_l_h(-1, 1);

    // Garante que a quantidade de moedas não seja negativa
    if (nova_solucao[p2] < 0)
        nova_solucao[p2] = 0;
}

//Função trepa colinas
int trepa_colinas(int solucao[], int valores_moedas[], int n_moedas, int valor_alvo, int num_iter, parameters parametro)
{
    // Aloca memória para armazenar a nova solução gerada
    int *nova_solucao = malloc(sizeof(int) * n_moedas);

    if (nova_solucao == NULL) {
        printf("Erro na alocacao de memoria\n");
        exit(1);
    }

    // Calcula o custo inicial da solução
    int custo = calcula_fit(solucao, valores_moedas, n_moedas, valor_alvo);
    for (int i = 0; i < num_iter; i++) {

        // Gera vizinho baseado no número de vizinhanças configurado
        if(parametro.Args_trepa.n_vizinhancas == 1)
            gera_vizinho(solucao, nova_solucao, n_moedas);
        else
            gera_vizinho2(solucao, nova_solucao, n_moedas);

        // Calcula o custo da nova solução vizinha
        int custo_vizinho = calcula_fit(nova_solucao, valores_moedas, n_moedas, valor_alvo);

        // Verifica se deve aceitar a nova solução
        if(parametro.Args_trepa.custoIgual == 0) {
            if (custo_vizinho < custo) {
                substitui(solucao, nova_solucao, n_moedas);
                custo = custo_vizinho;
            }
        } else {
            if (custo_vizinho <= custo) {
                substitui(solucao, nova_solucao, n_moedas);
                custo = custo_vizinho;
            }
        }
    }

    // Libera a memória alocada para a nova solução
    free(nova_solucao);
    return custo; // Retorna o melhor custo encontrado
}

// Função de torneio simples para seleção
void torneio(pchrom pop, struct info d, pchrom parents)
{
    int x1, x2;

    for (int i = 0; i < d.popsize; i++)
    {
        x1 = random_l_h(0, d.popsize - 1);
        do
            x2 = random_l_h(0, d.popsize - 1);
        while (x1 == x2);

        if (pop[x1].fitness < pop[x2].fitness)
            parents[i] = pop[x1];
        else
            parents[i] = pop[x2];
    }
}

// Função de torneio geral para seleção
void torneio_geral(pchrom pop, struct info d, pchrom parents)
{
    int sair, best, *pos;

    // Aloca memória para armazenar as posições do torneio
    pos = malloc(d.tamTor * sizeof(int));

    for (int i = 0; i < d.popsize; i++)
    {
        best = -1;

        for (int j = 0; j < d.tamTor; j++)
        {
            do {
                pos[j] = random_l_h(0, d.popsize - 1);
                sair = 0;

                // Verifica se a posição já foi selecionada anteriormente
                for (int k = 0; k < j; k++)
                {
                    if (pos[k] == pos[j])
                        sair = 1;
                }
            } while (sair);

            // Guarda a melhor solução válida entre as selecionadas
            if (best == -1 || (pop[pos[j]].valido && pop[pos[j]].fitness > pop[pos[best]].fitness))
                best = j;
        }

        // Se não encontrar nenhum indivíduo válido, seleciona o melhor inválido
        if (best == -1)
        {
            for (int j = 0; j < d.tamTor; j++)
            {
                if (best == -1 || pop[pos[j]].fitness > pop[pos[best]].fitness)
                    best = j;
            }
        }
        parents[i] = pop[pos[best]];
    }
    free(pos);
}

// Operadores genéticos para geração dos filhos
void operadores_geneticos(pchrom parents, struct info d, pchrom offspring)
{
    // Aplica recombinação
    if(d.parametro->Args_evolucao.recombinacao == 1)
        recombinacao_um_ponto_corte(parents, d, offspring);
    else
        recombinacao_dois_pontos_corte(parents, d, offspring);

    // Aplica mutação
    if(d.parametro->Args_evolucao.mutacao == 1)
        mutacao(offspring, d);
    else
        mutacao_troca(offspring, d);
}

// Recombinação com um ponto de corte
void recombinacao_um_ponto_corte(pchrom parents, struct info d, pchrom offspring)
{
    int ponto;

    for (int i = 0; i < d.popsize; i += 2)
    {
        if (rand_01() < d.pr)
        {
            ponto = random_l_h(0, d.n_Moedas - 1);

            // Recombina genes antes e após o ponto de corte
            for (int j = 0; j < ponto; j++)
            {
                offspring[i].p[j] = parents[i].p[j];
                offspring[i + 1].p[j] = parents[i + 1].p[j];
            }
            for (int j = ponto; j < d.n_Moedas; j++)
            {
                offspring[i].p[j] = parents[i + 1].p[j];
                offspring[i + 1].p[j] = parents[i].p[j];
            }
        }
        else
        {
            // Copia diretamente os pais para os descendentes
            offspring[i] = parents[i];
            offspring[i + 1] = parents[i + 1];
        }
    }
}

// Recombinação com dois pontos de corte
void recombinacao_dois_pontos_corte(pchrom parents, struct info d, pchrom offspring)
{
    int ponto1, ponto2;

    for (int i = 0; i < d.popsize; i += 2)
    {
        if (rand_01() < d.pr)
        {
            ponto1 = random_l_h(0, d.n_Moedas - 2);
            ponto2 = random_l_h(ponto1 + 1, d.n_Moedas - 1);

            // Recombina genes nas três regiões
            for (int j = 0; j < ponto1; j++)
            {
                offspring[i].p[j] = parents[i].p[j];
                offspring[i + 1].p[j] = parents[i + 1].p[j];
            }
            for (int j = ponto1; j < ponto2; j++)
            {
                offspring[i].p[j] = parents[i + 1].p[j];
                offspring[i + 1].p[j] = parents[i].p[j];
            }
            for (int j = ponto2; j < d.n_Moedas; j++)
            {
                offspring[i].p[j] = parents[i].p[j];
                offspring[i + 1].p[j] = parents[i + 1].p[j];
            }
        }
        else
        {
            // Copia diretamente os pais para os descendentes
            offspring[i] = parents[i];
            offspring[i + 1] = parents[i + 1];
        }
    }
}

// Mutação binária
void mutacao(pchrom offspring, struct info d)
{
    for (int i = 0; i < d.popsize; i++)
    {
        for (int j = 0; j < d.n_Moedas; j++)
        {
            if (rand_01() < d.pm)
            {
                // Gera um número aleatório de moedas
                offspring[i].p[j] = random_l_h(0, (int)(d.valor_alvo / d.moedas[j]));
            }
        }
    }
}

// Mutação por troca
void mutacao_troca(pchrom offspring, struct info d)
{
    int pos1, pos2, aux;
    for (int i = 0; i < d.popsize; i++)
    {
        if (rand_01() < d.pm)
        {
            int attempts = 0;

            // Encontra uma posição para troca
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

            // Realiza a troca de posições
            if (offspring[i].p[pos1] != 1 && offspring[i].p[pos2] != 0)
            {
                aux = offspring[i].p[pos1];
                offspring[i].p[pos1] = offspring[i].p[pos2];
                offspring[i].p[pos2] = aux;
            }
        }
    }
}

// Subida de colinas híbrida
int trepa_colinas_hibrido(int solucao[], struct info d, int num_iter)
{
    int *nova_solucao, custo, custo_vizinho, valido;

    // Aloca memória para a nova solução
    nova_solucao = malloc(sizeof(int) * d.n_Moedas);
    if (nova_solucao == NULL)
    {
        printf("Erro na alocacao de memoria");
        exit(1);
    }

    // Calcula o custo inicial usando uma função de reparação
    custo = evolutivo_reparado(solucao, d, &valido);

    for (int i = 0; i < num_iter; i++)
    {
        // Gera vizinho com base no número de vizinhanças configurado
        if (d.parametro->Args_trepa.n_vizinhancas == 1)
            gera_vizinho(solucao, nova_solucao, d.n_Moedas);
        else
            gera_vizinho2(solucao, nova_solucao, d.n_Moedas);

        // Calcula o custo da nova solução vizinha
        custo_vizinho = evolutivo_reparado(nova_solucao, d, &valido);

        // Verifica se deve aceitar a nova solução
        if (d.parametro->Args_trepa.custoIgual == 0)
        {
            if (custo_vizinho < custo)
            {
                substitui(solucao, nova_solucao, d.n_Moedas);
                custo = custo_vizinho;
            }
        }
        else
        {
            if (custo_vizinho <= custo)
            {
                substitui(solucao, nova_solucao, d.n_Moedas);
                custo = custo_vizinho;
            }
        }
    }

    // Libera a memória alocada
    free(nova_solucao);
    return custo; // Retorna o melhor custo encontrado
}
