const char *str = "hello";

int strlen_manual() {
  int i = 0;
  while (str[i] != 0)
    i++;
  return i;
}

int main() {
  return strlen_manual(); // expect 5
}