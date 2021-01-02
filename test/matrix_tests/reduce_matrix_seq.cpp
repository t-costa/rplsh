//matrix multiplication
#include <iostream>
#include <vector>

#include "../definition.hpp"

int main() {
  reduce_matrix_stage b;
  source_matrix3d_stage source;
  drain_matrix_stage drain;


  auto start = std::chrono::high_resolution_clock::now();

  while (source.has_next()) {
    auto res = new matrix();
    auto m = source.next();
    *res = b.compute(*m);
    delete m;
    drain.process(res);
  }

  auto end = std::chrono::high_resolution_clock::now();
  auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end-start);
  std::cout << duration.count() << " milliseconds" << std::endl;

  return 0;
}
