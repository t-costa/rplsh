#ifndef rpl_Inode_hpp
#define rpl_Inode_hpp

#include "visitors/visitor_interface.hpp"

/**
 * The base node interface of the rpl tree
 */
 //TODO: check if the rules of five should be applied
struct rpl_node {
    virtual ~rpl_node()             = default;
    virtual void accept(visitor& v) = 0;
};

/**
 * Represents the right part of a shell assignment
 */
struct rvalue_node : rpl_node {
};

#endif
