class A {
public:
  virtual int hello() { return 0; }
};

class B : public A {
public:
  virtual int hello() { return 1; }
};

class C : public A {
public:
  virtual int hello() { return 2; }
};

int main() {
  B b;
  C c;
  A a;
  A &a1 = b;
  A &a2 = c;
  int res = a.hello() + a2.hello();
  return res;
}