#ifndef rpl_printable_hpp
#define rpl_printable_hpp

#include "nodes/skeletons.hpp"
#include <string>

/**
 * Interface for the print operation
 */
struct printable {
    virtual std::string print(skel_node& n) = 0;
};

#endif
