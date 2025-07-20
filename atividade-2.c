#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <math.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/time.h>

#define N 10000

int dados[N];
double media = 0.0, mediana = 0.0, desvio_padrao = 0.0;

// Função de comparação para qsort
int cmpfunc(const void* a, const void* b) {
    return (*(int*)a - *(int*)b);
}

// Funções para as threads
void* calcular_media(void* arg) {
    long soma = 0;
    for (int i = 0; i < N; i++)
        soma += dados[i];
    media = (double)soma / N;
    pthread_exit(NULL);
}

void* calcular_mediana(void* arg) {
    qsort(dados, N, sizeof(int), cmpfunc);
    if (N % 2 == 0)
        mediana = (dados[N / 2 - 1] + dados[N / 2]) / 2.0;
    else
        mediana = dados[N / 2];
    pthread_exit(NULL);
}

void* calcular_desvio_padrao(void* arg) {
    double soma = 0.0;
    for (int i = 0; i < N; i++)
        soma += dados[i];
    double media_local = soma / N;

    soma = 0.0;
    for (int i = 0; i < N; i++)
        soma += pow(dados[i] - media_local, 2);

    desvio_padrao = sqrt(soma / N);
    pthread_exit(NULL);
}

// Tempo em microssegundos
long long tempo_em_us() {
    struct timeval tv;
    gettimeofday(&tv, NULL);
    return tv.tv_sec * 1000000LL + tv.tv_usec;
}

// Função principal
int main() {
    srand(time(NULL));
    for (int i = 0; i < N; i++)
        dados[i] = rand() % 101;

    printf("----- Modo com 3 Threads -----\n");
    long long inicio = tempo_em_us();

    pthread_t t1, t2, t3;
    pthread_create(&t1, NULL, calcular_media, NULL);
    pthread_create(&t2, NULL, calcular_mediana, NULL);
    pthread_create(&t3, NULL, calcular_desvio_padrao, NULL);

    pthread_join(t1, NULL);
    pthread_join(t2, NULL);
    pthread_join(t3, NULL);

    long long fim = tempo_em_us();
    printf("Média: %.2lf\n", media);
    printf("Mediana: %.2lf\n", mediana);
    printf("Desvio Padrão: %.2lf\n", desvio_padrao);
    printf("Tempo com 3 threads: %lld microssegundos\n\n", fim - inicio);

    // -----------------------------
    printf("----- Modo com 1 Thread (Sequencial) -----\n");
    inicio = tempo_em_us();

    calcular_media(NULL);
    calcular_mediana(NULL);
    calcular_desvio_padrao(NULL);

    fim = tempo_em_us();
    printf("Média: %.2lf\n", media);
    printf("Mediana: %.2lf\n", mediana);
    printf("Desvio Padrão: %.2lf\n", desvio_padrao);
    printf("Tempo com 1 thread: %lld microssegundos\n\n", fim - inicio);

    // -----------------------------
    printf("----- Modo com 3 Processos -----\n");

    int p1[2], p2[2], p3[2];
    pipe(p1); pipe(p2); pipe(p3);
    inicio = tempo_em_us();

    if (fork() == 0) { // Processo filho 1
        double m = 0;
        long soma = 0;
        for (int i = 0; i < N; i++)
            soma += dados[i];
        m = (double)soma / N;
        write(p1[1], &m, sizeof(double));
        close(p1[1]);
        exit(0);
    }

    if (fork() == 0) { // Processo filho 2
        double med = 0;
        qsort(dados, N, sizeof(int), cmpfunc);
        if (N % 2 == 0)
            med = (dados[N / 2 - 1] + dados[N / 2]) / 2.0;
        else
            med = dados[N / 2];
        write(p2[1], &med, sizeof(double));
        close(p2[1]);
        exit(0);
    }

    if (fork() == 0) { // Processo filho 3
        double m = 0, dp = 0;
        long soma = 0;
        for (int i = 0; i < N; i++)
            soma += dados[i];
        m = (double)soma / N;

        double somatorio = 0;
        for (int i = 0; i < N; i++)
            somatorio += pow(dados[i] - m, 2);
        dp = sqrt(somatorio / N);

        write(p3[1], &dp, sizeof(double));
        close(p3[1]);
        exit(0);
    }

    wait(NULL); wait(NULL); wait(NULL);

    read(p1[0], &media, sizeof(double));
    read(p2[0], &mediana, sizeof(double));
    read(p3[0], &desvio_padrao, sizeof(double));
    fim = tempo_em_us();

    printf("Média: %.2lf\n", media);
    printf("Mediana: %.2lf\n", mediana);
    printf("Desvio Padrão: %.2lf\n", desvio_padrao);
    printf("Tempo com 3 processos: %lld microssegundos\n\n", fim - inicio);

    return 0;
}
