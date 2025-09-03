// 凯撒加密

#include <stdio.h>

// 设置偏移量
const int BIAS = -10;

int encrypt(char *str);
int decrypt(char *str);

int main()
{
    char str[100];
    printf("Input something to encrypt: ");
    fgets(str, 100, stdin);

    // 加密
    encrypt(str);
    printf("Encrypt: ");
    fputs(str, stdout);

    // 解密
    decrypt(str);
    printf("\nDecrypt: ");
    fputs(str, stdout);


    return 0;

}

int encrypt(char *str)
{
    for (int i = 0; str[i] != '\n'; i++)
    {
        str[i] += BIAS;
    }

    return 0;
}

int decrypt(char *str)
{
    for (int i = 0; str[i] != '\n'; i++)
    {
        str[i] -= BIAS;
    }
}