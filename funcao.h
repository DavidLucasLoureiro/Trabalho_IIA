#include "algoritmo.h"

int calcula_fit(int solucao[], int valores_moedas[], int n_moedas, int valor_alvo);

float evolutivo_penalizado(int solucao[], struct info d, int *v);

float evolutivo_reparado(int solucao[], struct info d, int *v);

void evolutivo(pchrom pop, struct info d);
