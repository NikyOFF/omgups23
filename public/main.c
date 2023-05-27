#include <stdio.h>

int main() {
    int a;
    int b;

    printf("Write a: ");
    scanf("%i", &a);

    printf("Write b: ");
    scanf("%i", &b);

    printf("\nSum: %d\n", a + b);
    printf("Dif: %d\n", a - b);

    return 0;
}