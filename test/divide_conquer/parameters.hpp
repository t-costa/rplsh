//qui mi salvo i vari dimension, inputsize...
namespace parameters {
#ifdef DEBUG
  const int dimension = 20;
  const size_t inputsize = 7;
#else
  const int dimension = 100;
  const size_t inputsize = 3000;
#endif
  const int minimum_wait = 5;
  bool sequential = true;
}
