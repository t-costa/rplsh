#ifndef rpl_evaluators_hpp
#define rpl_evaluators_hpp

#include "environment/rpl_environment.hpp"
#include "visitors/visitor_interface.hpp"
#include "utils/printable.hpp"

struct eval_visitor : public skel_visitor, public printable
{
    explicit eval_visitor( rpl_environment& env );
protected:
    rpl_environment& env;
    double global_inputsize;
};

struct servicetime: public eval_visitor
{
    explicit servicetime(rpl_environment& env);
    void visit(seq_node& n) override;
    void visit(source_node& n) override;
    void visit(drain_node& n) override;
    void visit(comp_node& n) override;
    void visit(pipe_node& n) override;
    void visit(farm_node& n) override;
    void visit(map_node& n) override;
    void visit(reduce_node& n) override;
    void visit(dc_node& n) override;
    void visit(id_node& n) override;

    std::string print(skel_node& n) override;
    double operator()(skel_node& n);

    void reset_start();
private:
    double res;
    bool start;
    bool computing_reduce;
    double partial_res_map, partial_res_red;
};

struct latencytime: public eval_visitor
{
    explicit latencytime(rpl_environment& env);
    void visit(seq_node& n) override;
    void visit(source_node& n) override;
    void visit(drain_node& n) override;
    void visit(comp_node& n) override;
    void visit(pipe_node& n) override;
    void visit(farm_node& n) override;
    void visit(map_node& n) override;
    void visit(reduce_node& n) override;
    void visit(dc_node& n) override;
    void visit(id_node& n) override;

    std::string print(skel_node& n) override;
    double operator()(skel_node& n);

    void reset_start();
protected:
    double res;
    bool start;
    bool computing_reduce;
    double partial_res_map, partial_res_red;
};

struct completiontime: public eval_visitor
{
    explicit completiontime(rpl_environment& env);
    void visit(seq_node& n) override;
    void visit(source_node& n) override;
    void visit(drain_node& n) override;
    void visit(comp_node& n) override;
    void visit(pipe_node& n) override;
    void visit(farm_node& n) override;
    void visit(map_node& n) override;
    void visit(reduce_node& n) override;
    void visit(dc_node& n) override;
    void visit(id_node& n) override;

    std::string print(skel_node& n) override;
    double operator()(skel_node& n);
protected:
    latencytime lat;
    servicetime ts;
    double res;
    bool start;
};

struct pardegree: public eval_visitor
{
    explicit pardegree(rpl_environment& env);
    void visit(seq_node& n) override;
    void visit(source_node& n) override;
    void visit(drain_node& n) override;
    void visit(comp_node& n) override;
    void visit(pipe_node& n) override;
    void visit(farm_node& n) override;
    void visit(map_node& n) override;
    void visit(reduce_node& n) override;
    void visit(dc_node& n) override;
    void visit(id_node& n) override;

    std::string print(skel_node& n) override;
    std::size_t operator()(skel_node& n);
protected:
    std::size_t res;
};

struct resources: public eval_visitor
{
    explicit resources(rpl_environment& env);
    void visit(seq_node& n) override;
    void visit(source_node& n) override;
    void visit(drain_node& n) override;
    void visit(comp_node& n) override;
    void visit(pipe_node& n) override;
    void visit(farm_node& n) override;
    void visit(map_node& n) override;
    void visit(reduce_node& n) override;
    void visit(dc_node& n) override;
    void visit(id_node& n) override;

    std::string print(skel_node& n) override;
    std::size_t operator()(skel_node& n);
protected:
    std::size_t res;
};

#endif
