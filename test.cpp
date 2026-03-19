// test.c
__attribute__((import_module("env"), import_name("print"))) void print(int x);

int main() {
  print(42);
  return 0;
}