extern fn int scanf(ptr fmt, ...);
extern fn int printf(ptr fmt, ...);

fn uint collatz(int n) {
    uint len; len = 0 as uint;

    while (n > 1) {
        if (n % 2 == 0) {
            n = n / 2;
        } else {
            n = 3 * n + 1;
        }
        len = len + 1 as uint;
    }

    return len;
}

fn uint longest_collatz(int n) {
    uint max_l; max_l = 0 as uint;
    
    uint i; i = 0 as uint;
    while (i < n as uint) {
        uint curr_l; curr_l = collatz(i);

        if (curr_l > max_l) {
            max_l = curr_l;
        }

        i = i + 1 as uint;
    }

    return max_l;
}

export fn int main() {
    int n;
    scanf("%d", &n);
    printf("longest collatz seq from 0 to %d has %u steps\n", n, longest_collatz(n));
    return 0;
}