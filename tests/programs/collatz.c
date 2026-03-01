

fn uint collatz(int n) {
    if (n == 0) { return 0 as uint; }
    
    uint len;
    len = 0 as uint;

    while (n != 1) {
        if (n % 2 == 0) {
            n = n / 2;
        } else {
            n = 3 * n + 1;
        }
    }

    return len;
}

extern fn int printf(ptr fmt, ...);
extern fn int scanf(ptr fmt, ...);

export fn int main() {
    int n;
    scanf("%d", &n);
    printf("%u", collatz(n));
    return 0;
}