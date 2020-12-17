//qui mi salvo i vari dimension, inputsize...
namespace parameters {
#ifdef DEBUG
<<<<<<< HEAD
  const int dimension = 10;
  const size_t inputsize = 10;
=======
  const int dimension = 5;
  const size_t inputsize = 7;
>>>>>>> e9dce9cf3051f15f46492d03c728495952c63d13
#else
  const int dimension = 1000;
  const size_t inputsize = 3000;
#endif
  const int minimum_wait = 1;
  bool sequential = true;
}
