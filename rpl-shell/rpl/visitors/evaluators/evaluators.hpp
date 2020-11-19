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
};

//TODO: con il grain map questo cambia?????
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
    void visit(id_node& n) override;

    std::string print(skel_node& n) override;
    double operator()(skel_node& n);
private:
    double res;
};

//TODO: con il grain map questo cambia?????
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
    void visit(id_node& n) override;

    std::string print(skel_node& n) override;
    double operator()(skel_node& n);
protected:
    double res;
};

//TODO: con il grain map questo cambia?????
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
    void visit(id_node& n) override;

    std::string print(skel_node& n) override;
    double operator()(skel_node& n);
protected:
    latencytime lat;
    servicetime ts;
    double res;
};

//TODO: con il grain map questo NON dovrebbe cambiare
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
    void visit(id_node& n) override;

    std::string print(skel_node& n) override;
    std::size_t operator()(skel_node& n);
protected:
    std::size_t res;
};

//TODO: con il grain map questo NON dovrebbe cambiare
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
    void visit(id_node& n) override;

    std::string print(skel_node& n) override;
    std::size_t operator()(skel_node& n);
protected:
    std::size_t res;
};

#endif
