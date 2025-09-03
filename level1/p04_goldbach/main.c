// （1）每个大于2的偶数都是两个素数之和；（2）每个大于5的奇数都是三个素数之和。

#include <stdio.h>
#include <math.h>

int verify1(int n);
int verify2(int n);
int is_prime(int n);

int primeList[50];    // 存储100以内的素数
int t = 0; // 100以内素数个数

int main()
{
    // 找到100以内的素数
    for (int i = 2; i<100; i++)
    {
        if (is_prime(i))
        {
            primeList[t] = i;
            t++;
        }
    }

    // 验证1
    printf("Verify1:\n");
    for (int i = 4; i<=100; i+=2)
    {
        if (verify1(i))
        {
            printf("Wrong! %d is not a sum of two primes.\n", i);
        }
    }

    // 验证2
    printf("\nVerify2:\n");
    for (int i = 7; i<=100; i+=2)
    {
        if (verify2(i))
        {
            printf("Wrong! %d is not a sum of three primes.\n", i);
        }
    }

    return 0;
}

// 验证n是否满足第一条
int verify1(int n)
{
    for (int i = 0; i < t; i++)
    {
        for (int j = i; j < t; j++)
        {
            if (primeList[i] + primeList[j] == n)
            {
                printf("%d = %d + %d\n", n, primeList[i], primeList[j]);
                return 0;
            }
        }
    }
    return -1;
}

// 验证n是否满足第二条
int verify2(int n)
{
    for (int i = 0; i < t; i++)
    {
        for (int j = i; j < t; j++)
        {
            for (int k = j; k < t; k++)
            {
                if (primeList[i] + primeList[j] + primeList[k] == n)
                {
                    printf("%d = %d + %d + %d\n", n, primeList[i], primeList[j], primeList[k]);
                    return 0;
                }
            }
        }
    }
    return -1;
}


// 判断是否为素数
int is_prime(int n)
{
    if (n==1)
    {
        return 0;
    }
    for (int i=2;i<=sqrt(n);i++)    // 节省时间，只需算到sqrt(n)
    {
        if (n%i==0)
        {
            return 0;
        }
    }
    return 1;
}