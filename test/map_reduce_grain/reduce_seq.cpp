#include <iostream>
#include <vector>
#include <chrono>

#include "seq_definition.hpp"

int main() {
  source_vec_stage s;
  reduce_stage r;
  drain_stage d;

  auto start = std::chrono::high_resolution_clock::now();

  while (s.has_next()) {
    auto v = s.next();
    auto res = new utils::elem_type(r.compute(*v));
    d.process(res);
  }

  auto end = std::chrono::high_resolution_clock::now();
  auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end-start);
  std::cout << duration.count() << " milliseconds" << std::endl;
}
