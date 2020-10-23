#ifndef utils_hpp
#define utils_hpp

#include <math.h>
#include <random>
#include <chrono>
#include <vector>
#include <iostream>


namespace utils {

  typedef double size_type ;
  typedef std::vector<size_type> vec;
  typedef std::vector<std::pair<double, double>> vec_pair;

  uint seed = 0;

//   struct vec_pair {
//     vec_pair(size_t size) {
//       A.reserve(size);
//       B.reserve(size);
//     }
//
//     vec_pair(const vec& A, const vec& B) : A(A), B(B) {}
//
//     void push_back(double& a, double& b) {
//       A.push_back(a);
//       B.push_back(b);
//     }
//
//     void push_back(std::pair<double, double> ab) {
//       A.push_back(ab.first);
//       B.push_back(ab.second);
//     }
//
//     std::pair<double, double> operator[] (int x) {
//         return std::make_pair(A[x], B[x]);
//     }
//
//     std::size_t size() {
//         return A.size();
//     }
//
// private:
//     vec A;
//     vec B;
//   };

  void init_random() {
    srand(0);
  }

  template <typename Iterator>
  void init(Iterator begin, Iterator end) {
  #ifdef DEBUG
      double lower_bound = -9;
      double upper_bound = 9;
  #else
      double lower_bound = -999999;
      double upper_bound = 999999;
  #endif
      std::uniform_real_distribution<double> unif(lower_bound,upper_bound);
      std::default_random_engine re(seed);
      seed++;
      while (begin != end) {
          (*begin++) = unif(re);
      }
  }

  inline void waste(const double& microsec) {
      auto t1 = std::chrono::high_resolution_clock::now();
      auto t2 = std::chrono::high_resolution_clock::now();

      auto elapsed = t2 - t1;

      while (std::chrono::duration_cast<std::chrono::microseconds>(elapsed).count() < microsec) {
          t2 = std::chrono::high_resolution_clock::now();
          elapsed = t2 - t1;
      }
  }

  void print_vec_pair(vec_pair& vp) {
      for (size_t i=0; i<vp.size(); ++i) {
          std::cout << vp[i].first << "-" << vp[i].second << " ";
      }
      std::cout << std::endl;
  }

  void print_vec(const vec& v) {
      for (auto el : v) {
          std::cout << el << " ";
      }
      std::cout << std::endl;
  }

  template <typename T>
  bool test_equality(const T& v1, const T& v2) {
      if (v1.size() != v2.size()) {
          std::cout << "Vectors have different sizes!" << std::endl;
          std::cout << "Sizes: " << v1.size() << " and " << v2.size() << std::endl;
          return false;
      }

      for (size_t i=0; i<v1.size(); ++i) {
          if (v1[i] != v2[i]) {
              std::cout << "Vectors differ at index " << i << std::endl;
              std::cout << "Values: " << v1[i] << " and " << v2[i] << std::endl;
              return false;
          }
      }

      return true;
  }

  template <typename T>
  struct result_store {
    static std::vector<T> res;

    static void add_result(const T& r) {
        res.emplace_back(r);
    }

    static bool check(int i, const T& r) {
        return test_equality(res[i], r);
    }

    void check_store() {
        for (auto& v : res) {
            print_vec(v);
        }
    }
  };

}

#endif
