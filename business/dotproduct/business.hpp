// dot product of two vectors
// the multiplication operator in "mul op" is set to last 15 ms,
// the addition operator in "inc op" is set to last 5 ms

#ifndef dotproduct_business_hpp
#define dotproduct_business_hpp

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

typedef long int size_type ;
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

constexpr int dimension = 5;
constexpr size_type inputsize = 100000;

template <typename Iterator>
void init(Iterator begin, Iterator end) {
  double lower_bound = 0;
  double upper_bound = 999;
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

void print_vec_pair(vec_pair& vp) {
    for (size_t i=0; i<vp.size(); ++i) {
        std::cout << vp[i].first << " " << vp[i].second << std::endl;
    }
}

void print_vec(const vec& v) {
    for (auto el : v) {
        std::cout << el << " ";
    }
    std::cout << std::endl;
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
        auto vp = new vec_pair(A,B);
        //print_vec_pair(*vp);
        return vp;
    }

private:
    int i;
};

class ddrn : public drain<size_type> {
public:
    void process(size_type* value) {
        std::cout.precision(20);
        std::cout << "result is: " << *value << std::endl;
    }
};

class mul : public seq_wrapper<vec, vec_pair> {
public:
    vec compute(vec_pair& vp) {
        vec v(vp.size());
        for (size_t i = 0; i < vp.size(); i++)
            v[i] = op(vp[i]);

        //std::cout << "RISULTATO MUL (SEQ)" << std::endl;
        //print_vec(v);
        return v;
    }

    size_type op(const std::pair<size_type, size_type>& p) {
        std::this_thread::sleep_for(std::chrono::milliseconds(2));
        return p.first*p.second;
    }
};

class inc : public seq_wrapper<size_type, vec> {
public:
    size_type compute(vec& v) {
        size_type sum = identity;
        for (auto& e : v)
            sum = op(sum, e);

        //std::cout << "RISUTLATO INC (SEQ)" << std::endl;
        //std::cout << sum << std::endl;
        return sum;
    }

    size_type op(size_type& a, size_type& b) {
        std::this_thread::sleep_for(std::chrono::milliseconds(1));
        return a + b;
    }

    const size_type identity{0};
};

#endif
