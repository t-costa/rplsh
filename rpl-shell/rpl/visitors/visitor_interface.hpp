#ifndef rpl_Ivisitor_hpp
#define rpl_Ivisitor_hpp

#include "nodes/node_forwards.hpp"
#include <string>

/**
 * Interface for all the different visitors;
 * declares the visit operations for all the nodes
 */
struct visitor
{
    virtual void visit(assign_node& n)  = 0;
    virtual void visit(show_node& n)    = 0;
    virtual void visit(set_node& n)     = 0;
    virtual void visit(ann_node& n)     = 0;
    virtual void visit(rwr_node& n)     = 0;
    virtual void visit(opt_node& n)     = 0;
    virtual void visit(history_node& n) = 0;
    virtual void visit(import_node& n)  = 0;
    virtual void visit(gencode_node& n) = 0;
    virtual void visit(expand_node& n)  = 0;
    virtual void visit(add_node& n)     = 0;
    virtual void visit(load_node& n)  = 0;

    virtual void visit(seq_node& n)     = 0;
    virtual void visit(source_node& n)  = 0;
    virtual void visit(drain_node& n)   = 0;
    virtual void visit(comp_node& n)    = 0;
    virtual void visit(pipe_node& n)    = 0;
    virtual void visit(farm_node& n)    = 0;
    virtual void visit(map_node& n)     = 0;
    virtual void visit(reduce_node& n)  = 0;
    virtual void visit(dc_node& n)      = 0;
    virtual void visit(id_node& n)      = 0;

    virtual void visit(_& n) {}
};

/**
 * Skeleton visitor, it does nothing
 */
struct skel_visitor : public visitor
{
    using visitor::visit;

    // notice that source and drain have
    // the base_class implementation
    void visit(source_node& n) override {};
    void visit(drain_node& n) override  {};

    void visit(assign_node& n) override  {};
    void visit(show_node& n) override    {};
    void visit(set_node& n) override     {};
    void visit(ann_node& n) override     {};
    void visit(rwr_node& n) override     {};
    void visit(opt_node& n) override     {};
    void visit(history_node& n) override {};
    void visit(import_node& n) override  {};
    void visit(gencode_node& n) override {};
    void visit(expand_node& n) override  {};
    void visit(add_node& n) override  {};
    void visit(load_node& n) override {};
};

#endif
