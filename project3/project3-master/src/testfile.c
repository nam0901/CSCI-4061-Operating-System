#include<stdio.h>
int main()
{
    // a = 5(00000101), b = 9(00001001)
    unsigned char a = 255, b = 9;

    // The result is 00001010
    unsigned char tmp = a>>2;
    printf("a>>2 = %d\n", tmp);
    printf("a = %d\n", a);

    // The result is 00010010
    printf("b<<1 = %d\n", b<<1);
    return 0;
}
