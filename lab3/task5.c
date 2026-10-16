#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <omp.h>

typedef struct
{
    double x;
    double y;
    double z;
} Point;

int main(int argc, char *argv[])
{
    // Проверяем аргумент командной строки
    if (argc != 2 ||
        (strcmp(argv[1], "loop") != 0 &&
         strcmp(argv[1], "functional") != 0))
    {
        printf("Usage: %s loop|functional\n", argv[0]);
        return 1;
    }

    // Открываем файл с координатами
    FILE *file = fopen("points.txt", "r");

    if (file == NULL)
    {
        printf("Cannot open points.txt\n");
        return 1;
    }

    // Считаем количество точек
    size_t N = 0;
    double x, y, z;

    while (fscanf(file, "%lf %lf %lf", &x, &y, &z) == 3)
    {
        N++;
    }

    if (N == 0)
    {
        printf("No points found in file\n");
        fclose(file);
        return 1;
    }

    // Возвращаемся в начало файла
    rewind(file);

    // Выделяем память для точек
    Point *points = malloc(N * sizeof(Point));

    if (points == NULL)
    {
        printf("Memory allocation error\n");
        fclose(file);
        return 1;
    }

    // Читаем координаты точек
    for (size_t i = 0; i < N; i++)
    {
        if (fscanf(file, "%lf %lf %lf",
                   &points[i].x,
                   &points[i].y,
                   &points[i].z) != 3)
        {
            printf("Error reading points\n");
            free(points);
            fclose(file);
            return 1;
        }
    }

    fclose(file);

    // Суммы координат
    double sum_x = 0.0;
    double sum_y = 0.0;
    double sum_z = 0.0;

    omp_set_dynamic(0);

    // Начало измерения времени
    double start = omp_get_wtime();

    // Способ 1: распараллеливание цикла
    if (strcmp(argv[1], "loop") == 0)
    {
        #pragma omp parallel for reduction(+:sum_x,sum_y,sum_z)
        for (long long i = 0; i < (long long)N; i++)
        {
            sum_x += points[i].x;
            sum_y += points[i].y;
            sum_z += points[i].z;
        }
    }

    // Способ 2: функциональная декомпозиция
    else if (strcmp(argv[1], "functional") == 0)
    {
        #pragma omp parallel sections num_threads(3)
        {
            // Секция 1: сумма координат X
            #pragma omp section
            {
                for (size_t i = 0; i < N; i++)
                    sum_x += points[i].x;
            }

            // Секция 2: сумма координат Y
            #pragma omp section
            {
                for (size_t i = 0; i < N; i++)
                    sum_y += points[i].y;
            }

            // Секция 3: сумма координат Z
            #pragma omp section
            {
                for (size_t i = 0; i < N; i++)
                    sum_z += points[i].z;
            }
        }
    }

    // Вычисляем геометрический центр
    double center_x = sum_x / N;
    double center_y = sum_y / N;
    double center_z = sum_z / N;

    // Конец измерения времени
    double end = omp_get_wtime();

    // Выводим результаты
    printf("Number of points: %zu\n", N);
    printf("Method: %s\n", argv[1]);

    printf("Geometric center:\n");
    printf("X = %.3f\n", center_x);
    printf("Y = %.3f\n", center_y);
    printf("Z = %.3f\n", center_z);

    printf("Execution time: %.6f seconds\n", end - start);

    free(points);

    return 0;
}
