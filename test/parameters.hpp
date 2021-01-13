//qui mi salvo i vari dimension, inputsize...
namespace parameters {
#ifdef DEBUG
  const int dimension = 1;
  const size_t inputsize = 4;
  const size_t matrix_size = 4;
  const int cut_off = 1;
#else
  const int dimension = 20;
  const size_t inputsize = 30;
  const size_t matrix_size = 512;
  const int cut_off = 200;
#endif
  const int minimum_wait = 200;
  bool sequential = true;
}
