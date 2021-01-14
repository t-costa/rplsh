//qui mi salvo i vari dimension, inputsize...
namespace parameters {
#ifdef DEBUG
  const int dimension = 1;
  const size_t inputsize = 4;
  const size_t matrix_size = 4;
  const int cut_off = 1;
#else
  const int dimension = 150;
  const size_t inputsize = 500;
  const size_t matrix_size = 512;
  const int cut_off = 200;
#endif
  const int minimum_wait = 100;
  bool sequential = true;
}
