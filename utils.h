struct info ler_dados(char *filename, int mat[][2]);

void ler_instancia(char *nome, int *n, int *v, int **moedas);

void init_rand();

int random_l_h(int min, int max);

float rand_01();

void gera_sol_inicial(int *solucao, int n_moedas);

void escreve_solucao(int *solucao, int *valores_moedas, int n_moedas);

void substitui(int solucao[], int nova_solucao[], int n_moedas);

pchrom init_populacao(struct info d);

chrom get_best(pchrom pop, struct info d, chrom best);

void write_best(chrom x, struct info d);



