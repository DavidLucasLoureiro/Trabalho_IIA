#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "algoritmo.h"
#include "utils.h"

#define DEFAULT_RUNS 10

int main(int argc, char *argv[])
{
    char    nome_fich[100];
    int     v, diff_moedas, k, runs, custo, best_custo=0;
    int     *grafo, *sol, *best;
	float   mbf = 0.0;

	if(argc == 3)
	{
		runs = atoi(argv[2]);
		strcpy(nome_fich, argv[1]);
	}
	else
        if(argc == 2)
        {
            runs = DEFAULT_RUNS;
            strcpy(nome_fich, argv[1]);
        }
        else
        {
            runs = DEFAULT_RUNS;
            printf("Nome do Ficheiro: ");
            gets(nome_fich);
        }
	if(runs <= 0)
		return 0;
	init_rand();
    // Preenche matriz de adjacencias
    grafo = init_dados(nome_fich, &diff_moedas, &v);
	sol = malloc(sizeof(int)*diff_moedas);
	best = malloc(sizeof(int)*diff_moedas);
	if(sol == NULL || best == NULL)
	{
		printf("Erro na alocacao de memoria");
		exit(1);
	}
	for(k=0; k<runs; k++)
	{
		// Gerar solucao inicial
		gera_sol_inicial(sol, diff_moedas);
		escreve_sol(sol, diff_moedas);
		// Trepa colinas
		custo = recristalizacao(sol, grafo, diff_moedas, diff_moedas,100,5,5);
		// Escreve resultados da repeticao k
		printf("\nRepeticao %d:", k);
		escreve_sol(sol, diff_moedas);
		printf("Custo final: %2d\n", custo);
		mbf += custo;
		if(k==0 || best_custo > custo)
		{
			best_custo = custo;
			substitui(best, sol, vert);
		}
    }
    /*//teste para guardar
    FILE *csv_file = fopen("results.csv", "w");
    if (csv_file == NULL) {
        perror("Error opening file");
        return 1;
    }

    // Write the header row
    fprintf(csv_file, "Best Cost,MBF\n");

    // Write the values
    fprintf(csv_file, "%d,%.2f\n", best_custo, mbf);

    // Close the file
    fclose(csv_file);

    // Write the header row
    fprintf(csv_file, "Best Cost,MBF\n");

    // Write the values
    fprintf(csv_file, "%d,%.2f\n", best_custo, mbf);

    // Close the file
    fclose(csv_file);
    //*/
	// Escreve eresultados globais
	printf("\n\nMBF: %f\n", mbf/k);
	printf("\nMelhor solucao encontrada");
	escreve_sol(best, vert);
	printf("Custo final: %2d\n", best_custo);
	free(grafo);
    free(sol);
	free(best);
    return 0;
}
