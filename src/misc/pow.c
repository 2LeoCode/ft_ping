double ft_pow(const double x, const int y) {
  double result = 1;
  int i = 0;

  while (i < y) {
    result *= x;
    ++i;
  }
  return result;
}
