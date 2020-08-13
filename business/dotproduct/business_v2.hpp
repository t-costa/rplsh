// dot product of two vectors
// the multiplication operator in "mul op" is set to last 15 ms,
// the addition operator in "inc op" is set to last 5 ms

#ifndef dotproduct_business_v2_hpp
#define dotproduct_business_v2_hpp

#include "aux/wrappers.hpp"
#include "aux/aux.hpp"
#include <ctime>
#include <cstdlib>
#include <limits>
#include <vector>
#include <utility>
#include <iostream>
#include <chrono>
#include <thread>
#include <random>

typedef double size_type ;
typedef std::vector<size_type> vec;

struct vec_pair {

    vec_pair(vec& A, vec& B) : A(A), B(B) {}

    const std::pair<size_type,size_type> operator[] (int x) {
        return std::make_pair(A[x], B[x]);
    }

    std::size_t size() {
        return A.size();
    }

    // vec_pair operator=(vec_pair& v1, vec_pair& v2) {
    //   for (int i=0; i<v1.size(); ++i) {
    //     v2.emplace_back(v[i]);
    //   }
    //   return v2;
    // }

private:
    const vec A;
    const vec B;
};

constexpr int dimension = 1;
constexpr size_type inputsize = 20;

template <typename Iterator>
void init(Iterator begin, Iterator end) {
  double lower_bound = 0;
  double upper_bound = 25;
  std::uniform_real_distribution<double> unif(lower_bound,upper_bound);
  std::default_random_engine re;
  //double a_random_double = unif(re);
  while (begin != end) {
    // auto tmp = (double) ( ( (float)rand() / (float)RAND_MAX ) * std::numeric_limits<int>::max() );
    // std::cout << tmp << std::endl;
    // *(begin++) = tmp > 100 ? 100 : tmp;
    (*begin++) = unif(re);
  }
}

class dsrc : public source<vec_pair> {
public:

    dsrc() : i(0) {
        srand(0);
    }

    bool has_next() {
        return i++ < dimension;
    }

    vec_pair* next() {
        vec A(inputsize);
        vec B(inputsize);
        init(A.begin(), A.end());
        init(B.begin(), B.end());
        return new vec_pair(A,B);
    }

private:
    int i;
};

class ddrn : public drain<size_type> {
public:
    void process(size_type* value) {
        std::cout << "result is: " << *value << std::endl;
    }
};

class mul : public map_stage_wrapper<vec, vec_pair, size_type, std::pair<size_type, size_type>> {
public:
    vec compute(vec_pair& vp) override {
        //vec v(vp.size());
        auto v = begin_hook(vp);
        for (size_t i = 0; i < vp.size(); i++)
            v[i] = op(vp[i]);
        return v;
    }

    size_type op(const std::pair<size_type, size_type>& p) override {
        std::this_thread::sleep_for(std::chrono::milliseconds(15));
        return p.first*p.second;
    }

    vec begin_hook(vec_pair vp) override {
        vec v(vp.size());
        return v;
    }
};

class inc : public reduce_stage_wrapper<size_type, vec, size_type, size_type> {
public:
    size_type compute(vec& v) {
        //size_type sum = identity;
        auto sum = begin_hook(v);
        for (auto& e : v)
            sum = op(sum, e);
        return sum;
    }

    size_type op(size_type& a, size_type& b) {
        std::this_thread::sleep_for(std::chrono::milliseconds(5));
        return a + b;
    }

    size_type begin_hook(vec v) {
        //in realtà qui non serve l'hook per questo,
        //però magari serve fare altro per settare
        //il valore iniziale... tipo il primo elemento di v?
        return identity;
    }

    //non dovrebbe servire perchè incluso in red_wrapper
    //const size_type identity{0};
};

#endif
