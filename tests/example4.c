
ptr yes;
ptr no;

extern fn int printf(ptr fmt, ...);
extern fn int scanf(ptr fmt, ...);


fn uint fibb(uint n) {
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

export fn int main(){
    yes = "yes";
    no = "no";

    uint limit; uint cnt;
    scanf("%u%u", &limit, &cnt);
    printf("limit = %u\n", limit);
    printf("cnt = %u\n", cnt);

    cnt = fibb(cnt);
    if (cnt < limit) {
        printf("%s", yes);
    } else {
        printf("%s", no);
    }

    return 0;
}
