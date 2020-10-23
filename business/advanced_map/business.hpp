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
#include <math.h>


typedef double size_type ;
typedef std::vector<size_type> vec;

#ifdef DEBUG
constexpr int dimension = 5;
constexpr size_type inputsize = 10;
#else
constexpr int dimension = 100;
constexpr size_type inputsize = 300;
#endif

bool sequential = true;
int index_par_check = 0;
unsigned seed = 1;
int waste_time = 10;

struct vec_pair {

    vec_pair(vec& A, vec& B) : A(A), B(B) {}

    const std::pair<size_type,size_type> operator[] (int x) {
        return std::make_pair(A[x], B[x]);
    }

    std::size_t size() {
        return A.size();
    }

private:
    const vec A;
    const vec B;
};

template <typename Iterator>
void init(Iterator begin, Iterator end) {
#ifdef DEBUG
    double lower_bound = 0;
    double upper_bound = 9;
#else
    double lower_bound = 0;
    double upper_bound = 999999999;
#endif
    std::uniform_real_distribution<double> unif(lower_bound,upper_bound);
    std::default_random_engine re(seed);
    seed++;
    while (begin != end) {
        (*begin++) = unif(re);
    }
}

inline void waste(const double& microsec) {
    auto t1 = std::chrono::high_resolution_clock::now();
    auto t2 = std::chrono::high_resolution_clock::now();

    auto elapsed = t2 - t1;

    while (std::chrono::duration_cast<std::chrono::microseconds>(elapsed).count() < microsec) {
        t2 = std::chrono::high_resolution_clock::now();
        elapsed = t2 - t1;
    }
}

void print_vec_pair(vec_pair& vp) {
    for (size_t i=0; i<vp.size(); ++i) {
        std::cout << vp[i].first << "-" << vp[i].second << " ";
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

namespace result_store {
    template <typename T>
    static std::vector<T> res;

    template <typename T>
    static void add_result(const T& r) {
        res<T>.emplace_back(r);
    }

    template <typename T>
    static bool check(int i, const T& r) {
        return test_equality(res<T>[i], r);
    }

    template <typename T>
    void check_store() {
        for (auto& v : result_store::res<T>) {
            print_vec(v);
        }
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
        auto vp = new vec_pair(A,B);

#ifdef DEBUG
std::cout << "Result source: ";
print_vec_pair(*vp);
#endif
        return vp;
    }

private:
    int i;
};

class ddrn : public drain<vec> {
public:
    void process(vec* value) {

#ifdef DEBUG
std::cout << "Result drain: ";
print_vec(*value);
#endif

        if (sequential) {
            result_store::add_result(*value);
        } else {
            //devo fare il check
            #ifdef DEBUG
            result_store::check(index_par_check, *value) ? std::cout << "OK" : std::cout << "ERROR";
            std::cout << std::endl;
            #else
            result_store::check(index_par_check, *value);
            #endif

            index_par_check++;

        }

        //delete value;
    }
};

class map_first : public map_stage_wrapper<vec_pair, vec, std::pair<size_type, size_type>, size_type>
{
public:
    vec compute(vec_pair& vp) {
        vec v(vp.size());
        for (size_t i = 0; i < vp.size(); i++)
            v[i] = op(vp[i]);

#ifdef DEBUG
std::cout << "Result map_first: ";
print_vec(v);
#endif
        delete &vp;

        return v;
    }

    size_type op(const std::pair<size_type, size_type>& p) {
        //std::this_thread::sleep_for(std::chrono::nanoseconds(300));
        waste(2 * waste_time);
        return p.first*p.second;
    }
};

class map_second : public map_stage_wrapper<vec, vec, size_type, size_type> {
public:
    vec compute(vec& v) {
        vec out(v.size());

        for (size_t i=0; i<v.size(); ++i) {
            out[i] = op(v[i]);
        }

#ifdef DEBUG
std::cout << "Result map_second: ";
print_vec(out);
#endif

        return out;
    }

    size_type op(const size_type& el) {
        //std::this_thread::sleep_for(std::chrono::nanoseconds(200));
        waste(waste_time);
        return el / 2;
    }
};

class map_third : public map_stage_wrapper<vec, vec, size_type, size_type> {
public:
    vec compute(vec& v) {
        vec out(v.size());

        for (size_t i=0; i<v.size(); ++i) {
            out[i] = op(v[i]);
        }

#ifdef DEBUG
std::cout << "Result map_third: ";
print_vec(out);
#endif

        return out;
    }

    size_type op(const size_type& el) {
        //std::this_thread::sleep_for(std::chrono::nanoseconds(400));
        waste(3 * waste_time);
        return el * el;
    }
};

class map_fourth : public map_stage_wrapper<vec, vec, size_type, size_type> {
public:
    vec compute(vec& v) {
        vec out(v.size());

        for (size_t i=0; i<v.size(); ++i) {
            out[i] = op(v[i]);
        }

#ifdef DEBUG
std::cout << "Result map_fourth: ";
print_vec(out);
#endif

        return out;
    }

    size_type op(const size_type& el) {
        //std::this_thread::sleep_for(std::chrono::nanoseconds(10));
        waste(waste_time / 2);
        return el + 1;
    }
};

class map_fifth : public map_stage_wrapper<vec, vec, size_type, size_type> {
public:
    vec compute(vec& v) {
        vec out(v.size());

        for (size_t i=0; i<v.size(); ++i) {
            out[i] = op(v[i]);
        }

#ifdef DEBUG
std::cout << "Result map_fifth: ";
print_vec(out);
#endif

        return out;
    }

    size_type op(const size_type& el) {
        //std::this_thread::sleep_for(std::chrono::nanoseconds(10));
        waste(waste_time / 2);
        return el - 7;
    }
};


#endif
