#include <stdio.h>
#include <stdlib.h>
#include <omp.h>

int main(int argc, char *argv[])
{
    int potoki;

    if (argc < 2)
    {
        printf("%s <Trebuetcya chislo potokov>\n", argv[0]);
        return 1;
    }

    potoki = atoi(argv[1]);

    omp_set_num_threads(potoki);

    #pragma omp parallel
    {
        int potok_id = omp_get_thread_num();
        int num_potoki = omp_get_num_threads();

        printf("Potok %d: %d potokov: Hello World\n",
               potok_id, num_potoki);
    }

    return 0;
}
