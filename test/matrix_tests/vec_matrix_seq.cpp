//create a matrix from a vector using a map stage
#include <iostream>
#include <vector>

#include "../definition.hpp"

int main() {
  map_vec_matrix_stage a;
  source_vec_stage source;
  drain_matrix_stage drain;

  auto start = std::chrono::high_resolution_clock::now();

  while (source.has_next()) {
    auto vs = source.next();
    auto m = new matrix<utils::elem_type>();
    *m = a.compute(*vs);
    delete vs;
    drain.process(m);
  }

  auto end = std::chrono::high_resolution_clock::now();
  auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end-start);
  std::cout << duration.count() << " milliseconds" << std::endl;

  return 0;
}
