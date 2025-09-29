#include <stdio.h>

void hanoi(int n, char fom, char by, char to);
void moveOne(char from, char to);

int main()
{
    hanoi(64, 'A', 'B', 'C');
    return 0;
}

void hanoi(int n, char fom, char by, char to)
{
    if (n==1)
    {
        moveOne(fom, to);
        return ;
    }
    else
    {
        hanoi(n-1, fom, to, by);
        moveOne(fom, to);
        hanoi(n-1, by, fom, to);
    }
}

void moveOne(char from, char to)
{
    printf("%c -> %c\n", from, to);
}