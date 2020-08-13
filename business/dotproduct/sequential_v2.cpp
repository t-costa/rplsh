#include "business_v2.hpp"
#include <iostream>

using namespace std;

int main() {

    dsrc _src;
    ddrn _drn;
    mul _mul;
    inc _inc;
    size_t size;

    auto t1 = aux::now();
    bool loop = _src.has_next();
    if (!loop) std::cout << "something wrong\n";
    auto tmp = _src.next();

    size = tmp->size();
    std::cout << "size: " << size << endl;

    auto t2 = aux::now();
    auto v1 = _mul.compute(*tmp);
    auto t3 = aux::now();
    auto res = _inc.compute(v1);
    auto t4 = aux::now();

    // compute total times
    auto tsrc = aux::time_elapsed<aux::milliseconds>(t2, t1);
    auto tmul = aux::time_elapsed<aux::milliseconds>(t3, t2);
    auto tinc = aux::time_elapsed<aux::milliseconds>(t4, t3);
    auto ttot = aux::time_elapsed<aux::milliseconds>(t4, t1);

    // not interesting measuring "drain time" (negligible)
    _drn.process(&res);

    std::cout << "total time :" << ttot << std::endl;
    std::cout << "source time :" << tsrc << std::endl;
    std::cout << "mul time :" << tmul << " (total), " << (tmul/size) << " (per item)" << std::endl;
    std::cout << "inc time :" << tinc << " (total), " << (tinc/size) << " (per item)" << std::endl;
    std::cout << "size: " << size << std::endl;

    return 0;
}
