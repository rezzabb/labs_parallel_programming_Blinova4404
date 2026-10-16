#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <omp.h>

int main(int argc, char *argv[])
{
    if (argc < 2)
    {
        printf("Usage: %s <matrix_size>\n", argv[0]);
        return 1;
    }

    int N = atoi(argv[1]);

    if (N <= 0)
    {
        printf("Matrix size must be positive\n");
        return 1;
    }

    size_t elements = (size_t)N * N;

    double *A = malloc(elements * sizeof(double));
    double *B = malloc(elements * sizeof(double));
    double *C = malloc(elements * sizeof(double));

    if (A == NULL || B == NULL || C == NULL)
    {
        printf("Memory allocation error\n");
        free(A);
        free(B);
        free(C);
        return 1;
    }

    // Заполняем исходные матрицы
    for (int i = 0; i < N; i++)
    {
        for (int j = 0; j < N; j++)
        {
            size_t index = (size_t)i * N + j;

            A[index] = (i + j) % 10 + 1;
            B[index] = (i * 2 + j) % 10 + 1;
            C[index] = 0.0;
        }
    }

    char mode[20];
    int num_threads = 1;

    // Выбираем способ вычислений
    printf("Choose mode (sequential/parallel): ");

    if (scanf("%19s", mode) != 1)
    {
        printf("Input error\n");
        free(A);
        free(B);
        free(C);
        return 1;
    }

    // Если выбран параллельный режим
    if (strcmp(mode, "parallel") == 0)
    {
        printf("Enter number of threads (2, 4 or 8): ");

        if (scanf("%d", &num_threads) != 1 ||
            (num_threads != 2 &&
             num_threads != 4 &&
             num_threads != 8))
        {
            printf("Invalid number of threads\n");
            free(A);
            free(B);
            free(C);
            return 1;
        }

        omp_set_dynamic(0);
        omp_set_num_threads(num_threads);
    }
    else if (strcmp(mode, "sequential") != 0)
    {
        printf("Unknown mode\n");
        free(A);
        free(B);
        free(C);
        return 1;
    }

    double start = omp_get_wtime();

    // Последовательное умножение
    if (strcmp(mode, "sequential") == 0)
    {
        for (int i = 0; i < N; i++)
        {
            for (int k = 0; k < N; k++)
            {
                double value = A[(size_t)i * N + k];

                for (int j = 0; j < N; j++)
                {
                    C[(size_t)i * N + j] +=
                        value * B[(size_t)k * N + j];
                }
            }
        }
    }

    // Параллельное умножение
    else
    {
        #pragma omp parallel for schedule(static)
        for (int i = 0; i < N; i++)
        {
            for (int k = 0; k < N; k++)
            {
                double value = A[(size_t)i * N + k];

                for (int j = 0; j < N; j++)
                {
                    C[(size_t)i * N + j] +=
                        value * B[(size_t)k * N + j];
                }
            }
        }
    }

    double end = omp_get_wtime();

    printf("\nMatrix size: %d x %d\n", N, N);
    printf("Mode: %s\n", mode);

    printf("Mode: %s\n", mode);

    if (strcmp(mode, "parallel") == 0)
    {
        printf("Threads: %d\n", num_threads);
    }

    printf("Execution time: %.6f seconds\n", end - start);
    printf("Result C[0][0] = %.1f\n", C[0]);

    free(A);
    free(B);
    free(C);

    return 0;
}
