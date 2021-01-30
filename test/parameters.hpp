//qui mi salvo i vari dimension, inputsize...
namespace parameters {
#ifdef DEBUG
  const int dimension = 1;
  const size_t inputsize = 15;
  const size_t matrix_size = 2;
  const int matrix_production = matrix_size*matrix_size;
  const int cut_off = 1;
#else
  const int dimension = 2;
  const size_t inputsize = 500;
  const size_t matrix_size = 500;
  const int matrix_production = matrix_size*matrix_size;
  const int cut_off = 200;
#endif
  const int minimum_wait = 1;
  bool sequential = true;
}
