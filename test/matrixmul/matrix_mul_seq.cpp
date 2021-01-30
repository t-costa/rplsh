//matrix mul, like a stream
#include <iostream>
#include <vector>

#include "../definition.hpp"

int main() {
  source_matrix_vectors s;
  drain_matrixelem d;
  map_vec_prod m;
  red_sum r;

  auto start = std::chrono::high_resolution_clock::now();

  while (s.has_next()) {
    auto mvv = s.next();
    auto vv = m.compute(*mvv);
    auto el = new elem_idx_idx(r.compute(vv));
    d.process(el);
  }

  auto end = std::chrono::high_resolution_clock::now();
  auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end-start);
  std::cout << duration.count() << " milliseconds" << std::endl;

  return 0;
}
