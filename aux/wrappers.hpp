#ifndef rplsh_wrappers_hpp
#define rplsh_wrappers_hpp

#include <cstdlib>

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

template <typename Tin, typename Tout>
class seq_wrapper {
public:
  virtual Tout compute(Tin& input) = 0;
};

template<typename Tin,
          typename Tout,
          typename Tin_el, //Type of the elements of Tin
          typename Tout_el>  //Type of the elements of Tout
class map_stage_wrapper {

  //can be used by any other pattern
  virtual Tout compute(Tin& input) = 0;

  virtual Tout_el op(const Tin_el& input) = 0;
};

template <typename Tin, typename Tout>
class reduce_stage_wrapper {

  virtual Tout compute (Tin& input) = 0;

  virtual Tout op(Tout& input1, Tout& input2) = 0;

  Tout identity;
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


#endif
