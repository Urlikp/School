#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>

int main(int argc, char *argv[]) {
    clock_t start = clock();

    int width, count_1, count_2;
    long long result = 0, distance;

    scanf("%d %lld\n", &width, &distance);

    scanf("%d\n", &count_1);
    int* stations_1 = (int*) malloc(count_1 * sizeof(int));
    for (int i = 0; i < count_1; i++)
    {
        scanf("%d", &stations_1[i]);
    }

    scanf("%d\n", &count_2);
    int* stations_2 = (int*) malloc(count_2 * sizeof(int));
    for (int i = 0; i < count_2; i++)
    {
        scanf("%d", &stations_2[i]);
    }

    int j = 1;

    for (int i = 0; i < count_1 - 1; i++)
    {
        if (i == j)
        {
            j++;
        }
        while (j < count_1)
        {
            if (stations_1[j] - stations_1[i] <= distance)
            {
                j++;
            }
            else
            {
                break;
            }
        }
        result += j - i - 1;
    }

    j = 1;

    for (int i = 0; i < count_2 - 1; i++)
    {
        if (i == j)
        {
            j++;
        }
        while (j < count_2)
        {
            if (stations_2[j] - stations_2[i] <= distance)
            {
                j++;
            }
            else
            {
                break;
            }
        }
        result += j - i - 1;
    }

    if (width <= distance)
    {
        distance = (int) sqrt(distance * distance - width * width);
        j = 0;
        int k = 0;

        for (int i = 0; i < count_1; i++)
        {
            while (j < count_2)
            {
                if (stations_2[j] - stations_1[i] <= distance)
                {
                    j++;
                }
                else
                {
                    break;
                }
            }
            while (k < j)
            {
                if (stations_1[i] - stations_2[k] > distance)
                {
                    k++;
                }
                else
                {
                    break;
                }
            }
            result += j - k;
        }
    }

    free(stations_1);
    free(stations_2);

    printf("%lld\n", result);

    clock_t stop = clock();
    double elapsed = (double) (stop - start) / CLOCKS_PER_SEC;
    printf("\nTime elapsed: %.5f\n", elapsed);

	return 0;
}
