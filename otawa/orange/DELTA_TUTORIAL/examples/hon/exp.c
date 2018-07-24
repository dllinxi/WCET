int main ()
{
  volatile int x, w;
  int y, z;
  z = x + y * y + w * x;
  return z;
}
