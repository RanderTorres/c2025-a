#include <stdio.h>

int is_prime(int n);

int main()
{
    int n;
    scanf("%d",&n);
    if (is_prime(n))
    {
        printf("YES");
    }
    else
    {
        printf("NO");
    }

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