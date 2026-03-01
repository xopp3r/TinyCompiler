
extern fn int printf(ptr fmt, ...);
extern fn int scanf(ptr fmt, ...);
extern fn uint clock();

fn uint fibb_loop(uint n) {
    uint prev; uint curr;
    prev = curr = 1;
    
    uint i;
    while (i < n) {
        uint tmp;
        tmp = prev;
        prev = curr;
        curr = prev + tmp;
    }
    
    return curr;
}

fn uint fibb_recursive(uint n) {
    if (n == 0 as uint || n == 1 as uint) {
        return 1;
    } 
    
    return fibb_recursive(n - 1 as uint) + fibb_recursive(n - 2 as uint);
}

uint prev_n;
uint prev_v;
uint prev_prev_v;

fn uint fibb_recursive_memo(uint n) {
    if (n == 0 as uint || n == 1 as uint) {
        return 1 as uint;
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

    start_time = clock();
    fibb_recursive(limit);
    end_time = clock();
    printf("recursive version took %u cycles\n", end_time - start_time);

    return 0;
}
