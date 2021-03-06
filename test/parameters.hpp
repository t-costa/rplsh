//qui mi salvo i vari dimension, inputsize...
namespace parameters {
#ifdef DEBUG
  const int dimension = 3;
  const int matrix_stream = 1;
  const size_t inputsize = 15;
  const size_t matrix_size = 4;
  const int matrix_production = matrix_size*matrix_size;
  const int cut_off = 1;
#else
  const int dimension = 10;
  const int matrix_stream = 1;
  const size_t inputsize = 1000;
  const size_t matrix_size = 1000;
  const int matrix_production = matrix_size*matrix_size;
  const int cut_off = 100;
#endif
  const int minimum_wait = 1;
  bool sequential = true;
}
