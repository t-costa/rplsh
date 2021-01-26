#include <iostream>
#include <vector>
#include <fstream>
#include <iterator>
#include <algorithm>
#include <math.h>
#include <mutex>
#include <random>
#include <chrono>


#include "../aux/wrappers.hpp"
#include "../aux/types.hpp"
#include "../aux/aux.hpp"


namespace utils {

  //typedef double size_type ;
  typedef int elem_type;
  typedef std::vector<elem_type> vec;
  typedef std::pair<elem_type, elem_type> pair;

  uint seed = 1;

  void init_random() {
    srand(0);
  }

  template <typename Iterator>
  void init(Iterator begin, Iterator end) {
  #ifdef DEBUG
      elem_type lower_bound = 0;
      elem_type upper_bound = 9;
  #else
      elem_type lower_bound = -99999;
      elem_type upper_bound = 99999;
  #endif
      std::uniform_real_distribution<double> unif(lower_bound,upper_bound);
      std::default_random_engine re(seed);
      seed++;
      while (begin != end) {
          (*begin++) = unif(re);
      }
  }

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

  template <typename Iterator>
  void init_unbalanced(Iterator begin, Iterator end) {
      int i = 0;
      auto size = end-begin;
      while (i < size / 2) {
            (*begin++) = 10;
            i++;
      }
      while (begin != end) {
        (*begin++) = 0;
      }
      //voglio qualcosa del tipo:
      //[a, b, c, ... k, 0, ... 0]
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

  void print_vec(const vec& v) {
      for (auto el : v) {
          std::cout << el << " ";
      }
      std::cout << std::endl;
  }

  struct range {
      std::vector<utils::elem_type>::iterator left{};
      std::vector<utils::elem_type>::iterator right{};

      range() = default;

      range(std::vector<utils::elem_type>::iterator _left, std::vector<utils::elem_type>::iterator _right) : left(_left), right(_right) {}
  };
}

namespace parameters {
#ifdef DEBUG
  const int dimension = 1;
  const size_t inputsize = 4;
  const size_t matrix_size = 4;
  const int cut_off = 1;
#else
  const int dimension = 2;
  const size_t inputsize = 500;
  const size_t matrix_size = 512;
  const int cut_off = 200;
#endif
  const int minimum_wait = 100;
  bool sequential = true;
}

using namespace utils;

////////////////////////////////////////////////////////////////////////////////
struct source_vec : public source<std::vector<utils::elem_type>> {
public:
  source_vec() : i(0) {
    utils::init_random();
  }

  bool has_next() {
    return i++ < parameters::dimension;
  }

  std::vector<utils::elem_type>* next() {
    auto v = new std::vector<utils::elem_type>(parameters::inputsize);
    utils::init(v->begin(), v->end());

#ifdef DEBUG
std::cout << "[source_vec_stage] result:\n";
utils::print_vec(*v);
#endif

    // utils::waste(parameters::minimum_wait);

    return v;
  }

private:
  int i;
};

struct source_ordered_vec : public source<std::vector<elem_type>> {
public:
  source_ordered_vec() : i(0) { }

  bool has_next() {
    return i++ < parameters::dimension;
  }

  std::vector<utils::elem_type>* next() {
    auto v = new std::vector<utils::elem_type>(parameters::inputsize);
    for (size_t i=0; i<parameters::inputsize; ++i) {
      (*v)[i] = i;
    }

#ifdef DEBUG
std::cout << "[source_vec_stage] result:\n";
utils::print_vec(*v);
#endif

    utils::waste(parameters::minimum_wait);

    return v;
  }

private:
  int i;

};

struct source_range : public source<utils::range> {
public:
  source_range() : i(0) {
    utils::init_random();
  }

  bool has_next() {
    return i++ < parameters::dimension;
  }

  utils::range* next() {
    auto v = new std::vector<utils::elem_type>(parameters::inputsize);
    utils::init(v->begin(), v->end());

#ifdef DEBUG
std::cout << "[source_range_stage] result:\n";
utils::print_vec(*v);
#endif

    auto r = new utils::range(v->begin(), v->end());
    return r;
  }

private:
  int i;
};

struct source_elem : public source<utils::elem_type> {
public:
  source_elem() : i(0) {
    utils::init_random();
  }

  bool has_next() {
    return i++ < parameters::dimension;
  }

  utils::elem_type* next() {
    //lo faccio come vettore di un elemento giusto per tenermi la generazione
    //casuale già fatta
    auto v = new std::vector<utils::elem_type>(1);
    utils::initFib(v->begin(), v->end());

#ifdef DEBUG
std::cout << "[source_vec_stage] result:\n";
utils::print_vec(*v);
#endif

    auto out = new utils::elem_type((*v)[0]);
    delete v;
    return out;
  }

private:
  int i;
};
////////////////////////////////////////////////////////////////////////////////


////////////////////////////////////////////////////////////////////////////////
struct drain_vec : public drain<std::vector<utils::elem_type>> {
public:
  void process(std::vector<utils::elem_type>* in) {

#ifdef DEBUG
std::cout << "[drain_vec_stage] result:\n";
utils::print_vec(*in);
#endif

    delete in;
  }
};

struct drain_ordered_vec : public drain<std::vector<utils::elem_type>> {
public:
  void process(std::vector<utils::elem_type>* in) {

#ifdef DEBUG
std::cout << "[drain_vec_stage] result:\n";
utils::print_vec(*in);
#endif

    // bool ordered = true;
    for (size_t i=0; i<in->size()-1; ++i) {
      if ((*in)[i] > (*in)[i+1]) {
        std::cout << "NOT ORDERED!" << std::endl;
        std::cout << "v[" << i << "] = " << (*in)[i] << ";";
        std::cout << "v[" << i+1 << "] = " << (*in)[i+1] << std::endl;
        // ordered = false;
      }
    }

    //print_vec(*in);

    // if (ordered) {
    //   std::cout << "OK!" << std::endl;
    // }

    delete in;
  }

};

struct drain_range : public drain<utils::range> {
public:
  void process(utils::range* in) {

#ifdef DEBUG
std::cout << "[drain_range_stage] result:\n";
std::vector v(in->left, in->right);
utils::print_vec(v);
#endif

    delete in;
  }
};

struct drain_elem : public drain<utils::elem_type> {
public:
  void process(utils::elem_type* in) {

#ifdef DEBUG
std::cout << "[drain_stage] result:\n";
std::cout << (*in) << std::endl;
#endif

    delete in;
  }
};
////////////////////////////////////////////////////////////////////////////////


////////////////////////////////////////////////////////////////////////////////
//ogni elemento + 2
struct seq_vec_vec : public seq_wrapper<std::vector<utils::elem_type>, std::vector<utils::elem_type>> {
public:
  explicit seq_vec_vec() {}

  std::vector<utils::elem_type> compute(std::vector<utils::elem_type>& in) override {
    utils::waste(3*parameters::minimum_wait);
    std::vector<utils::elem_type> out;
    out.reserve(in.size());

    for (size_t i=0; i<in.size(); ++i) {
      out.emplace_back(in[i]+2);
    }


#ifdef DEBUG
std::cout << "[seq_vec_vec_stage] compute:\n";
utils::print_vec(out);
#endif

    return out;
  }
};

//ogni elemento + 2 (uguale a seq, ma per map)
struct map_vec_vec : public map_stage_wrapper<std::vector<utils::elem_type>, std::vector<utils::elem_type>, utils::elem_type, utils::elem_type> {
public:
  explicit map_vec_vec() {}

  std::vector<utils::elem_type> compute(std::vector<utils::elem_type>& in) override {
    std::vector<utils::elem_type> out;
    out.reserve(in.size());

    for (auto& el: in) {
      out.emplace_back(op(el));
    }

// #ifdef DEBUG
// std::cout << "[map_vec_vec_stage] compute:\n";
// utils::print_vec(out);
// #endif

    return out;
  }

  utils::elem_type op(const utils::elem_type& in) override {
    utils::waste(parameters::minimum_wait);
    return in+2;
  }
};

//ogni elemento decrementato di uno, waste proporzionale al valore dell'elemento
struct map_minus : public map_stage_wrapper<std::vector<utils::elem_type>, std::vector<utils::elem_type>, utils::elem_type, utils::elem_type> {
public:
  explicit map_minus() {}

  std::vector<utils::elem_type> compute(std::vector<utils::elem_type>& in) override {
    std::vector<utils::elem_type> out;
    out.reserve(in.size());

    for (auto& el: in) {
      out.emplace_back(op(el));
    }

#ifdef DEBUG
std::cout << "[map_minus_stage] compute:\n";
utils::print_vec(out);
#endif

    return out;
  }

  //valori più grandi => attese maggiori
  utils::elem_type op(const utils::elem_type& in) override {
    utils::waste(in*parameters::minimum_wait);
    return in-1;
  }
};

//somma di tutti gli elementi
struct reduce_vec_double : public reduce_stage_wrapper<std::vector<utils::elem_type>, utils::elem_type> {
public:
  utils::elem_type identity = 0;

  reduce_vec_double() {}

  utils::elem_type compute(std::vector<utils::elem_type>& in) override {
    utils::elem_type out = identity;

    for (auto& el : in) {
      out = op(out, el);
    }

#ifdef DEBUG
std::cout << "[reduce_vec_double_stage] compute:\n";
std::cout << out << std::endl;
#endif

    return out;
  }

  utils::elem_type op(utils::elem_type& a, utils::elem_type& b) override {
    utils::waste(parameters::minimum_wait);
    return a + b;
  }
};

//fibonacci
struct dc_fibonacci : public dc_stage_wrapper<utils::elem_type, utils::elem_type> {
public:
  void divide(const utils::elem_type& in, std::vector<utils::elem_type>& in_vec) override {
    utils::waste(parameters::minimum_wait);
    in_vec.emplace_back(in - 1);
    in_vec.emplace_back(in - 2);
  }

  void combine(std::vector<utils::elem_type>& out_vec, utils::elem_type& out) override {
    utils::waste(parameters::minimum_wait);
    out = out_vec[0] + out_vec[1];
  }

  void seq(const utils::elem_type& in, utils::elem_type& out ) override {
    utils::waste(parameters::minimum_wait);
    out = 1;
  }

  bool cond(const utils::elem_type& in) override {
    return in <= 2;
  }
};

//mergesort
struct dc_mergesort : public dc_stage_wrapper<utils::range, utils::range> {
public:
  explicit dc_mergesort() {}

  void divide(const utils::range& prob, std::vector<utils::range>& subps) override {
      utils::waste(parameters::minimum_wait);
      auto mid = prob.left +(prob.right - prob.left) / 2;
      utils::range a, b;
      a.left = prob.left;
      a.right = mid;
      b.left = mid;
      b.right = prob.right;

      subps.push_back(a);
      subps.push_back(b);
  }

  void combine(std::vector<utils::range>& res, utils::range& ret) override {
      utils::waste(parameters::minimum_wait);
      auto size = (size_t) (res[1].right - res[0].left);

      std::vector<utils::elem_type> tmp(size);
      auto i = res[0].left, j = res[1].left, mid = res[1].left, right = res[1].right;
      //merge in order
      for(size_t k=0;k<size;k++) {
          if(i<mid && (j>=right || *i <= *j)) {
              tmp[k] = *i;
              i++;
          } else {
              tmp[k] = *j;
              j++;
          }
      }
      //copy back
      std::copy(tmp.begin(), tmp.end(), res[0].left);
      ret.left = res[0].left;
      ret.right = res[1].right;

  }

  void seq(const utils::range& p, utils::range& res) override {
      utils::waste(parameters::minimum_wait);
      std::sort(p.left, p.right);
      res.left = p.left;
      res.right = p.right;
  }

  bool cond(const utils::range& p) override {
      utils::waste(parameters::minimum_wait);
      return (p.right - p.left <= parameters::cut_off);
  }
};

// ogni elemento + 2
struct dc_dummy : public dc_stage_wrapper<std::vector<utils::elem_type>, std::vector<utils::elem_type>> {
public:
  explicit dc_dummy() {}

  void divide(const std::vector<elem_type>& in, std::vector<std::vector<elem_type>>& in_vec) override {
    size_t schedule = 2;
    auto new_size = in.size() / schedule;
    in_vec.resize(schedule);
    size_t j = 0;
    for (size_t i=0; i<in.size(); ++i) {
      if (i >= (j+1)*new_size && j<schedule-1)
        j++;
      in_vec[j].push_back(in[i]);
    }
  }

  void combine(std::vector<std::vector<elem_type>>& out_vec, std::vector<elem_type>& out) override {
    size_t schedule = 2;
    size_t final_size = 0;
    //compute size of out vector
    for (size_t k=0; k<schedule; ++k) {
      final_size += out_vec[k].size();
    }
    out.resize(final_size);
    //combine results
    size_t i = 0, j = 0;
    for (j=0; j<schedule; ++j) {
      for(auto& a : out_vec[j]) {
        out[i] = a;
        i++;
      }
    }
  }

  void seq(const std::vector<elem_type>& in, std::vector<elem_type>& out) override {
    waste(parameters::minimum_wait);
    out.push_back(in[0] + 2);
  }

  bool cond(const std::vector<elem_type>& in) override {
    return in.size() <= 1;
  }
};
////////////////////////////////////////////////////////////////////////////////
