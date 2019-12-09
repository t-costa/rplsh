///////////////////////////////////////////////////////////////////////////////
//  Concrete visitors:
//  printer, service time, latency time, ...
///////////////////////////////////////////////////////////////////////////////

#ifndef rpl_visitor_hpp
#define rpl_visitor_hpp

#include "environment/rpl_environment.hpp"
#include "evaluators/evaluators.hpp"
#include "visitor_interface.hpp"
#include "utils/printable.hpp"

struct printer : public skel_visitor, public printable
{
    void visit(seq_node& n) override;
    void visit(source_node& n) override;
    void visit(drain_node& n) override;
    void visit(comp_node& n) override;
    void visit(pipe_node& n) override;
    void visit(farm_node& n) override;
    void visit(map_node& n) override;
    void visit(reduce_node& n) override;
    void visit(id_node& n) override;

    std::string print(skel_node& sk) override;
    std::string operator()(skel_node& sk);
private:
    std::string res;
    void tostring(const std::string& name, const skel_node& n);
};

struct ann_printer : public skel_visitor, public printable
{
    void visit(seq_node& n) override;
    void visit(source_node& n) override;
    void visit(drain_node& n) override;
    void visit(comp_node& n) override;
    void visit(pipe_node& n) override;
    void visit(farm_node& n) override;
    void visit(map_node& n) override;
    void visit(reduce_node& n) override;
    void visit(id_node& n) override;

    std::string print(skel_node& sk) override;
    std::string operator()(skel_node& sk);
private:
    std::string res;
    void tostring(const std::string& name, const skel_node& n);
    void tostring(const std::string& name, const std::string& ann, const skel_node& n);
};

// implementation of a visitor
struct label_printer : public skel_visitor {
    void visit(seq_node& n) override;
    void visit(source_node& n) override;
    void visit(drain_node& n) override;
    void visit(comp_node& n) override;
    void visit(pipe_node& n) override;
    void visit(farm_node& n) override;
    void visit(map_node& n) override;
    void visit(reduce_node& n) override;
    void visit(id_node& n) override;
    std::string operator()(skel_node& n);
private:
    std::string str;
};

struct single_node_cloner : public skel_visitor {
    void visit(seq_node& n) override;
    void visit(source_node& n) override;
    void visit(drain_node& n) override;
    void visit(comp_node& n) override;
    void visit(pipe_node& n) override;
    void visit(farm_node& n) override;
    void visit(map_node& n) override;
    void visit(reduce_node& n) override;
    void visit(id_node& n) override;
    skel_node* operator()(skel_node& n);
private:
    skel_node* tmp;
};

struct reduce_resources : public skel_visitor {
    explicit reduce_resources( rpl_environment& env );
    void visit(seq_node& n) override;
    void visit(source_node& n) override;
    void visit(drain_node& n) override;
    void visit(comp_node& n) override;
    void visit(pipe_node& n) override;
    void visit(farm_node& n) override;
    void visit(map_node& n) override;
    void visit(reduce_node& n) override;
    void visit(id_node& n) override;
    bool operator()(skel_node& n);
    bool subexp;
private:
    bool res;
    servicetime ts;
    resources getres;
    rpl_environment& env;
};

struct assign_resources : public skel_visitor {
    void visit(seq_node& n) override;
    void visit(comp_node& n) override;
    void visit(pipe_node& n) override;
    void visit(farm_node& n) override;
    void visit(map_node& n) override;
    void visit(reduce_node& n) override;
    void visit(id_node& n) override;
    void operator()(skel_node& n, double inputsize);
};

struct get_seq_wrappers : public skel_visitor {
    explicit get_seq_wrappers(rpl_environment& env);
    void visit(seq_node& n) override;
    void visit(source_node& n) override;
    void visit(drain_node& n) override;
    void visit(comp_node& n) override;
    void visit(pipe_node& n) override;
    void visit(farm_node& n) override;
    void visit(map_node& n) override;
    void visit(reduce_node& n) override;
    void visit(id_node& n) override;

    std::vector<seq_node*> get_seq_nodes();
    std::vector<drain_node*> get_drain_nodes();
    std::vector<source_node*> get_source_nodes();

    void operator()(skel_node& n);
private:
    rpl_environment& env;
    std::vector<seq_node*> seq_nodes;
    std::vector<drain_node*> drn_nodes;
    std::vector<source_node*> src_nodes;
};

struct top_datap_skeletons : public skel_visitor {
    explicit top_datap_skeletons(rpl_environment& env);
    void visit(seq_node& n) override;
    void visit(source_node& n) override;
    void visit(drain_node& n) override;
    void visit(comp_node& n) override;
    void visit(pipe_node& n) override;
    void visit(farm_node& n) override;
    void visit(map_node& n) override;
    void visit(reduce_node& n) override;
    void visit(id_node& n) override;

    std::vector<map_node*> get_map_nodes();
    std::vector<reduce_node*> get_reduce_nodes();

    void operator()(skel_node& n);
private:
    rpl_environment& env;
    std::vector<map_node*> map_nodes;
    std::vector<reduce_node*> red_nodes;
};

struct ranker : public skel_visitor {
    explicit ranker(rpl_environment& env);
    void visit(seq_node& n) override;
    void visit(comp_node& n) override;
    void visit(pipe_node& n) override;
    void visit(farm_node& n) override;
    void visit(map_node& n) override;
    void visit(reduce_node& n) override;
    void visit(id_node& n) override;
    void operator()(skel_node& n);
private:
    rpl_environment& env;
    single_node_cloner snc;
};

struct unranker : public skel_visitor {
    explicit unranker(rpl_environment& env);
    void visit(seq_node& n) override;
    void visit(comp_node& n) override;
    void visit(pipe_node& n) override;
    void visit(farm_node& n) override;
    void visit(map_node& n) override;
    void visit(reduce_node& n) override;
    void visit(id_node& n) override;
    void operator()(skel_node& n);
private:
    rpl_environment& env;
};

struct count_stages : public skel_visitor {
    explicit count_stages(rpl_environment& env);
    void visit(seq_node& n) override;
    void visit(source_node& n) override;
    void visit(drain_node& n) override;
    void visit(comp_node& n) override;
    void visit(pipe_node& n) override;
    void visit(farm_node& n) override;
    void visit(map_node& n) override;
    void visit(reduce_node& n) override;
    void visit(id_node& n) override;
    std::size_t operator()(skel_node& n);
private:
    rpl_environment& env;
    std::size_t res;
};

#endif
