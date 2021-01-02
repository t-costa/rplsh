//matrix multiplication
#include <iostream>
#include <vector>

#include "../definition.hpp"

int main() {
  map_matrix_mul_stage a;
  source_matrixpair_stage source;
  drain_matrix_stage drain;


  auto start = std::chrono::high_resolution_clock::now();

  while (source.has_next()) {
    auto m = new matrix();
    auto mc = source.next();
    *m = a.compute(*mc);
    drain.process(m);
    delete mc;
  }

  auto end = std::chrono::high_resolution_clock::now();
  auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end-start);
  std::cout << duration.count() << " milliseconds" << std::endl;

  return 0;
}
