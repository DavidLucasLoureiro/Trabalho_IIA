double *ler_dados(const char *filename, int *n_moedas, double *valor_alvo);
void gera_sol_inicial(int *sol, int v);
void escreve_sol(int *sol, int vert);
void substitui(int a[], int b[], int n);
void init_rand(void);
int random_l_h(int min, int max);
float rand_01(void);
void print_total(int *sol,double *valor_moedas ,int n_moedas);
