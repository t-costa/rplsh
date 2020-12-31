#include <iostream>
#include <vector>
#include <fstream>
#include <iterator>
#include <algorithm>
#include <math.h>

#include "../aux/wrappers.hpp"
#include "../aux/types.hpp"
#include "../aux/aux.hpp"

#include "parameters.hpp"
#include "utils.hpp"

using namespace utils;

////////////////////////////////////////////////////////////////////////////////
struct source_matrix_stage : public source<matrix> {
public:
  source_matrix_stage() : i(0) {
    utils::init_random();
  }

  bool has_next() override {
    return i++ < parameters::dimension;
  }

  matrix* next() {
    auto size = (size_t) (parameters::matrix_size);
    std::vector<utils::elem_type> a(size);
    auto m = new matrix();

    for (size_t j=0; j<size; ++j) {
      utils::init(a.begin(), a.end());
      m->push_back(a);
    }

  #ifdef DEBUG
  std::cout << "[source_matrix_stage] result:\n";
  //TODO: m->print();
  for (auto& v : *m) {
    print_vec(v);
  }
  #endif

    return m;
  }

private:
  int i;
};

struct source_matrixpair_stage : public source<matrix_couple> {
public:
  source_matrixpair_stage() : i(0) {
    utils::init_random();
  }

  bool has_next() override {
    return i++ < parameters::dimension;
  }

  matrix_couple* next() {
    auto size = (size_t) (parameters::matrix_size);
    std::vector<utils::elem_type> a(size);
    // auto m = new matrix();
    matrix m, n;
    // auto n = new matrix();

    for (size_t j=0; j<size; ++j) {
      utils::init(a.begin(), a.end());
      m.push_back(a);
      utils::init(a.begin(), a.end());
      n.push_back(a);
    }

  #ifdef DEBUG
  std::cout << "[source_matrix_stage] result:\n";
std::cout << "------\n";
  std::cout << "first matrix:\n";
  for (auto& v : m) {
    print_vec(v);
  }
  std::cout << "second matrix\n";
  for (auto& v : n) {
    print_vec(v);
  }
  std::cout << "------\n";
  #endif

    auto c = new matrix_couple(m, n);

    return c;
  }

private:
  int i;
};

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
  std::cout << "[source_vecpair_stage] result:\n";
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
std::cout << "[source_vec_stage] result:\n";
utils::print_vec(*v);
#endif

    utils::waste(100*parameters::minimum_wait);

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
std::cout << "[source_unbalanced_vec_stage] result:\n";
utils::print_vec(*v);
#endif

    return v;
  }

private:
  int i;
};

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
std::cout << "[source_range_stage] result:\n";
utils::print_vec(*v);
#endif

    auto r = new utils::range(v->begin(), v->end());
    return r;
  }

private:
  int i;
};

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
struct drain_matrix_stage : public drain<matrix> {
public:
    void process(matrix* in) {

#ifdef DEBUG
std::cout << "[drain_matrix_stage] result:\n";
for (auto& v : *in) {
    print_vec(v);
  }
#endif

      delete in;
    }
};

struct drain_matrix3d_stage :public drain<matrix_3d> {
public:
    void process(matrix_3d* in) {

#ifdef DEBUG
std::cout << "[drain_matrix_stage] result:\n";
for (auto& v : *in) {
  for (auto& v2 : v) {
    print_vec(v2);
  }
}
#endif

      delete in;
    }
};

struct drain_vecpair_stage : public drain<utils::vec_pair> {
public:
  void process(utils::vec_pair* in) {
    // std::string file_name;
    // if (parameters::sequential) {
    //   file_name = "./res_seq.txt";
    // } else {
    //   file_name = "./res_ff.txt";
    // }

#ifdef DEBUG
std::cout << "[drain_vec_pair_stage] result:\n";
utils::print_vec_pair(*in);
#endif

    // std::ofstream output_file(file_name, std::ios_base::app);
    // std::ostream_iterator<std::string> output_iterator(output_file, "\n");
    // // std::copy(in->begin(), in->end(), output_iterator);
    //
    // output_file << "\n";
    // for (auto el : *in) {
    //   output_file << el.first << "-" << el.second << " ";
    // }
    // output_file << "\n";

    delete in;
  }
};

struct drain_vec_stage : public drain<std::vector<utils::elem_type>> {
public:
  void process(std::vector<utils::elem_type>* in) {

#ifdef DEBUG
std::cout << "[drain_vec_stage] result:\n";
utils::print_vec(*in);
#endif

    delete in;
  }
};

struct drain_range_stage : public drain<utils::range> {
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

struct drain_stage : public drain<utils::elem_type> {
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

//quadrato di ogni elemento
struct map_matrix : public map_stage_wrapper<matrix, matrix, std::vector<utils::elem_type>, std::vector<utils::elem_type>> {
public:
  matrix compute(matrix& mn) override {

    matrix res;

    for (size_t i=0; i<mn.size(); ++i) {
      res.push_back(op(mn[i]));
    }

    return res;
  }

  std::vector<utils::elem_type> op(const std::vector<utils::elem_type>& v) override {
    std::vector<utils::elem_type> res(v.size());

    for (size_t i=0; i<v.size(); ++i) {
      res[i] = v[i] * v[i];
    }

    return res;
  }
};

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
std::cout << "[seq_vec_vec_stage] compute:\n";
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
std::cout << "[map_vec_vec_stage] compute:\n";
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
struct reduce_vec_double_stage : public reduce_stage_wrapper<std::vector<utils::elem_type>, utils::elem_type> {
public:
  utils::elem_type identity = 0;

  reduce_vec_double_stage() {}

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
std::cout << "[map_vecpair_vec_stage] compute:\n";
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
std::cout << "[map_vecpair_vecpair_stage] compute:\n";
utils::print_vec_pair(out);
#endif

    return out;
  }

  std::pair<utils::elem_type, utils::elem_type> op(const std::pair<utils::elem_type, utils::elem_type>& in) override {
    utils::waste(parameters::minimum_wait);
    return std::make_pair(in.second, in.first);
  }
};

//per ogni elemento crea un vettore che contiene solo quegli elementi
struct map_vec_matrix_stage : public map_stage_wrapper<std::vector<elem_type>, matrix, elem_type, std::vector<elem_type>> {
public:
  matrix compute(std::vector<elem_type>& in) override {
    matrix out;

    for (size_t i=0; i<in.size(); ++i) {
      out.push_back(op(in[i]));
    }

#ifdef DEBUG
std::cout << "[map_vec_matrix_stage] compute:\n";
for (auto& v : out) {
  print_vec(v);
}
#endif

    return out;
  }

  std::vector<elem_type> op(const elem_type& in) override {
    auto v = std::vector<elem_type>(parameters::inputsize, in);
    return v;
  }
};

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
std::cout << "[map_plus_stage] compute:\n";
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

struct reduce_stage : public reduce_stage_wrapper<std::vector<utils::elem_type>, utils::elem_type> {
public:
  utils::elem_type identity = 0;

  reduce_stage() {}

  utils::elem_type compute(std::vector<utils::elem_type>& in) override {
    utils::elem_type out = identity;

    for (auto& el : in) {
      out = op(out, el);
    }

#ifdef DEBUG
std::cout << "[reduce_stage] compute:\n";
std::cout << out << std::endl;
#endif

    return out;
  }

  utils::elem_type op(utils::elem_type& a, utils::elem_type& b) override {
    utils::waste(4*parameters::minimum_wait);
    return a + b;
  }
};

struct map_prod : public map_stage_wrapper<matrix_couple, matrix_3d, vec_matrix_couple, matrix> {
public:

  explicit map_prod() {}

  matrix_3d compute(matrix_couple& mn) override {
    // auto a = mn.first();
    // auto b = mn.second();

    matrix_3d res;

    for (size_t i=0; i<mn.size(); ++i) {
      res.push_back(op(mn[i]));
    }

// #ifdef DEBUG
// std::cout << "[map_prod] compute:\n";
// // for (size_t i=0; i<res.size(); ++i) {
// //   res[i][0].print();
// // }
// #endif

    return res;
  }

  matrix op(const vec_matrix_couple& vm_pair) override {
    const auto a = vm_pair._vec;
    const auto b = vm_pair._mat;
    matrix res;

    for (size_t i=0; i<a.size(); ++i) {
      auto v = std::vector<elem_type>(a.size());
      res.push_back(v);
    }

    std::cout << "res inizializzato\n";
    for (auto row : res) {
      print_vec(row);
    }
    std::cout << "fine res inizializzato\n";

    vm_pair.print();

    //per ogni elemento del vettore
    for (size_t i=0; i<a.size(); ++i) {
      //recupero riga matrice da moltiplicare
      const auto tmp = b[i];
      //per ogni elemento della riga
      for (size_t j=0; j<tmp.size(); ++j) {
          //faccio tutti i prodotti che servono per a[i]
          //e li metto nella riga giusta
          res[j][i] = a[i] * tmp[j];
      }
    }

    // for (size_t i=0; i<a.size(); ++i) {
    //   const auto tmp = b[i];
    //   std::vector<utils::elem_type> v;
    //
    //   for (size_t j=0; j<tmp.size(); ++j) {
    //     //metto nel vettore temporaneo
    //     v.push_back(a[i] * tmp[j]);
    //   }
    //   //prima riga pronta
    //   res.push_back(v);
    // }

    //vediamo com'è res
    std::cout << "------\n";
    std::cout << "res da op\n";
    for (auto& v : res) {
      utils::print_vec(v);
    }
    std::cout << "------\n";

    return res;
  }
};

struct reduce_matrix_stage : public reduce_stage_wrapper<matrix_3d, matrix> {
public:
  matrix identity;

  reduce_matrix_stage() {
    identity = matrix();
  }

  matrix compute(matrix_3d& in) override {
    auto out = identity;
    //assumo identity (e quindi out) inizia tutto vuoto
    for (size_t i=0; i<in.size(); ++i) {
      out = op(out, in[i]);
    }

    return out;
  }

  matrix op(matrix& a, matrix& b) {
    auto res = 0;
    std::vector<utils::elem_type> res_vec;

    for (size_t i=0; i<b.size(); ++i) {
      //row of the matrix;
      auto tmp = b[i];
      std::cout << "riga " << i << std::endl;
      print_vec(tmp);
      for (size_t j=0; j<tmp.size(); ++j) {
        res += tmp[j];
      }
      //aggiungo un elemento alla riga
      res_vec.push_back(res);
      res = 0;
    }
    //aggiungo la riga alla matrice finale
    a.push_back(res_vec);

        std::cout << "res_vec:\n";
        print_vec(res_vec);
        std::cout << "matrice finale:\n";
        for (size_t i=0; i<a.size(); ++i) {
          print_vec(a[i]);
        }
    return a;
  }

private:
  long row = 0;
};
////////////////////////////////////////////////////////////////////////////////
