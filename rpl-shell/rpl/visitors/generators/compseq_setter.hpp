#ifndef rpl_compseq_setter_hpp
#define rpl_compseq_setter_hpp

#include "visitors/visitor_interface.hpp"
#include "visitors/evaluators/evaluators.hpp"
#include "environment/rpl_environment.hpp"

struct compseq_setter : public skel_visitor {
public:
    explicit compseq_setter(rpl_environment& env);
    void visit(seq_node& n) override;
    void visit(source_node& n) override;
    void visit(drain_node& n) override;
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

#endif
