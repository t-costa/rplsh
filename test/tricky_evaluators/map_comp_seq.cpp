#include <iostream>
#include <vector>
#include <chrono>

#include "../definition.hpp"

int main() {
  source_unbalanced_vec_stage s;
  map_minus_stage m1;
  map_plus_stage m2;
  drain_vec_stage d;

  auto start = std::chrono::high_resolution_clock::now();

  while (s.has_next()) {
    auto v = s.next();
    (*v) = m1.compute(*v);
    (*v) = m2.compute(*v);
    d.process(v);
  }

    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end-start);
    std::cout << duration.count() << " milliseconds" << std::endl;
}
