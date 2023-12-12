double ft_sqrt(const double x) {
  double result = 1;
  double tmp = 0;

  while (result != tmp) {
    tmp = result;
    result = (result + x / result) / 2;
  }
  return result;
}
