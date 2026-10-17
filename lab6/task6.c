#include <stdio.h>
#include <stdlib.h>
#include <omp.h>

typedef struct
{
    double x;
    double y;
    double z;
} Point;

int main(void)
{
    // Открываем файл с координатами точек
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

    // Возвращаемся к началу файла
    rewind(file);

    // Выделяем память для точек
    Point *points = malloc(N * sizeof(Point));

    if (points == NULL)
    {
        printf("Memory allocation error\n");
        fclose(file);
        return 1;
    }

    // Считываем координаты в массив
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

    // Общая сумма координат
    double global_sum = 0.0;

    omp_set_dynamic(0);

    double start = omp_get_wtime();

    // Функциональная декомпозиция на три секции
    #pragma omp parallel sections num_threads(3)
    {
        // Секция 1: суммируем координаты X
        #pragma omp section
        {
            double local_sum_x = 0.0;

            for (size_t i = 0; i < N; i++)
            {
                local_sum_x += points[i].x;
            }

            // Добавляем результат в общую сумму
            #pragma omp critical
            {
                global_sum += local_sum_x;
            }
        }

        // Секция 2: суммируем координаты Y
        #pragma omp section
        {
            double local_sum_y = 0.0;

            for (size_t i = 0; i < N; i++)
            {
                local_sum_y += points[i].y;
            }

            #pragma omp critical
            {
                global_sum += local_sum_y;
            }
        }

        // Секция 3: суммируем координаты Z
        #pragma omp section
        {
            double local_sum_z = 0.0;

            for (size_t i = 0; i < N; i++)
            {
                local_sum_z += points[i].z;
            }

            #pragma omp critical
            {
                global_sum += local_sum_z;
            }
        }
    }

    // Вычисляем одно общее среднее значение
    double result = global_sum / (3.0 * N);

    double end = omp_get_wtime();

    // Выводим результат
    printf("Number of points: %zu\n", N);
    printf("Global sum: %.3f\n", global_sum);
    printf("Result: %.3f\n", result);
    printf("Execution time: %.6f seconds\n", end - start);

    // Освобождаем память
    free(points);

    return 0;
}
