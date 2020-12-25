#ifndef rplsh_wrappers_hpp
#define rplsh_wrappers_hpp

#include <cstdlib>

template <typename Tin, typename Tout>
class seq_wrapper {
public:
  virtual Tout compute(Tin& input) = 0;
};

template <typename Tin, typename Tout>
class dc_stage_wrapper {
public:
  virtual Tout compute(Tin& input) {
    Tout res;
    DC_algo(input, res);
    return res;
  }

  virtual void divide(const Tin& in, std::vector<Tin>& in_vec) = 0;

  virtual void combine(std::vector<Tout>& out_vec, Tout& out) = 0;

  virtual void seq(const Tin& in, Tout& out) = 0;

  virtual bool cond(const Tin& in) = 0;

private:
  void DC_algo(const Tin& p, Tout& r) {
    if (!cond(p)) {
        std::vector<Tin> sub_p;

        divide(p, sub_p);

        std::vector<Tout> res(sub_p.size());
        for (size_t i=0; i<sub_p.size(); ++i) {
            DC_algo(sub_p[i], res[i]);
        }

        combine(res, r);
    } else {
        seq(p, r);
    }
  }
};

template<typename Tin,
          typename Tout,
          typename Tin_el, //tipo degli elementi di Tin
          typename Tout_el>  //tipo degli elementi di Tout, o direttamente Tout se non è un vettore
          //typename T>
class map_stage_wrapper {

  //lascio la compute perchè così lo stadio può essere
  //messo dentro un qualsiasi pattern (farm, pipe, seq...)
  virtual Tout compute(Tin& input) = 0;

  virtual Tout_el op(const Tin_el& input) = 0;
};

//stessi discorsi che per la map
template<typename Tin,
          typename Tout,
          typename Tin_el,
          typename Tout_el>
          //typename T>
class reduce_stage_wrapper {

  virtual Tout compute(Tin& input) = 0;

  virtual Tout_el op(Tin_el& input1, Tin_el& input2) = 0;

protected:
  Tout identity{};
};

template <typename Tout>
class source {
public:
  virtual bool has_next() = 0;
  virtual Tout* next() = 0;
};

template <typename Tin>
class drain {
public:
  virtual void process(Tin * x) = 0;
};

/*template <typename T>
class composable {
public:
    virtual T&& operator[](size_t idx);
    virtual T& operator[](size_t idx);
}*/

// class type representing a set of values of type T
// that could be splitted in several sub-composable sets
// -> similiar concept to the TBB Range
template <typename T>
class composable {
public:
    virtual bool is_empty() const       = 0;
    virtual std::size_t size() const    = 0;
    virtual bool is_splittable() const  = 0;
};


template <typename T>
struct vec_couple {
public:
  vec_couple(std::vector<T> a, std::vector<T> b) {
    v = std::make_pair(a, b);
  }

  size_t size() {
    return v.first.size();
  }

  std::pair<T, T>& operator[] (size_t i) {
    return std::make_pair(v.first[i], v.second[i]);
  }

  void push_back(std::pair<T, T> p) {
    v.first.push_back(p.first);
    v.second.push_back(p.second);
  }

  std::vector<T> first() {
    return v.first;
  }

  std::vector<T> second() {
    return v.second;
  }

private:
  std::pair<std::vector<T>, std::vector<T>> v;
};

template <typename T>
struct matrix {
public:
  explicit matrix() {
    m = std::vector<std::vector<T>>();
  }

  size_t size() {
    return m.size();
  }

  std::vector<T>& operator[] (size_t i) {
    return m[i];
  }

  void push_back(const std::vector<T>& v) {
    m.push_back(v);
  }

  void print() {
    for (const auto& v : m) {
      for (const auto& el : v) {
        std::cout << el << " ";
      }
      std::cout << std::endl;
    }
  }

private:
  std::vector<std::vector<T>> m;
};

//Assumption: the second matrix is transposed
template <typename T>
struct matrix_couple {
public:
  matrix_couple(matrix<T> a, matrix<T> b) {
    m = std::make_pair(a, b);
  }

  size_t size() {
    return m.first.size();
  }

  vec_couple<T>& operator[] (size_t i) {
    return vec_couple(m.first[i], m.second[i]);
  }

  void push_back(vec_couple<T>& v) {
    m.first.push_back(v.first());
    m.second.push_back(v.second());
  }

private:
  std::pair<matrix<T>, matrix<T>> m;
};

#endif
