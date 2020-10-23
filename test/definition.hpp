#include <iostream>
#include <vector>
#include <fstream>
#include <iterator>

#include "aux/wrappers.hpp"
#include "aux/aux.hpp"

#include "parameters.hpp"
#include "utils.hpp"

////////////////////////////////////////////////////////////////////////////////
struct source_vecpair_stage : public source<utils::vec_pair> {
public:
  source_vecpair_stage() : i(0) {
    utils::init_random();
  }

  bool has_next() {
    return i++ < parameters::dimension;
  }

  utils::vec_pair* next() {
    std::vector<double> a(parameters::inputsize);
    std::vector<double> b(parameters::inputsize);

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

struct source_vec_stage : public source<std::vector<double>> {
public:
  source_vec_stage() : i(0) {
    utils::init_random();
  }

  bool has_next() {
    return i++ < parameters::dimension;
  }

  std::vector<double>* next() {
    auto v = new std::vector<double>(parameters::inputsize);
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
////////////////////////////////////////////////////////////////////////////////


////////////////////////////////////////////////////////////////////////////////
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

    for (auto el : *in) {
      output_file << el.first << "-" << el.second << " ";
    }

    delete in;
  }
};

struct drain_vec_stage : public drain<std::vector<double>> {
public:
  void process(std::vector<double>* in) {
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

    for (auto el : *in) {
      output_file << el << " ";
    }

    delete in;
  }
};

struct drain_stage : public drain<double> {
public:
  void process(double* in) {
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
    output_file << *in << "/n";

    delete in;
  }
};
////////////////////////////////////////////////////////////////////////////////


////////////////////////////////////////////////////////////////////////////////
//somma di tutti gli elementi
struct reduce_vec_double_stage : public reduce_stage_wrapper<std::vector<double>, double, double, double> {
public:
  reduce_vec_double_stage() {}

  double compute(std::vector<double>& in) override {
    double out = identity;

#ifdef DEBUG
std::cout << "[reduce_vec_double_stage] compute" << std::endl;
#endif

    for (auto& el : in) {
      out = op(out, el);
    }

#ifdef DEBUG
std::cout << "[reduce_vec_double_stage] result: ";
std::cout << out << std::endl;
#endif

    return out;
  }

  double op(double& a, double& b) override {
    utils::waste(4*parameters::minimum_wait);
    return a + b;
  }

protected:
  double identity = 0;
};

//moltiplica ogni elemento per 7
struct map_vec_vec_stage : public map_stage_wrapper<std::vector<double>, std::vector<double>, double, double> {
public:
  explicit map_vec_vec_stage() {}

  std::vector<double> compute(std::vector<double>& in) override {
    std::vector<double> out;
    out.reserve(in.size());

#ifdef DEBUG
std::cout << "[map_vec_vec_stage] compute" << std::endl;
#endif

    for (auto& el: in) {
      out.emplace_back(op(el));
    }

#ifdef DEBUG
std::cout << "[map_vec_vec_stage] result: ";
utils::print_vec(out);
#endif

    return out;
  }

  double op(const double& in) override {
    utils::waste(3*parameters::minimum_wait);
    return in*7;
  }
};

//per ogni elemento fa il quadrato
struct seq_vec_vec_stage : public seq_wrapper<std::vector<double>, std::vector<double>> {
public:
  explicit seq_vec_vec_stage() {}

  std::vector<double> compute(std::vector<double>& in) override {
#ifdef DEBUG
std::cout << "[seq_vec_vec_stage] compute" << std::endl;
#endif
    std::vector<double> out;
    out.reserve(in.size());

    for (size_t i=0; i<in.size(); ++i) {
      out.emplace_back(in[i]*in[i]);
    }

#ifdef DEBUG
std::cout << "[seq_vec_vec_stage] result: ";
utils::print_vec(out);
#endif

    return out;
  }
};

//ogni coppia diventa la sua differenze
struct map_vecpair_vec_stage : public map_stage_wrapper<utils::vec_pair, std::vector<double>, std::pair<double, double>, double> {
public:
  explicit map_vecpair_vec_stage() {}

  std::vector<double> compute(utils::vec_pair& in) override {
    std::vector<double> out;
    out.reserve(in.size());

#ifdef DEBUG
std::cout << "[map_vecpair_vec_stage] compute" << std::endl;
#endif

    for (size_t i=0; i<in.size(); ++i) {
      out.emplace_back(op(in[i]));
    }

#ifdef DEBUG
std::cout << "[map_vecpair_vec_stage] result: ";
utils::print_vec(out);
#endif

    return out;
  }

  double op(const std::pair<double, double>& in) override {
    utils::waste(2*parameters::minimum_wait);
    return in.first - in.second;
  }
};

//inverte le coppie nell'array in input
struct map_vecpair_vecpair_stage : public map_stage_wrapper<utils::vec_pair, utils::vec_pair, std::pair<double, double>, std::pair<double, double>> {
public:
  explicit map_vecpair_vecpair_stage() {}

  utils::vec_pair compute(utils::vec_pair& in) override {
    utils::vec_pair out(in.size());

#ifdef DEBUG
std::cout << "[map_vecpair_vecpair_stage] compute" << std::endl;
#endif

    for (size_t i=0; i<in.size(); ++i) {
      auto el = in[i];
      out.push_back(op(el));
    }

#ifdef DEBUG
std::cout << "[map_vecpair_vecpair_stage] result: ";
utils::print_vec_pair(out);
#endif

    return out;
  }

  std::pair<double, double> op(const std::pair<double, double>& in) override {
    utils::waste(parameters::minimum_wait);
    return std::make_pair(in.second, in.first);
  }
};
////////////////////////////////////////////////////////////////////////////////
