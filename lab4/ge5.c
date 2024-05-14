// requires additional changes to the code to make it work

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <time.h>
#include <x86intrin.h>

#define BLKSIZE 8
#define max(x, y) (((x) < (y)) ? (y) : (x))
#define A(i, j) A[(i) * SIZE + (j)]

static double gtod_ref_time_sec = 0.0;

/* Adapted from the bl2_clock() routine in the BLIS library */

double
dclock ()
{
    double the_time, norm_sec;
    struct timeval tv;
    gettimeofday (&tv, NULL);
    if (gtod_ref_time_sec == 0.0)
        gtod_ref_time_sec = (double)tv.tv_sec;
    norm_sec = (double)tv.tv_sec - gtod_ref_time_sec;
    the_time = norm_sec + tv.tv_usec * 1.0e-6;
    return the_time;
}

int
ge (double *A, size_t SIZE)
{
    register int i, j, k;
    for (k = 0; k < SIZE; k++)
    {
        register double c = A (k, k);
        for (i = k + 1; i < SIZE; i++)
        {
            register double multiplier = A (i, k) / c;
            for (j = k + 1; j < SIZE;)
            {
                if (j < max (SIZE - BLKSIZE, 0))
                {
                    A (i, j + 0) -= A (k, j + 0) * multiplier;
                    A (i, j + 1) -= A (k, j + 1) * multiplier;
                    A (i, j + 2) -= A (k, j + 2) * multiplier;
                    A (i, j + 3) -= A (k, j + 3) * multiplier;
                    A (i, j + 4) -= A (k, j + 4) * multiplier;
                    A (i, j + 5) -= A (k, j + 5) * multiplier;
                    A (i, j + 6) -= A (k, j + 6) * multiplier;
                    A (i, j + 7) -= A (k, j + 7) * multiplier;
                    j += BLKSIZE;
                }
                else
                {
                    A (i, j) = A (i, j) - A (k, j) * multiplier;
                    j++;
                }
            }
        }
    }
    return 0;
}

int
main (int argc, const char *argv[])
{
    int i, j, k, iret;
    double dtime;
    int SIZE = 1500;
    double *matrix = calloc (SIZE * SIZE, sizeof (double));

    srand (1);
    for (i = 0; i < SIZE; i++)
    {
        for (j = 0; j < SIZE; j++)
        {
            matrix[j + SIZE * i] = rand ();
        }
    }
    printf ("call GE");
    dtime = dclock ();
    iret = ge (matrix, SIZE);
    dtime = dclock () - dtime;
    printf ("Time: %le \n", dtime);

    double check = 0.0;
    for (i = 0; i < SIZE; i++)
    {
        for (j = 0; j < SIZE; j++)
        {
            check = check + matrix[i + SIZE * j];
        }
    }
    printf ("Check: %le \n", check);
    fflush (stdout);
    free (matrix);

    return iret;
}
