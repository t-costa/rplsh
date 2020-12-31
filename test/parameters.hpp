//qui mi salvo i vari dimension, inputsize...
namespace parameters {
#ifdef DEBUG
  const int dimension = 2;
  const size_t inputsize = 16;
  const size_t matrix_size = 2;
  const int cut_off = 5;
#else
  const int dimension = 20;
  const size_t inputsize = 3000;
  const size_t matrix_size = 512;
  const int cut_off = 200;
#endif
  const int minimum_wait = 5;
  bool sequential = true;
}
