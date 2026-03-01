export fn int main(int argc, ptr argv) {
    @char((argv as int + 15) as ptr) = 42 as char; 
    return @uint((argv as int + 11) as ptr);
}