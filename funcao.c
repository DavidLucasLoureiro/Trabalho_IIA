#include "funcao.h"
#include <math.h>

// Calcula a qualidade de uma solução
// Penaliza soluções inválidas (soma diferente do valor alvo)
double calcula_fit(int a[], double valores_moedas[], int n_moedas, double valor_alvo)
{
    double soma = 0.0;
    int total_moedas = 0;

    // Calcula a soma total e o número de moedas usadas
    for (int i = 0; i < n_moedas; i++) {
        soma += a[i] * valores_moedas[i];
        total_moedas += a[i];
    }

    // Penaliza soluções inválidas (soma diferente do valor alvo)
    if (fabs(soma - valor_alvo) > 1e-6) {
        // Reduz a penalização proporcional ao valor alvo
        double penalizacao = fabs(soma - valor_alvo) * 10;
        return total_moedas + penalizacao;
    }

    // Retorna o número de moedas para soluções válidas
    return total_moedas;
}