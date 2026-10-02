#include <stdio.h>
#include <stdlib.h>
#include <omp.h>


// ============================================================
// СПОСОБ 1: BARRIER
// ============================================================

void method_barrier(int potoki)
{
    int next = potoki - 1;

    omp_set_num_threads(potoki);

    #pragma omp parallel shared(next)
    {
        int tid = omp_get_thread_num();
        int total = omp_get_num_threads();

        while (1)
        {
            #pragma omp barrier

            if (tid == next)
            {
                printf("Thread %d of %d: Hello World\n", tid, total);
                next--;
            }

            #pragma omp barrier

            if (next < 0)
            {
                break;
            }
        }
    }
}


// ============================================================
// СПОСОБ 2: CRITICAL
// ============================================================

void method_critical(int potoki)
{
    int next = potoki - 1;

    omp_set_num_threads(potoki);

    #pragma omp parallel shared(next)
    {
        int tid = omp_get_thread_num();
        int total = omp_get_num_threads();

        while (1)
        {
            int finished = 0;

            #pragma omp critical
            {
                if (tid == next)
                {
                    printf("Thread %d of %d: Hello World\n",
                           tid, total);

                    next--;
                }

                if (next < 0)
                {
                    finished = 1;
                }
            }

            if (finished)
            {
                break;
            }
        }
    }
}


// ============================================================
// СПОСОБ 3: FLUSH
// ============================================================

void method_flush(int potoki)
{
    int next = potoki - 1;

    omp_set_num_threads(potoki);

    #pragma omp parallel shared(next)
    {
        int tid = omp_get_thread_num();
        int total = omp_get_num_threads();

        while (1)
        {
            int current;

            #pragma omp flush(next)

            current = next;

            if (current < 0)
            {
                break;
            }

            if (tid == current)
            {
                printf("Thread %d of %d: Hello World\n",
                       tid, total);

                next--;

                #pragma omp flush(next)
            }
        }
    }
}


// ============================================================
// СПОСОБ 4: ATOMIC
// ============================================================

void method_atomic(int potoki)
{
    int next = potoki - 1;

    omp_set_num_threads(potoki);

    #pragma omp parallel shared(next)
    {
        int tid = omp_get_thread_num();
        int total = omp_get_num_threads();

        while (1)
        {
            int current;

            #pragma omp atomic read
            current = next;

            if (current < 0)
            {
                break;
            }

            if (tid == current)
            {
                printf("Thread %d of %d: Hello World\n",
                       tid, total);

                #pragma omp atomic write
                next = tid - 1;
            }
        }
    }
}


// ============================================================
// СПОСОБ 5: MASTER + BARRIER
// ============================================================

void method_master(int potoki)
{
    int next = potoki - 1;
    int current = -1;

    omp_set_num_threads(potoki);

    #pragma omp parallel shared(next, current)
    {
        int tid = omp_get_thread_num();
        int total = omp_get_num_threads();

        for (int i = 0; i < potoki; i++)
        {
            #pragma omp barrier

            #pragma omp master
            {
                current = next;
                next--;
            }

            #pragma omp barrier

            if (tid == current)
            {
                printf("Thread %d of %d: Hello World\n",
                       tid, total);
            }

            #pragma omp barrier
        }
    }
}



int main(int argc, char *argv[])
{
    if (argc < 3)
    {
        printf("Usage: %s <number of threads> <method>\n", argv[0]);
        printf("\n");
        printf("Methods:\n");
        printf("1 - barrier\n");
        printf("2 - critical\n");
        printf("3 - flush\n");
        printf("4 - atomic\n");
        printf("5 - master + barrier\n");

        return 1;
    }

    int potoki = atoi(argv[1]);
    int method = atoi(argv[2]);

    if (potoki <= 0)
    {
        printf("Number of threads must be greater than 0\n");
        return 1;
    }

    if (method < 1 || method > 5)
    {
        printf("Method must be from 1 to 5\n");
        return 1;
    }

    printf("Number of threads: %d\n", potoki);
    printf("Selected method: %d\n\n", method);

    switch (method)
    {
        case 1:
            method_barrier(potoki);
            break;

        case 2:
            method_critical(potoki);
            break;
        case 3:
            method_flush(potoki);
            break;

        case 4:
            method_atomic(potoki);
            break;

        case 5:
            method_master(potoki);
            break;
    }

    return 0;
}
