int add(int a, int b) { return a + b; }
int sub(int a, int b) { return a - b; }

int main() {
  int a = 10, b = 4;
  int c = add(a, b) - sub(a, b);
  return c;
}
