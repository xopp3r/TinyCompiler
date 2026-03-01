
ptr yes;
ptr no;

extern fn int puts(ptr str);
extern fn int scanf(ptr fmt, ptr arg1, ptr arg2);


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

    cnt = fibb(cnt);
    if (cnt < limit) {
        puts(yes);
    } else {
        puts(no);
    }

    return 0;
}
