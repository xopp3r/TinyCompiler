int a;
extern uint b;
export char c;

export fn int main() {
    b = a = a + b as int - c as int;
    return b;
}