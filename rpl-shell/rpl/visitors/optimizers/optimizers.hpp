#ifndef rpl_optimizers_hpp
#define rpl_optimizers_hpp

#include "visitors/visitor_interface.hpp"
#include "visitors/visitors.hpp"
#include "visitors/evaluators/evaluators.hpp"
#include "environment/rpl_environment.hpp"

/**
    interface for each optimize rule: recursively
    apply the optimization rule for each children
    of a visited node, excluding the sequential
    node and the identifier node
*/
struct optrule : public skel_visitor
{
    void visit(seq_node& n) override;
    void visit(comp_node& n) override;
    void visit(pipe_node& n) override;
    void visit(farm_node& n) override;
    void visit(map_node& n) override;
    void visit(reduce_node& n) override;
    void visit(id_node& n) override;
    virtual void operator() ( skel_node& n ) = 0;

    bool subexp;

protected:
    explicit optrule(rpl_environment& env);
    rpl_environment& env;
    servicetime ts;
};

// choose parallelism degree such that
// the new servicetime is equal to the
// emitter service time
struct farmopt : public optrule
{
    explicit farmopt( rpl_environment& env );
    void visit( farm_node& n ) override;
    void operator()( skel_node& n ) override ;
};

// choose parallelism degree such that
// the new servicetime is equal to the
// max of scatter-gather service times
struct mapopt : public optrule
{
    explicit mapopt( rpl_environment& env );
    void visit( map_node& n ) override;
    void operator()( skel_node& n ) override;
//private:
//    assign_resources assignres;
};

// choose parallelism degree such that
// the new servicetime is equal to the
// max of scatter-gather service times
struct reduceopt : public optrule
{
    explicit reduceopt( rpl_environment& env );
    void visit( reduce_node& n ) override;
    void operator()( skel_node& n ) override;
//private:
//    assign_resources assignres;
};

// given a skeleton pipe node it tries
// to reduce the parallelism degree of
// its fastest children
struct pipebalance : public optrule
{
    explicit pipebalance( rpl_environment& env );
    void visit(pipe_node& n) override;
    void visit(farm_node& n) override;
    void visit(map_node& n) override;
    void visit(reduce_node& n) override;
    void operator()( skel_node& n ) override;
private:
    void operator()( skel_node& n, double max );
    double ts_max;
};

// 1) balances the stages -> call pipebalance
// 2) merges faster stages
struct pipeopt : public optrule
{
    explicit pipeopt( rpl_environment& env );
    void visit( pipe_node& n ) override;
    void operator()( skel_node& n ) override;
private:
    pipebalance balance;
};

struct maxresources : public optrule
{
    explicit maxresources ( rpl_environment& env );
    void visit( comp_node& n ) override;
    void visit( pipe_node& n ) override;
    void visit( farm_node& n ) override;
    void visit( map_node& n ) override;
    void visit( reduce_node& n ) override;
    void operator()( skel_node& n ) override;
private:
    //void operator()( skel_node& n, size_t maxres );
    reduce_resources reduce_res;
    resources res;
    size_t maxres;
};

struct twotier : public optrule
{
    explicit twotier( rpl_environment& env );
    void visit( map_node& n ) override;
    void visit( reduce_node& n) override;
    void operator()( skel_node& n ) override;
};

struct farmfarmopt : public optrule
{
    explicit farmfarmopt( rpl_environment& env );
    void visit(farm_node& n) override;
    void operator()( skel_node& n ) override;
};

struct mapmapopt : public optrule
{
    explicit mapmapopt( rpl_environment& env );
    void visit(map_node& n) override;
    void operator()( skel_node& n ) override;
};

#endif
