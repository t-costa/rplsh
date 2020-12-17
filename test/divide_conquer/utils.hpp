#ifndef utils_hpp
#define utils_hpp

#include <math.h>
#include <random>
#include <chrono>
#include <vector>
#include <iostream>


namespace utils {

  //typedef double size_type ;
  typedef int elem_type;
  typedef std::vector<elem_type> vec;
  typedef std::pair<elem_type, elem_type> pair;
  typedef std::vector<pair> vec_pair;

  uint seed = 1;

  void init_random() {
    srand(0);
  }

  template <typename Iterator>
  void init(Iterator begin, Iterator end) {
  #ifdef DEBUG
<<<<<<< HEAD
      elem_type lower_bound = -9;
      elem_type upper_bound = 9;
  #else
      elem_type lower_bound = -99999;
      elem_type upper_bound = 99999;
=======
      elem_type lower_bound = 1;
      elem_type upper_bound = 10;
  #else
      elem_type lower_bound = -999999;
      elem_type upper_bound = 999999;
>>>>>>> e9dce9cf3051f15f46492d03c728495952c63d13
  #endif
      std::uniform_real_distribution<double> unif(lower_bound,upper_bound);
      std::default_random_engine re(seed);
      seed++;
      while (begin != end) {
          (*begin++) = unif(re);
      }
  }

<<<<<<< HEAD
  template <typename Iterator>
  void initFib(Iterator begin, Iterator end) {
    #ifdef DEBUG
        elem_type lower_bound = 1;
        elem_type upper_bound = 10;
    #else
        elem_type lower_bound = 20;
        elem_type upper_bound = 25;
    #endif
        std::uniform_real_distribution<double> unif(lower_bound,upper_bound);
        std::default_random_engine re(seed);
        seed++;
        while (begin != end) {
            (*begin++) = unif(re);
        }
  }

=======
>>>>>>> e9dce9cf3051f15f46492d03c728495952c63d13
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
          std::cout << vp[i].first << "/" << vp[i].second << " ";
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
  void write(const T& name, const std::string& file_name) {

    std::ofstream output_file(file_name, std::ios_base::app);
    std::ostream_iterator<std::string> output_iterator(output_file, "\n");

    output_file << name << "\n";
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

<<<<<<< HEAD
  struct range {
      std::vector<utils::elem_type>::iterator left{};
      std::vector<utils::elem_type>::iterator right{};

      range() = default;

      range(std::vector<utils::elem_type>::iterator _left, std::vector<utils::elem_type>::iterator _right) : left(_left), right(_right) {}
  };

=======
>>>>>>> e9dce9cf3051f15f46492d03c728495952c63d13
}

#endif
