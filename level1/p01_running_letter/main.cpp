#include <iostream>
#include <Windows.h>

int main()
{
    using namespace std;

    char a = 'A';
    int i, t;
    while (true)
    {
        for (t = 1; t < 100; t++)
        {

            for (i = 0; i < t; i++)
            {
                cout << " ";
            }
            cout << a;
            Sleep(10);
            system("cls");
        }
        for (t = 99; t > 0; t--)
        {
            for (int j = 0; j < t; j++)
            {
                cout << " ";
            }
            cout << a;
            Sleep(10);
            system("cls");
        }
    }

    return 0;
}