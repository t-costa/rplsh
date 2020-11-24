#ifndef rpl_aux_hpp
#define rpl_aux_hpp

#include <chrono>

namespace aux{
    using namespace std::chrono;

    time_point<high_resolution_clock> now(){
        return high_resolution_clock::now();
    }

    template <typename T>
    double time_elapsed(time_point<high_resolution_clock> t1, time_point<high_resolution_clock> t2) {

        auto diff = duration_cast<std::chrono::milliseconds>( t2 - t1 );
        return diff.count();

    }
}

#endif
