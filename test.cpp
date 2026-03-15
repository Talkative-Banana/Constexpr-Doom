int fact(int n) {
  if (n <= 1)
    return 1;

  return n * fact(n - 1);
}

int fibbo(int n) {
  if (n <= 2) {
    return 1;
  }

  return fibbo(n - 1) + fibbo(n - 2);
}

int main() {
  int r = fibbo(3);
  return r;
}