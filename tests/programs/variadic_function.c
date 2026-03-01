
extern fn int printf(ptr fmt, ...);

export fn int main(int argc, ptr argv) {
    return printf("%d %s %d", argc, @ptr(argv), argc);
}