//qui mi salvo i vari dimension, inputsize...
namespace parameters {
#ifdef DEBUG
  const int dimension = 10;
  const size_t inputsize = 12;
#else
  const int dimension = 1000;
  const size_t inputsize = 3000;
#endif
  const int minimum_wait = 1;
}
