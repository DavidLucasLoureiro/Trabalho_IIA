#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include "funcao.h"
#include "utils.h"

int calcula_fit(int sol[], int valores_moedas[], int n_moedas, int valor_alvo)
{
    int valor_soma = 0;
    int total_moedas = 0;

    for (int i = 0; i < n_moedas; i++) {
        total_moedas += sol[i];
        valor_soma += sol[i] * valores_moedas[i];
    }

    if (valor_soma != valor_alvo) {

        return total_moedas + abs(valor_alvo - valor_soma); // Penaliza soluções inválidas
    }
    else{
        return total_moedas;
    }
}

float evolutivo_penalizado(int solucao[], struct info d, int *v)
{
    int total_moedas = 0;
    int valor_total = 0;

    for (int i = 0; i < d.n_Moedas; i++)
    {
        total_moedas += solucao[i];
        valor_total += solucao[i] * d.moedas[i];
    }

    if (valor_total == d.valor_alvo)
    {
        *v = 1; // Solução válida
        return total_moedas;
    }
    else
    {
        *v = 0; // Solução inválida
        return total_moedas + abs(d.valor_alvo - valor_total); // Penaliza soluções inválidas
    }
}

// Calcula a qualidade de uma solução com reparação aleatória
// Parâmetros de entrada: solução, estrutura com parâmetros, número de objetos válidos (
// Parâmetros de saída: qualidade da solução (se o valor total for excedido, devolve uma penalização)
float evolutivo_reparado(int solucao[], struct info d, int *v)
{
    int total_moedas = 0;
    int valor_total = 0;

    // Calcula o total de moedas e o valor total
    for (int i = 0; i < d.n_Moedas; i++)
    {
        total_moedas += solucao[i];
        valor_total += solucao[i] * d.moedas[i];
    }

    // Processo de reparação
    while (valor_total > d.valor_alvo || valor_total < d.valor_alvo)
    {
        if (valor_total > d.valor_alvo)
        {
            // Escolhe uma moeda aleatoriamente
            int i = random_l_h(0, d.n_Moedas - 1);
            // Se essa moeda estiver na solução, retira-a e ajusta os somatórios do valor total e do número de moedas
            if (solucao[i] > 0)
            {
                solucao[i]--;
                total_moedas--;
                valor_total -= d.moedas[i];
            }
        }
        else
        {
            // Escolhe uma moeda aleatoriamente
            int i = random_l_h(0, d.n_Moedas - 1);
            // Adiciona uma moeda à solução e ajusta os somatórios do valor total e do número de moedas
            solucao[i]++;
            total_moedas++;
            valor_total += d.moedas[i];
        }
    }


    *v = 1; // Solução válida
    return total_moedas;
}

void evolutivo(pchrom pop, struct info d)
{
    for (int i = 0; i < d.popsize; i++)
    {
        if(d.parametro->Args_evolucao.tipo == 1)
            pop[i].fitness = evolutivo_penalizado(pop[i].p, d, &pop[i].valido);
        else
            pop[i].fitness = evolutivo_reparado(pop[i].p, d, &pop[i].valido);
    }
}

