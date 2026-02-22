// #include <stdio.h>

int main(void) {
    unsigned int n;
    scanf("%u", &n);
    
    int sum = 0;
    
    // пропускаем цикл, если исходное число было нечетным
    if (sum % 2) goto end;
    
    n /= 2; // shr esi, 1
    
    while (n != 0) {
        int a, b;
        scanf("%d", &a);  
        scanf("%d", &b);  
        sum += a * b;     
        n--;
    }
    
    end:

    printf("%d\n", sum); 
    
    return 0;
}