//qui mi salvo i vari dimension, inputsize...
namespace parameters {
#ifdef DEBUG
  const int dimension = 3;
  const size_t inputsize = 7;
#else
  const int dimension = 5;
  const size_t inputsize = 13;
#endif
  const int minimum_wait = 5;
  bool sequential = true;
}
