//qui mi salvo i vari dimension, inputsize...
namespace parameters {
#ifdef DEBUG
  const int dimension = 5;
  const size_t inputsize = 16;
  const int cut_off = 5;
#else
  const int dimension = 20;
  const size_t inputsize = 3000;
  const int cut_off = 200;
#endif
  const int minimum_wait = 5;
  bool sequential = true;
}
