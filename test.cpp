int matrix[8][8];

void fill(int val) {
  for (int i = 0; i < 8; i++)
    for (int j = 0; j < 8; j++)
      matrix[i][j] = val + i * 8 + j;
}

int sum() {
  int s = 0;
  for (int i = 0; i < 8; i++)
    for (int j = 0; j < 8; j++)
      s += matrix[i][j];
  return s;
}

int main() {
  fill(0);
  return sum(); // expected: 2016
}