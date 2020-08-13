#include "business.hpp"

int main() {
    dsrc source;
    ddrn drain;
    map_first m1;
    map_second m2;
    map_third m3;
    map_fourth m4;
    map_fifth m5;

    while (source.has_next()) {
        auto v = source.next();
        auto v1 = m1.compute(*v);
        auto v2 = m2.compute(v1);
        auto v3 = m3.compute(v2);
        auto v4 = m4.compute(v3);
        auto v5 = m5.compute(v4);
        drain.process(&v5);
    }
}