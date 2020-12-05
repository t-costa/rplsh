#ifndef rpl_ffmapper_hpp
#define rpl_ffmapper_hpp

#include "visitors/visitor_interface.hpp"
#include "visitors/evaluators/evaluators.hpp"
#include "environment/rpl_environment.hpp"

struct ffmapper : public skel_visitor {
public:
    explicit ffmapper(rpl_environment& env);
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

    void clear();
    std::vector<std::size_t> get_worker_mapping() const;
    const ffmapper& operator()(skel_node& n);

private:
    std::vector<std::size_t> mw;
    rpl_environment& env;
    std::size_t startID;
    std::size_t endID;
    resources res;
};

#endif
