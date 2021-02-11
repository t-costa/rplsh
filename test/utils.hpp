#ifndef utils_hpp
#define utils_hpp

#include <math.h>
#include <random>
#include <chrono>
#include <vector>
#include <fstream>
#include <iterator>
#include <iostream>
#include <nlohmann/json.hpp>
#include <sys/stat.h>
#include <dirent.h>
#include <printf.h>

using json = nlohmann::json;


namespace utils {

  //typedef double size_type ;
  typedef int elem_type;
  typedef std::vector<elem_type> vec;
  typedef std::pair<elem_type, elem_type> pair;
  typedef std::vector<pair> vec_pair;

  typedef std::vector<vec> matrix;
  typedef std::vector<matrix> matrix_3d;
  typedef std::pair<vec, vec> vec_couple;

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
  void init(Iterator begin, Iterator end, uint fixed_seed) {
  #ifdef DEBUG
      elem_type lower_bound = 0;
      elem_type upper_bound = 9;
  #else
      elem_type lower_bound = -99999;
      elem_type upper_bound = 99999;
  #endif
      std::uniform_real_distribution<double> unif(lower_bound,upper_bound);
      std::default_random_engine re(fixed_seed);
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
    #ifdef DEBUG
        elem_type lower_bound = 1;
        elem_type upper_bound = 5;
    #else
        elem_type lower_bound = 1;
        elem_type upper_bound = 30;
    #endif
    std::uniform_real_distribution<double> unif(lower_bound,upper_bound);
    std::default_random_engine re(seed);
    seed++;
    int i = 0;
    auto size = end-begin;
    while (i < size / 2) {
          (*begin++) = unif(re);
          i++;
    }
    while (begin != end) {
      (*begin++) = 0;
    }
    //voglio qualcosa del tipo:
    //[a, b, c, ... k, 0, ... 0]
  }

  inline void waste(const double& millisec) {
      auto t1 = std::chrono::high_resolution_clock::now();
      auto t2 = std::chrono::high_resolution_clock::now();

      auto elapsed = t2 - t1;

      while (std::chrono::duration_cast<std::chrono::milliseconds>(elapsed).count() < millisec) {
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

  struct range {
      std::vector<utils::elem_type>::iterator left{};
      std::vector<utils::elem_type>::iterator right{};

      range() = default;

      range(std::vector<utils::elem_type>::iterator _left, std::vector<utils::elem_type>::iterator _right) : left(_left), right(_right) {}
  };

  struct vec_matrix_couple {
  public:
    vec_matrix_couple(const std::vector<elem_type>& v, const matrix& m) :
      _vec(v), _mat(m) { }

    const std::vector<elem_type> get_vector() {
      return _vec;
    }

    const matrix get_matrix() {
      return _mat;
    }

    // void set_vector(const std::vector<T>& v){
    //   _vec = v;
    // }
    //
    // void set_matrix(const matrix<T>& m) {
    //   _mat = m;
    // }

    void print() const{
      std::cout << "------\n";
      std::cout << "vector: \n";
      for (const auto& el : _vec) {
        std::cout << el << " ";
      }
      std::cout << "\nmatrix: \n";
      for (const auto& v : _mat) {
        for (const auto& el : v) {
          std::cout << el << " ";
        }
        std::cout << std::endl;
      }
      std::cout << "------\n";
    }

    const std::vector<elem_type> _vec;
    const matrix _mat;
  };

  //Assumption: the second matrix is transposed
  struct matrix_couple {
  public:
    matrix_couple(const matrix& a, const matrix& b) {
      m = std::make_pair(a, b);
    }

    size_t size() {
      return m.first.size();
    }

    const vec_matrix_couple operator[] (size_t i) {
      auto a = m.first;
      auto b = m.second;

      return vec_matrix_couple(a[i], b);
    }

    // void push_back(vec_couple<T>& v) {
    //   m.first.push_back(v.first());
    //   m.second.push_back(v.second());
    // }

  private:
    std::pair<matrix, matrix> m;
  };

  struct elem_elem_idx_idx {
  public:
    elem_type a, b;
    size_t i, j;

    explicit elem_elem_idx_idx() : a(0), b(0), i(0), j(0) {}

    elem_elem_idx_idx(elem_type& a, elem_type& b, elem_type i, elem_type j) {
      this->a = a;
      this->b = b;
      this->i = i;
      this->j = j;
    }

    elem_elem_idx_idx& operator= (elem_elem_idx_idx & v) {
      this->a = v.a;
      this->b = v.b;
      this->i = v.i;
      this->j = v.j;
      return *this;
    }

    void print() const {
      std::cout << "elem_elem_idx_idx" << std::endl;
      std::cout << "a: " << a << std::endl;
      std::cout << "b: " << b << std::endl;
      std::cout << "i: " << i << std::endl;
      std::cout << "j: " << j << std::endl;
    }
  };

  struct vec_vec_idx_idx {
  public:
    vec a, b;
    size_t i, j;

    explicit vec_vec_idx_idx() : i(0), j(0) {
      a = vec(parameters::matrix_size);
      b = vec(parameters::matrix_size);
    }

    explicit vec_vec_idx_idx(const vec& a, const vec& b, size_t i, size_t j) :
      a(a), b(b), i(i), j(j) { }

    elem_elem_idx_idx& operator[](size_t idx) {
      out.a = a[idx];
      out.b = b[idx];
      out.i = i;
      out.j = j;
      //elem_elem_idx_idx(a[idx], b[idx], i, j)
      return out;
    }

    size_t size() {
      return a.size();
    }

    void print() {
      std::cout << "vec_vec_idx_idx" << std::endl;
      std::cout << "a: ";
      print_vec(a);
      std::cout << "b: ";
      print_vec(b);
      std::cout << "i: " << i << std::endl;
      std::cout << "j: " << j << std::endl;
    }
  private:
    elem_elem_idx_idx out;
  };

  struct elem_idx_idx {
  public:
    elem_type a;
    size_t i, j;

    explicit elem_idx_idx() : a(0), i(0), j(0) {}

    elem_idx_idx(elem_type& a, size_t i, size_t j) : a(a), i(i), j(j) {}

    void print() {
      std::cout << "elem_idx_idx" << std::endl;
      std::cout << "a: " << a << std::endl;
      std::cout << "i: " << i << std::endl;
      std::cout << "j: " << j << std::endl;
    }
  };

  struct vec_idx_idx {
  public:
    vec a;
    size_t i, j;

    explicit vec_idx_idx() : i(0), j(0) {
      a = vec(parameters::matrix_size);
    }

    vec_idx_idx(const vec& a, size_t i, size_t j) : a(a), i(i), j(j) {}

    elem_idx_idx& operator[](size_t idx) {
      // auto out = elem_idx_idx(a[idx], i, j);
      out.a = a[idx];
      out.i = i;
      out.j = j;
      return out;
    }

    size_t size() {
      return a.size();
    }

    void print() {
      std::cout << "vec_idx_idx" << std::endl;
      std::cout << "a: ";
      print_vec(a);
      std::cout << "i: " << i << std::endl;
      std::cout << "j: " << j << std::endl;
    }

  private:
    elem_idx_idx out;

  };

  inline double speed_up(const double& time_seq, const double& time_par) {
    return time_seq / time_par;
  }

  void build_json(const double& seq_time,                             //tseq
                const std::vector<std::pair<int, double>>& par_time,  //nw-tc
                const std::vector<int>& par_degree,                   //lista nw
                const std::string& problem) {                         //nome problema
    json j_time, j_speedup, j_scalab, j_eff;

    std::vector<double> par;
    int first = 1, count = 0;
    double avg = 0;
    for (const auto &i : par_time) {
        if (first == i.first) {
            //same degree
            avg += i.second;
            count++;
        } else {
            //new degee
            first = i.first;
            par.push_back(avg / count);
            avg = i.second;
            count = 1;
        }
    }

    //last element
    par.push_back(avg / count);

    std::vector<double> all_y;
    std::vector<double> all_x;
    for (auto& v : par_time) {
        all_y.push_back(v.second);
        all_x.push_back(v.first);
    }

    j_time["x"] = par_degree;
    j_time["y"] = par;
    j_time["all_x"] = all_x;
    j_time["all_y"] = all_y;
    j_time["title"] = "Graph of execution time w.r.t. parallel degree for " + problem;
    j_time["xaxis"] = "Parallelism degree";
    j_time["yaxis"] = "Execution time (seconds)";
    j_time["seq_time"] = seq_time;
    j_time["name"] = problem;

    DIR* dir = opendir("json_files");
    if (dir)
    {
        /* Directory exists. */
        closedir(dir);
    }
    else if (ENOENT == errno)
    {
        /* Directory does not exist. */
        const int dir_err = mkdir("json_files", S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
        if (dir_err == -1) {
            std::cerr << "Error creating directory!" << std::endl;
        }
    }
    else
    {
        /* opendir() failed for some other reason. */
        std::cerr << "Something went wrong with the directory..." << std::endl;
    }

    //write j_time to file
    std::ofstream of_time;
    of_time.open("./json_files/" + problem + "-time.json");
    of_time << std::setw(4) << j_time << std::endl;
    of_time.close();

    std::vector<double> speed_vec_all;
    std::vector<double> speed_vec_avg;
    std::vector<int> speed_x;
    for (const auto& t : par_time) {
        speed_vec_all.push_back(speed_up(seq_time, t.second));
        speed_x.push_back(t.first);
    }

    for (const auto& t : par) {
        speed_vec_avg.push_back(speed_up(seq_time, t));
    }

    j_speedup["x"] = par_degree;
    j_speedup["y"] = speed_vec_avg;
    j_speedup["times"] = par;
    j_speedup["all_x"] = speed_x;
    j_speedup["all_y"] = speed_vec_all;
    j_speedup["title"] = "Graph of speedup w.r.t. parallel degree for " + problem;
    j_speedup["xaxis"] = "Parallelism degree";
    j_speedup["yaxis"] = "Speedup";
    j_speedup["seq_time"] = seq_time;
    j_speedup["name"] = problem;

    std::ofstream of_speed;
    of_speed.open("./json_files/" + problem + "-speedup.json");
    of_speed << std::setw(4) << j_speedup << std::endl;
    of_speed.close();

    // std::vector<double> scalab;
    // for (const auto& t : par) {
    //     scalab.push_back(par[0] / t);
    // }
    //
    // j_scalab["x"] = par_degree;
    // j_scalab["y"] = scalab;
    // j_scalab["title"] = "Graph of scalability w.r.t. parallel degree for " + problem;
    // j_scalab["xaxis"] = "Parallelism degree";
    // j_scalab["yaxis"] = "Scalability";
    // j_scalab["name"] = problem;
    //
    // std::ofstream of_scalab;
    // of_scalab.open("./json_files/" + problem + "-scalability.json");
    // of_scalab << std::setw(4) << j_scalab << std::endl;
    // of_scalab.close();
    //
    // std::vector<double> eff;
    // for (size_t i=0; i<par_degree.size(); ++i) {
    //     eff.push_back(speed_vec_avg[i] / (double) par_degree[i]);
    // }
    //
    // j_eff["x"] = par_degree;
    // j_eff["y"] = eff;
    // j_eff["title"] = "Graph of efficiency w.r.t. parallel degree for " + problem;
    // j_eff["xaxis"] = "Parallelism degree";
    // j_eff["yaxis"] = "Efficiency";
    // j_eff["name"] = problem;
    //
    // std::ofstream of_eff;
    // of_eff.open("./json_files/" + problem + "-efficiency.json");
    // of_eff << std::setw(4) << j_eff << std::endl;
    // of_eff.close();

  }

}

#endif
