#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include <time.h>

struct test {
    char ch1;
    char ch2;
    char ch3;
    char ch4;
    int a;
};

int main()
{
    // printf("sizeof(int) = %lu\n", sizeof(int));
    // char test[8] = {'1', '2', '3', '4', '5', '6', '7', '8'};
    // *test = '9';
    // int tempa = 123;
    // *(test + 4) = tempa;
    // *(test + 7) = tempa;
    // struct test *p = (struct test *)test;
    // char ch1 = p->ch1;
    // char ch2 = p->ch2;
    // char ch3 = p->ch3;
    // char ch4 = p->ch4;
    // int a = p->a;
    // printf("%c %c %c %c %d\n", ch1, ch2, ch3, ch4, a);

    char data[1024] = "ubuntu";
    char a[1032];
    memset(a, 0, sizeof(a));
    *a = '$';
    *(a + 1) = 'Q';
    *(a + 2) = 'N';
    // int len = strlen(data) + ((char*)((int*)(a + 3) + 1) + 1) - a;
    // // int len = ((char*)((int*)(a + 3) + 1) + 1) - a;
    int len = strlen(data) + sizeof(char) * 4 + sizeof(int);
    printf("%d\n", len);

    *(int*)(a + 3) = -1;
    *(int*)(a + 3) = -1;
    *((int*)(a + 3)) = len;
    *((char*)((int *)(a + 3) + 1)) = '$';
    strcat((char*)((int *)(a + 3) + 1), data);
    // strcat(a, data);
    for (int i = 0; i < len; i++) {
        printf("%c ", *(a + i));
        printf("\n");
    }
    printf("%d\n", *((int*)(a + 3)));
    printf("%s\n", a);

    // time_t *timep = malloc(sizeof(time_t));
    // time(timep);
    // char *s = ctime(timep);
    // printf("%s  %d %d\n",s, sizeof(s), strlen(s));
    return 0;  
}