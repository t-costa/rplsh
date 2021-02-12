//matrix mul, like a stream
#include <iostream>
#include <vector>

#include "../definition.hpp"

int main() {
  source_matrixpair_stage s;
  drain_matrix_stage d;
  map_matrix_mul_stage m;

  auto start = std::chrono::high_resolution_clock::now();

  while (s.has_next()) {
    auto mm = s.next();
    auto ms = new matrix(m.compute(*mm));
    d.process(ms);
  }

  auto end = std::chrono::high_resolution_clock::now();
  auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end-start);
  std::cout << duration.count() << " milliseconds" << std::endl;

  return 0;
}
