//
// Created by tommaso on 1/21/20.
//

#ifndef AUX_SEQUENTIAL_HPP
#define AUX_SEQUENTIAL_HPP

#include <vector>
#include <algorithm>
#include <chrono>
#include <thread>
#include "./wrappers.hpp"

/**
 * generates vectors of double
 */
struct generator : public source<std::vector<double >> {
public:
    generator(size_t size, int dimension) : m_size(size), m_dimension(dimension) {
        current = 0;
    }

    bool has_next() override {
        return current < m_dimension;
    }

    std::vector<double>* next() override {
        if (has_next()) {
            current++;
            auto v = new std::vector<double >();

            for (size_t i=0; i<m_size; ++i) {
                double value = rand() % 10 + 1;
                std::cout << "[GENERATOR]: vector " << current <<
                ", position " << i << ", value " << value << std::endl;
                v->emplace_back(value);
            }

            std::this_thread::sleep_for(std::chrono::milliseconds(1));
            return v;
        }

        return nullptr;
    }

private:
    size_t m_size;
    int current;
    int m_dimension;
};


struct sorter : public  seq_wrapper<std::vector<double>, std::vector<double>> {
public:
    std::vector<double> compute(std::vector<double>& v) override {
        //should be pretty heavy
        std::sort(v.begin(), v.end());
        std::cout << "[SORTER]: ";
        for (auto n : v) {
          std::cout << n << " ";
        }
        std::cout << std::endl;
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
        return v;
    }
};


struct multiplier : public seq_wrapper<std::vector<double>, std::vector<double>> {
public:
    std::vector<double> compute(std::vector<double>& v) override {
        for (auto& el : v) {
            el = op(el);
//            el *= el;
//            //now this is dependent from the size
//            std::this_thread::sleep_for(std::chrono::milliseconds(2));
        }
        std::cout << "[MULTIPLIER]: ";
        for (auto n : v) {
          std::cout << n << " ";
        }
        std::cout << std::endl;
        return v;
    }

    double op(double a) {
        std::this_thread::sleep_for(std::chrono::milliseconds(2));
        return a*a;
    }

};

struct sum : public seq_wrapper<double, std::vector<double>> {
public:
    double const identity = 0;

    double compute(std::vector<double>& v) override {
        double res = identity;
        for (auto el : v) {
            res = op(res, el);
        }

        std::cout << "[REDUCER]" << res << std::endl;
        return res;
    }

    double op(double a, double b) {
        std::this_thread::sleep_for(std::chrono::milliseconds(1));
        return a+b;
    }
};

#endif //AUX_SEQUENTIAL_HPP
