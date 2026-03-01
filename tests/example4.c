
extern fn int printf(ptr fmt, ...);
extern fn int scanf(ptr fmt, ...);
extern fn uint clock();

fn uint fibb_loop(uint n) {
    if (n < 2 as uint) {
        return n;
    }

    uint prev; uint curr;
    prev = curr = 1;
    
    uint i;
    i = 0 as uint;
    while (i < n) {
        uint tmp;
        tmp = prev;
        prev = curr;
        curr = prev + tmp;
        i = i + 1 as uint;
    }
    
    return curr;
}

uint calls_cnt;

fn uint fibb_recursive(uint n) {
    calls_cnt = calls_cnt + 1 as uint;

    if (n == 0 as uint || n == 1 as uint) {
        return n;
    } 
    
    return fibb_recursive(n - 1 as uint) + fibb_recursive(n - 2 as uint);
}


export fn int main(){
    printf("write a number\n");
    uint limit;
    scanf("%u", &limit);
    printf("benchmarking %uth fibbonaci number computation\n", limit);
    
    uint start_time;
    uint end_time;

    start_time = clock();
    fibb_loop(limit);
    end_time = clock();
    printf("loop version took %u cycles\n", end_time - start_time);

    calls_cnt = 0;
    start_time = clock();
    fibb_recursive(limit);
    end_time = clock();
    printf("recursive version took %u cycles and %u function calls\n", end_time - start_time, calls_cnt);

    return 0;
}
