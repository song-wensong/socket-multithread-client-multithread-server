#include<stdio.h>

struct test {
    char ch1;
    char ch2;
    char ch3;
    char ch4;
    int a;
};

int main()
{
    printf("sizeof(int) = %lu\n", sizeof(int));
    char test[8] = {'1', '2', '3', '4', '5', '6', '7', '8'};
    *test = '9';
    int tempa = 123;
    *(test + 4) = tempa;
    *(test + 7) = tempa;
    struct test *p = (struct test *)test;
    char ch1 = p->ch1;
    char ch2 = p->ch2;
    char ch3 = p->ch3;
    char ch4 = p->ch4;
    int a = p->a;
    printf("%c %c %c %c %d\n", ch1, ch2, ch3, ch4, a);

    return 0;
}