//mergesort
#include <iostream>
#include <vector>

#include "../definition.hpp"

int main() {
  dc_mergesort a;
  source_range_stage source;
  drain_range_stage drain;

  auto start = std::chrono::high_resolution_clock::now();

  while (source.has_next()) {
    auto vs = source.next();
    *vs = a.compute(*vs);
    drain.process(vs);
  }

  auto end = std::chrono::high_resolution_clock::now();
  auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end-start);
  std::cout << duration.count() << " milliseconds" << std::endl;

  return 0;
}
