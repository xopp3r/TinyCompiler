export fn int main(int argc, ptr argv) {
    @char(argv + 15) = 42 as char; 
    return @uint(argv + 11);
}