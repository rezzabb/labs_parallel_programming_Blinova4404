#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <omp.h>

#define N 16000

int main(int argc, char *argv[])
{
    if (argc < 3)
    {
        printf("Usage: %s <number of threads> <schedule>\n", argv[0]);
        printf("schedule: static, dynamic, guided\n");
        return 1;
    }

    int potoki = atoi(argv[1]);
    char *schedule_type = argv[2];

    double a[N];
    double b[N];

    for (int i = 0; i < N; i++)
    {
        a[i] = i;
    }

    omp_set_num_threads(potoki);

    if (strcmp(schedule_type, "static") == 0)
    {
        #pragma omp parallel for schedule(static)
        for (int i = 1; i < N - 1; i++)
        {
            b[i] = (a[i - 1] + a[i] + a[i + 1]) / 3.0;
        }
    }
    else if (strcmp(schedule_type, "dynamic") == 0)
    {
        #pragma omp parallel for schedule(dynamic)
        for (int i = 1; i < N - 1; i++)
        {
            b[i] = (a[i - 1] + a[i] + a[i + 1]) / 3.0;
        }
    }
    else if (strcmp(schedule_type, "guided") == 0)
    {
        #pragma omp parallel for schedule(guided)
        for (int i = 1; i < N - 1; i++)
        {
            b[i] = (a[i - 1] + a[i] + a[i + 1]) / 3.0;
        }
    }
    else
    {
        printf("Unknown schedule type\n");
        return 1;
    }

    printf("Number of elements: %d\n", N);
    printf("Number of threads: %d\n", potoki);
    printf("Schedule: %s\n", schedule_type);

    printf("\nFirst elements:\n");

    for (int i = 1; i < 10; i++)
    {
        printf("b[%d] = %.2f\n", i, b[i]);
    }

    return 0;
}
