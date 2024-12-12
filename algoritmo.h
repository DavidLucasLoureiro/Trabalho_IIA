#ifndef ALGORITMO_H
#define ALGORITMO_H
#define MAX_OBJ 1000		// Numero maximo de objetos



typedef struct {
    int n_Interacoes;		    // Número de iterações
    int n_vizinhancas;	        // Número de vizinhanças
    int custoIgual;		        // Permitir custo igual
} ARGS_trepa;

typedef struct {
    int pop;		            // Tamanho da população
    int recombinacao;	        // Metodo de recombinacao
    float probRecombinacao;	    // Probabilidade de recombinação
    int mutacao;                // Metodo de mutação
    float probMutacao;		    // Probabilidade de mutação
    int selecao;                // Metodo de seleção
    int tipo;		            // Tipo (reparação ou penalização)

} ARGS_evolucao;

typedef struct {
    //TYPES
    // 0 - trepa colinas
    // 1 - algoritmo evolutivo
    // 2 - Hibrido, pesquisa local é usada para criar as soluções da população inicial
    // 3 - Hibrido, pesquisa local é usada para refinar as soluções da última população
    int type;
    ARGS_trepa Args_trepa;
    ARGS_evolucao Args_evolucao;
} parameters;

// EStrutura para armazenar parametros
struct info
{
    int popsize;                // Tamanho da população
    float pm;                   // Probabilidade de mutação
    float pr;                   // Probabilidade de recombinação
    int tamTor;                 // Tamanho do torneio para seleção do pai da próxima geração
    float ro;                   // Constante para avaliação com penalização
    int n_Moedas;               // Número de moedas
    int n_Geracoes;             // Número de gerações
    int valor_alvo;             // Valor desejado
    int moedas[MAX_OBJ];        // Valores das moedas
    parameters *parametro;
};

// Individuo (solucao)
typedef struct individual chrom, *pchrom;

struct individual
{
    // Solução (número de moedas de cada tipo)
    int p[MAX_OBJ];

    // Valor da qualidade da solução
    float fitness;

    // 1 se for uma solução válida e 0 se não for
    int valido;
};

void gera_vizinho(int solucao[], int nova_solucao[], int n_moedas);

void gera_vizinho2(int solucao[], int nova_solucao[], int n_moedas);

int trepa_colinas(int sol[], int valores_moedas[], int n_moedas, int valor_alvo, int num_iter, parameters parametro);

void torneio(pchrom pop, struct info d, pchrom parents);

void torneio_geral(pchrom pop, struct info d, pchrom parents);

void operadores_geneticos(pchrom parents, struct info d, pchrom offspring);

void crossover(pchrom parents, struct info d, pchrom offspring);

void recombinacao_dois_pontos_corte(pchrom parents, struct info d, pchrom offspring);

void mutacao(pchrom offspring,struct info d);

void mutacao_troca(pchrom offspring, struct info d);

int trepa_colinas_hibrido(int solucao[], struct info d, int num_iter);

#endif // ALGORITMO_H