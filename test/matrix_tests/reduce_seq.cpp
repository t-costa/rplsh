//reduce matrix (each row becomes the sum of its elements)
#include <iostream>
#include <vector>

#include "../definition.hpp"

int main() {
  reduce_matrix_stage a;
  source_matrix_stage source;
  drain_vec_stage drain;

  auto start = std::chrono::high_resolution_clock::now();

  while (source.has_next()) {
    auto m = source.next();
    auto v = new std::vector<elem_type>();
    *v = a.compute(*m);
    delete m;
    drain.process(v);
  }

  auto end = std::chrono::high_resolution_clock::now();
  auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end-start);
  std::cout << duration.count() << " milliseconds" << std::endl;

  return 0;
}
