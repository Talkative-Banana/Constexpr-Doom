int add(int a, int b) { return a + b; }
int sub(int a, int b) { return a - b; }

int main() {
  int a = 12, b = 5;
  int c = add(a, b) - sub(a, b);
  return c;
}
