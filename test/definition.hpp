#include <iostream>
#include <vector>
#include <fstream>
#include <iterator>
#include <algorithm>
#include <math.h>
#include <mutex>

#include "../aux/wrappers.hpp"
#include "../aux/types.hpp"
#include "../aux/aux.hpp"

#include "parameters.hpp"
#include "utils.hpp"

using namespace utils;

////////////////////////////////////////////////////////////////////////////////
struct source_strassen : public source<utils::Operand>{
public:
  source_strassen() : i(0) {}

  bool has_next() {
    return i++ < parameters::matrix_stream;
  }

  utils::Operand* next() {
    //7 e 24 sono seed
    utils::elem_type *a=generateCompactRandomMatrix(parameters::matrix_size, 7);
    utils::elem_type *b=generateCompactRandomMatrix(parameters::matrix_size, 24);

    auto op = new Operand(a,parameters::matrix_size,parameters::matrix_size,b,parameters::matrix_size,parameters::matrix_size,false,false);

#ifdef DEBUG
std::cout << "[source_strassen]" << std::endl;
op->print();

// std::cout << "print a mano:" << std::endl;
// for(int i=0; i<parameters::matrix_size*parameters::matrix_size; ++i) {
//   std::cout << a[i] << " ";
// }
#endif
    return op;
  }

private:
  int i;
};

struct source_matrix_vectors : public source<std::vector<utils::elem_elem_idx_idx>> {
public:
  source_matrix_vectors() : i(0), j(0), count(0), create(true) {
    utils::init_random();
    total = parameters::inputsize*parameters::inputsize;
  }

  //dimension must be size*size
  bool has_next() override {
    return count++ < parameters::matrix_production;
  }

  //for simplicity, square matrix
  std::vector<utils::elem_elem_idx_idx>* next() {
    if (create) {
      j = 0;
      create = false;
    }
    //create vectors
    auto row = std::vector<utils::elem_type>(parameters::matrix_size);
    auto col = std::vector<utils::elem_type>(parameters::matrix_size);
    //cerco di avere seed diversi, almeno un po'
    // utils::init(row.begin(), row.end(), i*2+1); //seed sempre dispari
    // utils::init(col.begin(), col.end(), j*3+3); //seed sempre pari, tranne 0

    utils::init_fast(row.begin(), row.end(), i*2+1);
    utils::init_fast(col.begin(), col.end(), j*3+3);

    auto out = new std::vector<utils::elem_elem_idx_idx>();
    for (size_t k=0; k<parameters::matrix_size; k++) {
      out->emplace_back(elem_elem_idx_idx(row[k], col[k], i, j));
    }

    // auto out = new utils::vec_vec_idx_idx(row, col, i, j);
    if (count % parameters::matrix_size == 0) {
      //ho creato tutte le colonne, serve nuova riga
      create = true;
      i++;
    } else {
      j++;
    }

    #ifdef DEBUG
    std::cout << "source:" << std::endl;
    for (auto el : *out) {
      std::cout << el.a << " " << el.b << " " << el.i << " " << el.j << std::endl;
    }
    #endif

    return out;
  }

private:
  int i, j, count;
  long total;
  bool create;
};

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
    return i++ < parameters::matrix_stream;
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

struct source_matrix3d_stage : public source<matrix_3d> {
public:
  source_matrix3d_stage() : i(0) {
    utils::init_random();
  }

  bool has_next() override {
    return i++ < parameters::dimension;
  }

  matrix_3d* next() {
    auto size = (size_t) (parameters::matrix_size);
    std::vector<utils::elem_type> a(size);
    auto out = new matrix_3d();

    for (size_t k=0; k<size; ++k) {
      matrix m;
      for (size_t j=0; j<size; ++j) {
        utils::init(a.begin(), a.end());
        m.push_back(a);
      }
      out->push_back(m);
    }


  #ifdef DEBUG
  std::cout << "[source_matrix3d_stage] result:\n";
  //TODO: m->print();
  for (auto& m_in : *out) {
    for (auto& v_in : m_in) {
      print_vec(v_in);
    }
  }
  #endif

    return out;
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

    //utils::waste(parameters::minimum_wait);

    return v;
  }

private:
  int i;
};

struct source_ordered_vec_stage : public source<std::vector<elem_type>> {
public:
  source_ordered_vec_stage() : i(0) { }

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

    //utils::waste(parameters::minimum_wait);

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
struct drain_strassen : public drain<utils::Result> {
public:
  drain_strassen() {}

  void process(utils::Result* in) {
#ifdef DEBUG
std::cout << "[drain strassen]" << std::endl;
in->print();
#endif

    delete in;
  }
};

struct drain_matrixelem : public drain<utils::elem_idx_idx> {
public:
  drain_matrixelem() {
    res = utils::matrix();
    for (size_t i=0; i<parameters::matrix_size; ++i) {
      auto v = std::vector<utils::elem_type>(parameters::matrix_size);
      res.emplace_back(v);
    }
  }

  void process(utils::elem_idx_idx* in) {
    res[in->i][in->j] = in->a;
  #ifdef DEBUG
    std::cout << "riempiendo matrice" << std::endl;
    for (auto v : res) {
      utils::print_vec(v);
    }
  #endif
    delete in;
  }

private:
  utils::matrix res;
};

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

    //utils::waste(parameters::minimum_wait);

#ifdef DEBUG
std::cout << "[drain_vec_stage] result:\n";
utils::print_vec(*in);
#endif

    delete in;
  }
};

struct drain_ordered_vec_stage : public drain<std::vector<utils::elem_type>> {
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

struct drain_range_stage : public drain<utils::range> {
public:
  void process(utils::range* in) {

#ifdef DEBUG
std::cout << "[drain_range_stage] result:\n";
std::vector<utils::elem_type> v(in->left, in->right);
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

struct map_vec_prod : public map_stage_wrapper<std::vector<utils::elem_elem_idx_idx>, std::vector<utils::elem_idx_idx>, utils::elem_elem_idx_idx, utils::elem_idx_idx> {
public:
  explicit map_vec_prod() {}

  std::vector<utils::elem_idx_idx> compute(std::vector<utils::elem_elem_idx_idx>& in) override {
    std::vector<utils::elem_idx_idx> v(parameters::matrix_size);

    for (size_t i=0; i<in.size(); ++i) {
      v[i] = op(in[i]);
    }

    #ifdef DEBUG
    std::cout << "map compute" << std::endl;
    for (auto el : v) {
      std::cout << el.a << " " << el.i << " " << el.j << std::endl;
    }
    #endif

    delete &in;

    return v;
  }

  utils::elem_idx_idx op(const utils::elem_elem_idx_idx& in) override {
    //utils::waste(parameters::minimum_wait);
    auto res = (in.a)*(in.b);
    auto out = utils::elem_idx_idx(res, in.i, in.j);
    return out;
  }
};

struct red_sum : public reduce_stage_wrapper<std::vector<utils::elem_idx_idx>, utils::elem_idx_idx> {
public:
  utils::elem_idx_idx identity;

  explicit red_sum() { }

  utils::elem_idx_idx compute(std::vector<utils::elem_idx_idx>& in) override {
    auto out = identity;

    for (size_t i=0; i<in.size(); ++i) {
      out = op(out, in[i]);
    }

    #ifdef DEBUG
    std::cout << "reduce compute" << std::endl;
    out.print();
    #endif

    delete &in;

    return out;
  }

  utils::elem_idx_idx op (utils::elem_idx_idx& a, utils::elem_idx_idx& b) override {
    //utils::waste(parameters::minimum_wait);
    auto res = a.a + b.a;
    return elem_idx_idx(res, b.i, b.j);
  }

};

//ogni elemento + 2
struct seq_vec_vec_stage : public seq_wrapper<std::vector<utils::elem_type>, std::vector<utils::elem_type>> {
public:
  explicit seq_vec_vec_stage() {}

  std::vector<utils::elem_type> compute(std::vector<utils::elem_type>& in) override {
    utils::waste(100*parameters::minimum_wait);
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
struct map_vec_vec_stage : public map_stage_wrapper<std::vector<utils::elem_type>, std::vector<utils::elem_type>, utils::elem_type, utils::elem_type> {
public:
  explicit map_vec_vec_stage() {}

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
    utils::waste(in*parameters::minimum_wait/20);
    return in-1;
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

//ogni elemento dimezzato
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
    utils::waste(5*parameters::minimum_wait);
    std::vector<utils::elem_type> res(v.size());

    for (size_t i=0; i<v.size(); ++i) {
      res[i] = v[i] / 2;
    }

    return res;
  }
};

//prodotto tra matrici
struct map_matrix_mul_stage : public map_stage_wrapper<matrix_couple, matrix, vec_matrix_couple, std::vector<elem_type>> {
public:
  explicit map_matrix_mul_stage() {}

  matrix compute(matrix_couple& mn) override {
    matrix res;

    for (size_t i=0; i<mn.size(); ++i) {
      res.push_back(op(mn[i]));
    }

    return res;
  }

  std::vector<elem_type> op(const vec_matrix_couple& vm_pair) override {
    // utils::waste(10*parameters::minimum_wait);
    const auto a = vm_pair._vec;
    const auto b = vm_pair._mat;

    //res vector initialized at 0
    std::vector<elem_type> res(a.size());

#ifdef DEBUG
std::cout << "input op\n";
std::cout << "vector: \n";
print_vec(a);
std::cout << "matrix: \n";
for (auto& v: b) {
  print_vec(v);
}
#endif

    //length of the orizontal vector
    for (size_t i=0; i<a.size(); ++i) {
      elem_type n = 0;
      //number of rows in matrix b
      for (size_t j=0; j<a.size(); ++j) {
        //b scanned vertically
        n += a[j] * b[j][i];
      }

      //add value to the result
      res[i] = n;
    }

#ifdef DEBUG
std::cout << "vector result\n";
print_vec(res);
#endif

    return res;
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
    utils::waste(parameters::minimum_wait);
    return a + b;
  }
};

//restituisce un vettore dove ogni elemento è la somma dei valori nelle righe della matrice
struct reduce_matrix_vec_stage : public reduce_stage_wrapper<matrix, std::vector<utils::elem_type>> {
public:
  std::vector<utils::elem_type> identity;

  reduce_matrix_vec_stage() {
    identity = std::vector<utils::elem_type>(parameters::inputsize);
    v = identity;
    index = 0;
  }

  std::vector<utils::elem_type> compute(matrix& in) override {

    for (size_t i=0; i<in.size(); ++i) {
      v = op(v, in[i]);
    }

    return v;
  }

  std::vector<utils::elem_type> op(std::vector<utils::elem_type>& a, std::vector<utils::elem_type>& b) override {
    utils::waste(7*parameters::minimum_wait);

    utils::elem_type res = 0;
#ifdef DEBUG
std::cout << "RICEVUTO OP:" << std::endl;
utils::print_vec(b);
#endif
    for (auto& n : b) {
      res += n;
    }
#ifdef DEBUG
    std::cout << "RES DI OP" << std::endl;
    std::cout << res << std::endl;
#endif
    mtx.lock();
#ifdef DEBUG
std::cout << "index = " << index << std::endl;
std::cout << "res = " << res << std::endl;
#endif
    v[index] = res;
#ifdef DEBUG
std::cout << "v[index] = " << v[index] << std::endl;
#endif
    index++;
    index = index % parameters::inputsize;
    mtx.unlock();

    return v;
  }

private:
  size_t index;
  std::vector<utils::elem_type> v;
  std::mutex mtx;
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
      // utils::waste(parameters::minimum_wait);
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
      // utils::waste(parameters::minimum_wait);
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
      // utils::waste(parameters::minimum_wait);
      std::sort(p.left, p.right);
      res.left = p.left;
      res.right = p.right;
  }

  bool cond(const utils::range& p) override {
      // utils::waste(parameters::minimum_wait);
      return (p.right - p.left <= parameters::cut_off);
  }
};

// ogni elemento + 2
struct dc_dummy : public dc_stage_wrapper<std::vector<utils::elem_type>, std::vector<utils::elem_type>> {
public:
  explicit dc_dummy() {}

  void divide(const std::vector<elem_type>& in, std::vector<std::vector<elem_type>>& in_vec) override {
    // auto half_size = in.size() / 2;
    // std::vector<utils::elem_type> a, b;
    // std::copy(in.begin(), in.begin() + half_size, std::back_inserter(a));
    // std::copy(in.begin() + half_size, in.end(), std::back_inserter(b));
    // in_vec.push_back(a);
    // in_vec.push_back(b);
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
    // out.resize(out_vec[0].size() + out_vec[1].size());
    // size_t i = 0;
    // for(auto& a : out_vec[0]) {
    //   out[i] = a;
    //   i++;
    // }
    // for(auto& b : out_vec[1]) {
    //   out[i] = b;
    //   i++;
    // }
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

struct dc_strassen : public dc_stage_wrapper<utils::Operand, utils::Result> {
  /*
   * Divide function: if possible reuses part of the operand (i.e. submatrices)
   */
  void divide(const Operand &op,std::vector<Operand> &subops) override {
      int submatrix_size=op.a_size/2;
      int rs_a=op.rs_a;
      int rs_b=op.rs_b;

      //derive submatrices
      elem_type *a11=&(op.a)[0];
      elem_type *a12=&(op.a)[submatrix_size];
      elem_type *a21=&(op.a)[submatrix_size*rs_a];
      elem_type *a22=&(op.a)[submatrix_size*rs_a+submatrix_size];
      elem_type *b11=&(op.b)[0];
      elem_type *b12=&(op.b)[submatrix_size];
      elem_type *b21=&(op.b)[submatrix_size*rs_b];
      elem_type *b22=&(op.b)[submatrix_size*rs_b+submatrix_size];

      //P1=(a11+a22)(b11+b22)
      elem_type *p11=allocateCompactMatrix(submatrix_size);
      elem_type *p12=allocateCompactMatrix(submatrix_size);
      addCompactMatrix(a11,rs_a,a22,rs_a,p11,submatrix_size,submatrix_size);
      addCompactMatrix(b11,rs_b,b22,rs_b,p12,submatrix_size,submatrix_size);

      //P2=(a21+a22)b11
      elem_type *p21=allocateCompactMatrix(submatrix_size);
      addCompactMatrix(a21,rs_a,a22,rs_a,p21,submatrix_size,submatrix_size);

      //P3=a11(b12-b22)
      elem_type *p32=allocateCompactMatrix(submatrix_size);
      subtCompactMatrix(b12,rs_b,b22,rs_b,p32,submatrix_size,submatrix_size);

      //P4=a22(b21-b11)
      elem_type *p42=allocateCompactMatrix(submatrix_size);
      subtCompactMatrix(b21,rs_b,b11,rs_b,p42,submatrix_size,submatrix_size);

      //P5=(a11+a12)b22
      elem_type *p51=allocateCompactMatrix(submatrix_size);
      addCompactMatrix(a11,rs_a,a12,rs_a,p51,submatrix_size,submatrix_size);

      //P6=(a21-a11)(b11+b12)
      elem_type *p61=allocateCompactMatrix(submatrix_size);
      elem_type *p62=allocateCompactMatrix(submatrix_size);
      subtCompactMatrix(a21,rs_a,a11,rs_a,p61,submatrix_size,submatrix_size);
      addCompactMatrix(b11,rs_b,b12,rs_b,p62,submatrix_size,submatrix_size);

      //P7=(a12-a22)(b21+b22)
      elem_type *p71=allocateCompactMatrix(submatrix_size);
      elem_type *p72=allocateCompactMatrix(submatrix_size);
      subtCompactMatrix(a12,rs_a,a22,rs_a,p71,submatrix_size,submatrix_size);
      addCompactMatrix(b21,rs_b,b22,rs_b,p72,submatrix_size,submatrix_size);

      //create the various operands
      subops.emplace_back(p11,submatrix_size,submatrix_size,p12,submatrix_size,submatrix_size,true,true);

      subops.emplace_back(p21,submatrix_size,submatrix_size,b11,submatrix_size,rs_b,true,false);

      subops.emplace_back(a11,submatrix_size,rs_a,p32,submatrix_size,submatrix_size,false,true);

      subops.emplace_back(a22,submatrix_size,rs_a,p42,submatrix_size,submatrix_size,false,true);

      subops.emplace_back(p51,submatrix_size,submatrix_size,b22,submatrix_size,rs_b,true,false);

      subops.emplace_back(p61,submatrix_size,submatrix_size,p62,submatrix_size,submatrix_size,true,true);

      subops.emplace_back(p71,submatrix_size,submatrix_size,p72,submatrix_size,submatrix_size,true,true);
  }

  /*
   * Combine Function
   */
  void combine(std::vector<Result>& ress, Result &ret) override {
      int submatrix_size=ress[0].c_size;

      //allocate the space for the result
      ret.c=allocateCompactMatrix(submatrix_size*2);
      ret.c_size=submatrix_size*2;
      ret.rs_c=submatrix_size*2;

      for(int i=0;i<submatrix_size;i++)
          for(int j=0;j<submatrix_size;j++) {
              //c11=p1+p4-p5+p7
              ret.c[i*ret.rs_c+j] = ress[0].c[i*submatrix_size+j] + ress[3].c[i*submatrix_size+j] -
                      ress[4].c[i*submatrix_size+j] + ress[6].c[i*submatrix_size+j];

              //c12=p3+p5
              ret.c[i*ret.rs_c+j + submatrix_size]= ress[2].c[i*submatrix_size+j] + ress[4].c[i*submatrix_size+j];

              //c21=p2+p4
              ret.c[(i + submatrix_size)*ret.rs_c+j] = ress[1].c[i*submatrix_size+j] + ress[3].c[i*submatrix_size+j];

              //c22=p1-p2+p3+p6
              ret.c[(i + submatrix_size)*ret.rs_c+j + submatrix_size] =
                      ress[0].c[i*submatrix_size+j] - ress[1].c[i*submatrix_size+j] +
                      ress[2].c[i*submatrix_size+j] + ress[5].c[i*submatrix_size+j];
          }
  }

  /*
   * Base case: classical algorithm
   */
  void seq(const Operand &op, Result &ret) override {
      ret.c=compactMatmul(op.a,op.rs_a,op.b,op.rs_b,op.a_size);
      ret.c_size=op.a_size;
      ret.rs_c=op.a_size;
  }

  bool cond(const Operand& op) {
      return (op.a_size<=parameters::cut_off);
  }
};

// //TODO: penso si possa eliminare
// struct map_prod : public mapCOMMENTO_stage_wrapper<matrix_couple, matrix_3d, vec_matrix_couple, matrix> {
// public:
//
//   explicit map_prod() {}
//
//   matrix_3d compute(matrix_couple& mn) override {
//     // auto a = mn.first();
//     // auto b = mn.second();
//
//     matrix_3d res;
//
//     for (size_t i=0; i<mn.size(); ++i) {
//       res.push_back(op(mn[i]));
//     }
//
// // #ifdef DEBUG
// // std::cout << "[map_prod] compute:\n";
// // // for (size_t i=0; i<res.size(); ++i) {
// // //   res[i][0].print();
// // // }
// // #endif
//
//     return res;
//   }
//
//   matrix op(const vec_matrix_couple& vm_pair) override {
//     const auto a = vm_pair._vec;
//     const auto b = vm_pair._mat;
//     matrix res;
//
//     for (size_t i=0; i<a.size(); ++i) {
//       auto v = std::vector<elem_type>(a.size());
//       res.push_back(v);
//     }
// #ifdef DEBUG
// std::cout << "res inizializzato\n";
// for (auto row : res) {
//   print_vec(row);
// }
// std::cout << "fine res inizializzato\n";
//
// vm_pair.print();
// #endif
//
//     //per ogni elemento del vettore
//     for (size_t i=0; i<a.size(); ++i) {
//       //recupero riga matrice da moltiplicare
//       const auto tmp = b[i];
//       //per ogni elemento della riga
//       for (size_t j=0; j<tmp.size(); ++j) {
//           //faccio tutti i prodotti che servono per a[i]
//           //e li metto nella riga giusta
//           res[j][i] = a[i] * tmp[j];
//       }
//     }
//
//     // for (size_t i=0; i<a.size(); ++i) {
//     //   const auto tmp = b[i];
//     //   std::vector<utils::elem_type> v;
//     //
//     //   for (size_t j=0; j<tmp.size(); ++j) {
//     //     //metto nel vettore temporaneo
//     //     v.push_back(a[i] * tmp[j]);
//     //   }
//     //   //prima riga pronta
//     //   res.push_back(v);
//     // }
//
//     //vediamo com'è res
// #ifdef DEBUG
// std::cout << "------\n";
// std::cout << "res da op\n";
// for (auto& v : res) {
//   utils::print_vec(v);
// }
// std::cout << "------\n";
// #endif
//     return res;
//   }
// };
//
// //TODO: penso si possa eliminare
// struct reduce_matrix_stage : public reduceCOMMENTO_stage_wrapper<matrix_3d, matrix> {
// public:
//   matrix identity;
//
//   reduce_matrix_stage() {
//     identity = matrix();
//   }
//
//   matrix compute(matrix_3d& in) override {
//     auto out = identity;
//     //assumo identity (e quindi out) inizia tutto vuoto
//     for (size_t i=0; i<in.size(); ++i) {
//       out = op(out, in[i]);
//     }
//
//     return out;
//   }
//
//   matrix op(matrix& a, matrix& b) {
//     auto res = 0;
//     std::vector<utils::elem_type> res_vec;
//
//     for (size_t i=0; i<b.size(); ++i) {
//       //row of the matrix;
//       auto tmp = b[i];
// #ifdef DEBUG
// std::cout << "riga " << i << std::endl;
// print_vec(tmp);
// #endif
//       for (size_t j=0; j<tmp.size(); ++j) {
//         res += tmp[j];
//       }
//       //aggiungo un elemento alla riga
//       res_vec.push_back(res);
//       res = 0;
//     }
//     //aggiungo la riga alla matrice finale
//     a.push_back(res_vec);
// #ifdef DEBUG
// std::cout << "res_vec:\n";
// print_vec(res_vec);
// std::cout << "matrice finale:\n";
// for (size_t i=0; i<a.size(); ++i) {
//   print_vec(a[i]);
// }
// #endif
//     return a;
//   }
//
// private:
//   long row = 0;
// };
////////////////////////////////////////////////////////////////////////////////
