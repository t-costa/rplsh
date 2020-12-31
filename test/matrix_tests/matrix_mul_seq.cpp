//matrix multiplication
#include <iostream>
#include <vector>

#include "../definition.hpp"

int main() {
  map_prod a;
  reduce_matrix_stage b;
  source_matrixpair_stage source;
  drain_matrix_stage drain;


  auto start = std::chrono::high_resolution_clock::now();

  while (source.has_next()) {
    auto m = new matrix_3d();
    auto res = new matrix();
    auto mc = source.next();
    *m = a.compute(*mc);
    *res = b.compute(*m);
    delete mc;
    delete m;
    drain.process(res);
  }

  auto end = std::chrono::high_resolution_clock::now();
  auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end-start);
  std::cout << duration.count() << " milliseconds" << std::endl;

  return 0;
}
