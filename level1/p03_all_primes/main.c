#include <stdio.h>
#include <time.h>

int is_prime(int n);

int main()
{
    clock_t start,end;
    double cpu_time_used;

    start = clock();
    for (int i=2;i<1000;i++)
    {
        if (is_prime(i))
        {
            printf("%d ",i);
        }
    }
    end = clock();

    cpu_time_used = ((double)(end - start))/CLOCKS_PER_SEC;
    printf("\nTime used: %f seconds.",cpu_time_used);

    return 0;
}

int is_prime(int n)
{
    if (n==1)
    {
        return 0;
    }
    for (int i=2;i<=n/2;i++)    // 节省时间，只需算到二分之n
    {
        if (n%i==0)
        {
            return 0;
        }
    }
    return 1;
}