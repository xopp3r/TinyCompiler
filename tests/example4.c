// test sample

// #include "io"


int square(int number){
    int sqr;
    sqr = number *number;
    return sqr;
}

pointer yes = "yes";
pointer no = "no";


int main(void){
    
    int x = 7;
    printf("%d", x);
    x = x + 2;
    printf("%d", x);
    x = sqr(x);

    if (x/6 < 7* 7)   {
        printf("%s", yes);
    } else {
        printf("%s", no);
    }

    for (int y = x; y > 0; y++){
        printf("%d", y);
    }

    return 0;
}
