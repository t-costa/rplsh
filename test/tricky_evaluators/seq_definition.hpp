#include <iostream>
#include <vector>
#include <fstream>
#include <iterator>

#include "aux/wrappers.hpp"
#include "aux/aux.hpp"

#include "parameters.hpp"
#include "utils.hpp"

////////////////////////////////////////////////////////////////////////////////
struct source_vec_stage : public source<std::vector<utils::elem_type>> {
public:
  source_vec_stage() : i(0) {
    utils::init_random();
  }

  bool has_next() {
    return i++ < parameters::dimension;
  }

  std::vector<utils::elem_type>* next() {
    auto v = new std::vector<utils::elem_type>(parameters::inputsize);
    utils::init(v->begin(), v->end());

#ifdef DEBUG
std::cout << "[source_vec_stage] result: ";
utils::print_vec(*v);
#endif

    return v;
  }

private:
  int i;
};

struct source_unbalanced_vec_stage: public source <std::vector<utils::elem_type>> {
public:
  source_unbalanced_vec_stage() : i(0) {
    utils::init_random();
  }

  bool has_next() {
    return i++ < parameters::dimension;
  }

  std::vector<utils::elem_type>* next() {
    auto v = new std::vector<utils::elem_type>(parameters::inputsize);
    utils::init_unbalanced(v->begin(), v->end());

#ifdef DEBUG
std::cout << "[source_unbalanced_vec_stage] result: ";
utils::print_vec(*v);
#endif

    return v;
  }

private:
  int i;
};
////////////////////////////////////////////////////////////////////////////////


////////////////////////////////////////////////////////////////////////////////
struct drain_vec_stage : public drain<std::vector<utils::elem_type>> {
public:
  void process(std::vector<utils::elem_type>* in) {
#ifdef DEBUG
std::cout << "[drain_vec_stage] result: ";
utils::print_vec(*in);
#endif

    delete in;
  }
};

struct drain_stage : public drain<utils::elem_type> {
public:
  void process(utils::elem_type* in) {
#ifdef DEBUG
std::cout << "[drain_stage] result: ";
std::cout << (*in) << std::endl;
#endif

    delete in;
  }
};
////////////////////////////////////////////////////////////////////////////////


////////////////////////////////////////////////////////////////////////////////
//ogni elemento + 2
struct map_plus_stage : public map_stage_wrapper<std::vector<utils::elem_type>, std::vector<utils::elem_type>, utils::elem_type, utils::elem_type> {
public:
  explicit map_plus_stage() {}

  std::vector<utils::elem_type> compute(std::vector<utils::elem_type>& in) override {
    std::vector<utils::elem_type> out;
    out.reserve(in.size());

    for (auto& el: in) {
      out.emplace_back(op(el));
    }

#ifdef DEBUG
std::cout << "[map_plus_stage] compute: ";
utils::print_vec(out);
#endif

    return out;
  }

  utils::elem_type op(const utils::elem_type& in) override {
    utils::waste(3*parameters::minimum_wait);
    return in+2;
  }
};


struct map_minus_stage : public map_stage_wrapper<std::vector<utils::elem_type>, std::vector<utils::elem_type>, utils::elem_type, utils::elem_type> {
public:
  explicit map_minus_stage() {}

  std::vector<utils::elem_type> compute(std::vector<utils::elem_type>& in) override {
    std::vector<utils::elem_type> out;
    out.reserve(in.size());

    for (auto& el: in) {
      out.emplace_back(op(el));
    }

#ifdef DEBUG
std::cout << "[map_minus_stage] compute: ";
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

struct reduce_stage : public reduce_stage_wrapper<std::vector<utils::elem_type>, utils::elem_type, utils::elem_type, utils::elem_type> {
public:
  utils::elem_type identity = 0;

  reduce_stage() {}

  utils::elem_type compute(std::vector<utils::elem_type>& in) override {
    utils::elem_type out = identity;

    for (auto& el : in) {
      out = op(out, el);
    }

#ifdef DEBUG
std::cout << "[reduce_stage] compute: ";
std::cout << out << std::endl;
#endif

    return out;
  }

  utils::elem_type op(utils::elem_type& a, utils::elem_type& b) override {
    utils::waste(100*parameters::minimum_wait);
    return a + b;
  }
};
//aggiungi qualcosa tipo power list per vedere differenze in performance
////////////////////////////////////////////////////////////////////////////////
