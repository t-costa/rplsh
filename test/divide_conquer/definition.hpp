#include <iostream>
#include <vector>
#include <fstream>
#include <iterator>
<<<<<<< HEAD
#include <algorithm>
=======
>>>>>>> e9dce9cf3051f15f46492d03c728495952c63d13

#include "aux/wrappers.hpp"
#include "aux/aux.hpp"

#include "parameters.hpp"
#include "utils.hpp"

////////////////////////////////////////////////////////////////////////////////
<<<<<<< HEAD
//TODO: source_matrix_stage

=======
>>>>>>> e9dce9cf3051f15f46492d03c728495952c63d13
struct source_vecpair_stage : public source<utils::vec_pair> {
public:
  source_vecpair_stage() : i(0) {
    utils::init_random();
  }

  bool has_next() {
    return i++ < parameters::dimension;
  }

  utils::vec_pair* next() {
    std::vector<utils::elem_type> a(parameters::inputsize);
    std::vector<utils::elem_type> b(parameters::inputsize);

    utils::init(a.begin(), a.end());
    utils::init(b.begin(), b.end());

    //TODO: potrei farlo shared?
    auto vp = new utils::vec_pair();

    for (size_t i=0; i<a.size(); ++i) {
      vp->push_back(std::make_pair(a[i], b[i]));
    }

  #ifdef DEBUG
  std::cout << "[source_vecpair_stage] result: ";
  utils::print_vec_pair(*vp);
  #endif

    return vp;
  }

private:
  int i;
};

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

    utils::waste(100*parameters::minimum_wait);

    return v;
  }

private:
  int i;
};

<<<<<<< HEAD
struct source_range_stage : public source<utils::range> {
public:
  source_range_stage() : i(0) {
    utils::init_random();
  }

  bool has_next() {
    return i++ < parameters::dimension;
  }

  utils::range* next() {
    auto v = new std::vector<utils::elem_type>(parameters::inputsize);
    utils::init(v->begin(), v->end());

#ifdef DEBUG
std::cout << "[source_range_stage] result: ";
utils::print_vec(*v);
#endif

    auto r = new utils::range(v->begin(), v->end());
    return r;
  }

private:
  int i;
};

=======
>>>>>>> e9dce9cf3051f15f46492d03c728495952c63d13
struct source_stage : public source<utils::elem_type> {
public:
  source_stage() : i(0) {
    utils::init_random();
  }

  bool has_next() {
    return i++ < parameters::dimension;
  }

  utils::elem_type* next() {
    //lo faccio come vettore di un elemento giusto per tenermi la generazione
    //casuale già fatta
    auto v = new std::vector<utils::elem_type>(1);
<<<<<<< HEAD
    utils::initFib(v->begin(), v->end());
=======
    utils::init(v->begin(), v->end());
>>>>>>> e9dce9cf3051f15f46492d03c728495952c63d13

#ifdef DEBUG
std::cout << "[source_vec_stage] result: ";
utils::print_vec(*v);
#endif

<<<<<<< HEAD
=======
    utils::waste(100*parameters::minimum_wait);

>>>>>>> e9dce9cf3051f15f46492d03c728495952c63d13
    auto out = new utils::elem_type((*v)[0]);
    delete v;
    return out;
  }

private:
  int i;
};
////////////////////////////////////////////////////////////////////////////////


////////////////////////////////////////////////////////////////////////////////
<<<<<<< HEAD
//TODO: drain_matrix_stage

=======
>>>>>>> e9dce9cf3051f15f46492d03c728495952c63d13
struct drain_vecpair_stage : public drain<utils::vec_pair> {
public:
  void process(utils::vec_pair* in) {
    std::string file_name;
    if (parameters::sequential) {
      file_name = "./res_seq.txt";
    } else {
      file_name = "./res_ff.txt";
    }

#ifdef DEBUG
std::cout << "[drain_vec_pair_stage] result: ";
utils::print_vec_pair(*in);
#endif

    std::ofstream output_file(file_name, std::ios_base::app);
    std::ostream_iterator<std::string> output_iterator(output_file, "\n");
    // std::copy(in->begin(), in->end(), output_iterator);

    output_file << "\n";
    for (auto el : *in) {
      output_file << el.first << "-" << el.second << " ";
    }
    output_file << "\n";

    delete in;
  }
};

struct drain_vec_stage : public drain<std::vector<utils::elem_type>> {
public:
  void process(std::vector<utils::elem_type>* in) {
    std::string file_name;
    if (parameters::sequential) {
      file_name = "./res_seq.txt";
    } else {
      file_name = "./res_ff.txt";
    }

#ifdef DEBUG
std::cout << "[drain_vec_stage] result: ";
utils::print_vec(*in);
#endif

    std::ofstream output_file(file_name, std::ios_base::app);
    std::ostream_iterator<std::string> output_iterator(output_file, "\n");
    // std::copy(in->begin(), in->end(), output_iterator);

    output_file << "\n";
    for (auto el : *in) {
      output_file << el << " ";
    }
    output_file << "\n";

    delete in;
  }
};

<<<<<<< HEAD
struct drain_range_stage : public drain<utils::range> {
public:
  void process(utils::range* in) {

#ifdef DEBUG
std::cout << "[drain_range_stage] result: ";
std::vector v(in->left, in->right);
utils::print_vec(v);
#endif

    delete in;
  }
};

=======
>>>>>>> e9dce9cf3051f15f46492d03c728495952c63d13
struct drain_stage : public drain<utils::elem_type> {
public:
  void process(utils::elem_type* in) {
    std::string file_name;
    if (parameters::sequential) {
      file_name = "./res_seq.txt";
    } else {
      file_name = "./res_ff.txt";
    }

#ifdef DEBUG
std::cout << "[drain_stage] result: ";
std::cout << (*in) << std::endl;
#endif
    std::ofstream output_file(file_name, std::ios_base::app);
    output_file << "\n";
    output_file << *in << "\n";

    delete in;
  }
};
////////////////////////////////////////////////////////////////////////////////


////////////////////////////////////////////////////////////////////////////////

//fibonacci
<<<<<<< HEAD
struct dc_fibonacci : public dc_stage_wrapper<utils::elem_type, utils::elem_type> {
public:
=======
struct dc_double_double_stage : public dc_stage_wrapper<utils::elem_type, utils::elem_type> {
public:
  explicit dc_double_double_stage() {}

>>>>>>> e9dce9cf3051f15f46492d03c728495952c63d13
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

<<<<<<< HEAD
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


//TODO: strassen


=======
>>>>>>> e9dce9cf3051f15f46492d03c728495952c63d13
//ogni elemento + 2
struct seq_vec_vec_stage : public seq_wrapper<std::vector<utils::elem_type>, std::vector<utils::elem_type>> {
public:
  explicit seq_vec_vec_stage() {}

  std::vector<utils::elem_type> compute(std::vector<utils::elem_type>& in) override {
    std::vector<utils::elem_type> out;
    out.reserve(in.size());

    for (size_t i=0; i<in.size(); ++i) {
      out.emplace_back(in[i]+2);
    }

    //stessa attesa di versione map
    utils::waste(3*in.size()*parameters::minimum_wait);

#ifdef DEBUG
std::cout << "[seq_vec_vec_stage] compute: ";
utils::print_vec(out);
#endif

    return out;
  }
};

//ogni elemento + 2 (uguale a seq, ma per map)
struct map_vec_vec_stage : public map_stage_wrapper<std::vector<utils::elem_type>, std::vector<utils::elem_type>, utils::elem_type, utils::elem_type> {
public:
  explicit map_vec_vec_stage() {}

  std::vector<utils::elem_type> compute(std::vector<utils::elem_type>& in) override {
    std::vector<utils::elem_type> out;
    out.reserve(in.size());

    for (auto& el: in) {
      out.emplace_back(op(el));
    }

#ifdef DEBUG
std::cout << "[map_vec_vec_stage] compute: ";
utils::print_vec(out);
#endif

    return out;
  }

  utils::elem_type op(const utils::elem_type& in) override {
    utils::waste(3*parameters::minimum_wait);
    return in+2;
  }
};

//somma di tutti gli elementi
struct reduce_vec_double_stage : public reduce_stage_wrapper<std::vector<utils::elem_type>, utils::elem_type, utils::elem_type, utils::elem_type> {
public:
  utils::elem_type identity = 0;

  reduce_vec_double_stage() {}

  utils::elem_type compute(std::vector<utils::elem_type>& in) override {
    utils::elem_type out = identity;

    for (auto& el : in) {
      out = op(out, el);
    }

#ifdef DEBUG
std::cout << "[reduce_vec_double_stage] compute: ";
std::cout << out << std::endl;
#endif

    return out;
  }

  utils::elem_type op(utils::elem_type& a, utils::elem_type& b) override {
    utils::waste(4*parameters::minimum_wait);
    return a + b;
  }
};

//ogni coppia diventa la sua differenze
struct map_vecpair_vec_stage : public map_stage_wrapper<utils::vec_pair, std::vector<utils::elem_type>, utils::pair, utils::elem_type> {
public:
  explicit map_vecpair_vec_stage() {}

  std::vector<utils::elem_type> compute(utils::vec_pair& in) override {
    std::vector<utils::elem_type> out;
    out.reserve(in.size());

    for (size_t i=0; i<in.size(); ++i) {
      out.emplace_back(op(in[i]));
    }

#ifdef DEBUG
std::cout << "[map_vecpair_vec_stage] compute: ";
utils::print_vec(out);
#endif

    return out;
  }

  utils::elem_type op(const std::pair<utils::elem_type, utils::elem_type>& in) override {
    utils::waste(2*parameters::minimum_wait);
    return in.first - in.second;
  }
};

//inverte le coppie nell'array in input
struct map_vecpair_vecpair_stage : public map_stage_wrapper<utils::vec_pair, utils::vec_pair, utils::pair, utils::pair> {
public:
  explicit map_vecpair_vecpair_stage() {}

  utils::vec_pair compute(utils::vec_pair& in) override {
    utils::vec_pair out;
    out.reserve(in.size());

    for (size_t i=0; i<in.size(); ++i) {
      auto el = in[i];
      out.push_back(op(el));
    }

#ifdef DEBUG
std::cout << "[map_vecpair_vecpair_stage] compute: ";
utils::print_vec_pair(out);
#endif

    return out;
  }

  std::pair<utils::elem_type, utils::elem_type> op(const std::pair<utils::elem_type, utils::elem_type>& in) override {
    utils::waste(parameters::minimum_wait);
    return std::make_pair(in.second, in.first);
  }
};
////////////////////////////////////////////////////////////////////////////////
