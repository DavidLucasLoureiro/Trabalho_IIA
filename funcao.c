#include "funcao.h"
#include <math.h>
#include <stdio.h>
// Calcula a qualidade de uma solução, agora usando inteiros
// Penaliza soluções inválidas (soma diferente do valor alvo)
double calcula_fit(int a[], double valores_moedas[], int n_moedas, double valor_alvo)
{
    int soma = 0;
    int total_moedas = 0;

    for (int i = 0; i < n_moedas; i++) {
        soma += a[i] * (int)(valores_moedas[i] * 100); // Trabalha com centavos
        total_moedas += a[i];
    }

    // Penalização para soluções inválidas (soma diferente do valor alvo em centavos)
    if (abs(soma - (int)(valor_alvo * 100)) > 0) {
        double penalizacao = abs(soma - (int)(valor_alvo * 100)) * 10; // Penalização proporcional
        return total_moedas + penalizacao;
    }

    return total_moedas; // Retorna o número de moedas para soluções válidas
}
