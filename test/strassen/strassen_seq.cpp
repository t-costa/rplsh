//strassen
#include <iostream>
#include <vector>

#include "../definition.hpp"

int main() {
  dc_strassen a;
  source_strassen source;
  drain_strassen drain;

  auto start = std::chrono::high_resolution_clock::now();
  while (source.has_next()) {
    auto mm = source.next();
    auto m = new utils::Result(a.compute(*mm));
    drain.process(m);
  }

  auto end = std::chrono::high_resolution_clock::now();
  auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end-start);
  std::cout << duration.count() << " milliseconds" << std::endl;
  return 0;
}
