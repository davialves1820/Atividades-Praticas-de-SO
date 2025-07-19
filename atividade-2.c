// threads_3.c
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <time.h>
#include <math.h>

#define N 10000

int numeros[N];
double media = 0.0, mediana = 0.0, desvio_padrao = 0.0;

// Função para gerar os 10.000 números aleatórios
void gerar_numeros() {
    srand(time(NULL));
    for (int i = 0; i < N; i++) {
        numeros[i] = rand() % 101; // entre 0 e 100
    }
}

// Função para comparar inteiros (usada em qsort)
int comparar(const void *a, const void *b) {
    return (*(int *)a - *(int *)b);
}

// Função da thread que calcula a média
void* calcular_media(void *arg) {
    double soma = 0.0;
    for (int i = 0; i < N; i++) {
        soma += numeros[i];
    }
    media = soma / N;
    pthread_exit(NULL);
}

// Função da thread que calcula a mediana
void* calcular_mediana(void *arg) {
    int *copia = malloc(N * sizeof(int));
    for (int i = 0; i < N; i++) copia[i] = numeros[i];
    qsort(copia, N, sizeof(int), comparar);
    if (N % 2 == 0)
        mediana = (copia[N/2 - 1] + copia[N/2]) / 2.0;
    else
        mediana = copia[N/2];
    free(copia);
    pthread_exit(NULL);
}

// Função da thread que calcula o desvio padrão
void* calcular_desvio(void *arg) {
    double soma = 0.0;
    for (int i = 0; i < N; i++) {
        soma += numeros[i];
    }
    double media_local = soma / N;
    double soma_quadrados = 0.0;
    for (int i = 0; i < N; i++) {
        soma_quadrados += pow(numeros[i] - media_local, 2);
    }
    desvio_padrao = sqrt(soma_quadrados / N);
    pthread_exit(NULL);
}

int main() {
    gerar_numeros();

    pthread_t t1, t2, t3;

    clock_t inicio = clock();

    // Criação das threads
    pthread_create(&t1, NULL, calcular_media, NULL);
    pthread_create(&t2, NULL, calcular_mediana, NULL);
    pthread_create(&t3, NULL, calcular_desvio, NULL);

    // Espera as threads terminarem
    pthread_join(t1, NULL);
    pthread_join(t2, NULL);
    pthread_join(t3, NULL);

    clock_t fim = clock();

    double tempo_total = (double)(fim - inicio) * 1000.0 / CLOCKS_PER_SEC;

    printf("Resultados com 3 threads:\n");
    printf("Média: %.2f\n", media);
    printf("Mediana: %.2f\n", mediana);
    printf("Desvio padrão: %.2f\n", desvio_padrao);
    printf("Tempo total (ms): %.2f\n", tempo_total);

    return 0;
}
