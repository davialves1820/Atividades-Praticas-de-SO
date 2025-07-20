// Inclusão das bibliotecas necessárias
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <math.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/time.h>

// Tamanho do vetor de dados
#define N 10000

// Vetor de dados e variáveis globais para armazenar os resultados
int dados[N];
double media = 0.0, mediana = 0.0, desvio_padrao = 0.0;

// Função de comparação usada no qsort para ordenar inteiros
int cmpfunc(const void* a, const void* b) {
    return (*(int*)a - *(int*)b);
}

// Função executada por uma thread para calcular a média
void* calcular_media(void* arg) {
    long soma = 0;

    for (int i = 0; i < N; i++) {
        soma += dados[i];
    }

    media = (double)soma / N;

    // Encerra a thread (no modo sequencial esse trecho é ignorado)
    if (arg != NULL) {
        pthread_exit(NULL);
    }
}

// Função executada por uma thread para calcular a mediana
void* calcular_mediana(void* arg) {
    int* copia = malloc(N * sizeof(int)); // Cópia do vetor para ordenação

    if (!copia) { 
        pthread_exit(NULL);
    }

    // Copia os dados
    for (int i = 0; i < N; i++) {
        copia[i] = dados[i];
    }

    // Ordena a cópia
    qsort(copia, N, sizeof(int), cmpfunc);

    // Calcula a mediana
    if (N % 2 == 0) {
        mediana = (copia[N / 2 - 1] + copia[N / 2]) / 2.0;
    } else {
        mediana = copia[N / 2];
    }

    free(copia);  // Libera memória

    if (arg != NULL) {
        pthread_exit(NULL);
    }
}

// Função executada por uma thread para calcular o desvio padrão
void* calcular_desvio_padrao(void* arg) {
    double soma = 0.0;

    for (int i = 0; i < N; i++) {
        soma += dados[i];
    }

    double media_local = soma / N;

    soma = 0.0;
    for (int i = 0; i < N; i++) {
        soma += pow(dados[i] - media_local, 2);
    }

    desvio_padrao = sqrt(soma / N);

    if (arg != NULL) {
        pthread_exit(NULL);
    }
}

// Função que retorna o tempo atual em microssegundos
long long tempo_em_us() {
    struct timeval tv;
    gettimeofday(&tv, NULL);
    return tv.tv_sec * 1000000LL + tv.tv_usec;
}

int main() {
    
    // Inicializa o vetor com valores aleatórios entre 0 e 100
    srand(time(NULL));
    for (int i = 0; i < N; i++) {
        dados[i] = rand() % 101;
    }

    // -----------------------------
    printf("----- Modo com 3 Threads -----\n");
    long long inicio = tempo_em_us(); // Tempo inicial

    // Criação das 3 threads para cálculo paralelo
    pthread_t t1, t2, t3;
    pthread_create(&t1, NULL, calcular_media, NULL);
    pthread_create(&t2, NULL, calcular_mediana, NULL);
    pthread_create(&t3, NULL, calcular_desvio_padrao, NULL);

    // Aguarda as threads terminarem
    pthread_join(t1, NULL);
    pthread_join(t2, NULL);
    pthread_join(t3, NULL);

    long long fim = tempo_em_us(); // Tempo final

    // Exibe os resultados
    printf("Media: %.2lf\n", media);
    printf("Mediana: %.2lf\n", mediana);
    printf("Desvio Padrao: %.2lf\n", desvio_padrao);
    printf("Tempo com 3 threads: %lld microssegundos\n\n", fim - inicio);

    // -----------------------------
    printf("----- Modo com 1 Thread (Sequencial) -----\n");
    inicio = tempo_em_us();

    // Executa os cálculos sequencialmente
    calcular_media(NULL);
    calcular_mediana(NULL);
    calcular_desvio_padrao(NULL);

    fim = tempo_em_us();

    // Exibe os resultados
    printf("Media: %.2lf\n", media);
    printf("Mediana: %.2lf\n", mediana);
    printf("Desvio Padrao: %.2lf\n", desvio_padrao);
    printf("Tempo com 1 thread: %lld microssegundos\n\n", fim - inicio);

    // -----------------------------
    printf("----- Modo com 3 Processos -----\n");

    // Pipes para comunicação entre pai e filhos
    int p1[2], p2[2], p3[2];
    pipe(p1); pipe(p2); pipe(p3);
    inicio = tempo_em_us();

    // Processo filho 1 - calcula a média
    if (fork() == 0) {
        long soma = 0;
        
        for (int i = 0; i < N; i++) {
            soma += dados[i];
        }

        double m = (double)soma / N;

        write(p1[1], &m, sizeof(double)); // Envia pelo pipe
        close(p1[1]);
        exit(0);
    }

    // Processo filho 2 - calcula a mediana
    if (fork() == 0) {
        int* copia = malloc(N * sizeof(int));
        
        if (!copia) { 
            exit(1);
        }

        for (int i = 0; i < N; i++) {
            copia[i] = dados[i];
        }

        qsort(copia, N, sizeof(int), cmpfunc);

        double med;
        if (N % 2 == 0) {
            med = (copia[N / 2 - 1] + copia[N / 2]) / 2.0;
        } else {
            med = copia[N / 2];
        }

        free(copia);
        write(p2[1], &med, sizeof(double)); // Envia pelo pipe
        close(p2[1]);
        exit(0);
    }

    // Processo filho 3 - calcula o desvio padrão
    if (fork() == 0) {
        long soma = 0;
        
        for (int i = 0; i < N; i++) {
            soma += dados[i];
        }

        double m = (double)soma / N;

        double somatorio = 0;
        for (int i = 0; i < N; i++) {
            somatorio += pow(dados[i] - m, 2);
        }

        double dp = sqrt(somatorio / N);

        write(p3[1], &dp, sizeof(double)); // Envia pelo pipe
        close(p3[1]);
        exit(0);
    }

    // Espera os 3 processos filhos terminarem
    wait(NULL); wait(NULL); wait(NULL);

    // Lê os resultados dos pipes
    read(p1[0], &media, sizeof(double));
    read(p2[0], &mediana, sizeof(double));
    read(p3[0], &desvio_padrao, sizeof(double));
    fim = tempo_em_us();

    // Exibe os resultados
    printf("Media: %.2lf\n", media);
    printf("Mediana: %.2lf\n", mediana);
    printf("Desvio Padrao: %.2lf\n", desvio_padrao);
    printf("Tempo com 3 processos: %lld microssegundos\n\n", fim - inicio);

    return 0;
}
