#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include "funcao.h"
#include "utils.h"

// Função que calcula o fitness de uma solução
int calcula_fit(int sol[], int valores_moedas[], int n_moedas, int valor_alvo)
{
    int valor_soma = 0;  // Soma total em valor monetário
    int total_moedas = 0; // Soma total de moedas utilizadas
    // Itera sobre cada moeda, calculando a soma total e o número total de moedas
    for (int i = 0; i < n_moedas; i++) {
        total_moedas += sol[i]; // Conta o número de moedas usadas
        valor_soma += sol[i] * valores_moedas[i]; // Calcula o valor monetário correspondente
    }

    // Verifica se o valor total obtido é diferente do valor alvo
    if (valor_soma != valor_alvo) {
        // Penaliza a solução inválida, adicionando a diferença absoluta ao número de moedas
        return total_moedas + abs(valor_alvo - valor_soma);
    } else {
        // Retorna apenas o número total de moedas para soluções válidas
        return total_moedas;
    }
}

// Função que calcula o fitness utilizando penalização para soluções inválidas
float evolutivo_penalizado(int solucao[], struct info d, int *v)
{
    int valor_soma = 0;  // Soma total em valor monetário
    int total_moedas = 0; // Soma total de moedas utilizadas

    // Itera sobre cada moeda, calculando a soma total e o número total de moedas
    for (int i = 0; i < d.n_Moedas; i++)
    {
        total_moedas += solucao[i]; // Conta o número de moedas usadas
        valor_soma += solucao[i] * d.moedas[i]; // Calcula o valor monetário correspondente
    }

    // Verifica se a solução é válida (valor soma igual ao valor alvo)
    if (valor_soma == d.valor_alvo)
    {
        *v = 1; // Marca a solução como válida
        return total_moedas; // Retorna o número total de moedas (menor é melhor)
    }
    else
    {
        *v = 0; // Marca a solução como inválida
        // Penaliza soluções inválidas somando a diferença absoluta ao número de moedas
        return total_moedas + abs(d.valor_alvo - valor_soma);
    }
}

// Função que calcula o fitness utilizando reparação para ajustar soluções inválidas
float evolutivo_reparado(int solucao[], struct info d, int *v)
{
    int valor_soma = 0;  // Soma total em valor monetário
    int total_moedas = 0; // Soma total de moedas utilizadas

    // Calcula o total de moedas e o valor total inicial
    for (int i = 0; i < d.n_Moedas; i++)
    {
        total_moedas += solucao[i]; // Conta o número de moedas usadas
        valor_soma += solucao[i] * d.moedas[i]; // Calcula o valor monetário correspondente
    }

    // Enquanto o valor total for maior ou menor que o valor alvo, tenta reparar
    while (valor_soma > d.valor_alvo || valor_soma < d.valor_alvo)
    {
        if (valor_soma > d.valor_alvo) // Se o valor total for maior que o alvo
        {
            // Escolhe uma moeda aleatória
            int i = random_l_h(0, d.n_Moedas - 1);
            // Remove uma moeda, se ela estiver presente na solução
            if (solucao[i] > 0)
            {
                solucao[i]--; // Remove uma unidade desta moeda
                total_moedas--; // Reduz o total de moedas
                valor_soma -= d.moedas[i]; // Reduz o valor total
            }
        }
        else // Se o valor total for menor que o alvo
        {
            // Escolhe uma moeda aleatória
            int i = random_l_h(0, d.n_Moedas - 1);
            // Adiciona uma moeda à solução
            solucao[i]++;
            total_moedas++; // Incrementa o total de moedas
            valor_soma += d.moedas[i]; // Incrementa o valor total
        }
    }

    *v = 1; // Marca a solução como válida após a reparação
    return total_moedas; // Retorna o número total de moedas (menor é melhor)
}

// Função que avalia a população evolutiva
void evolutivo(pchrom pop, struct info d)
{
    // Avalia cada indivíduo da população
    for (int i = 0; i < d.popsize; i++)
    {
        // Se o tipo de avaliação for penalizado
        if(d.parametro->Args_evolucao.tipo == 1)
            pop[i].fitness = evolutivo_penalizado(pop[i].p, d, &pop[i].valido);
        else // Caso contrário, usa reparação para ajustar soluções inválidas
            pop[i].fitness = evolutivo_reparado(pop[i].p, d, &pop[i].valido);
    }
}
