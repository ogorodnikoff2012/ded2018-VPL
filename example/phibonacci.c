int read_int();
int write_int(int value);

int phibonacci(int x) {
    if (x < 2) {
        return 1;
    } else {
        return phibonacci(x - 1) + phibonacci(x - 2);
    }
}

int main() {
    int x;
    x = read_int();
    write_int(phibonacci(x));
    return 0;
}
