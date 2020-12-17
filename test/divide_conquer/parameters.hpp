//qui mi salvo i vari dimension, inputsize...
namespace parameters {
#ifdef DEBUG
  const int dimension = 20;
  const size_t inputsize = 7;
<<<<<<< HEAD
  const int cut_off = 5;
#else
  const int dimension = 20;
  const size_t inputsize = 3000;
  const int cut_off = 200;
=======
#else
  const int dimension = 100;
  const size_t inputsize = 3000;
>>>>>>> e9dce9cf3051f15f46492d03c728495952c63d13
#endif
  const int minimum_wait = 5;
  bool sequential = true;
}
