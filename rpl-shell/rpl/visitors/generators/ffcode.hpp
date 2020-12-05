#ifndef rpl_ffcode_hpp
#define rpl_ffcode_hpp

#include "visitors/visitor_interface.hpp"
#include "visitors/visitors.hpp"
#include "environment/rpl_environment.hpp"
#include "compseq_setter.hpp"
#include <queue>
#include <utility>
#include <sstream>
#include <string>

struct ffcode : public skel_visitor {
    explicit ffcode(rpl_environment& env);

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

    std::string operator()(skel_node& n);

private:
    void comp_pipe(const std::string& type, const std::string& name, skel_node& n);
    void seq_wraps(const std::string& name);

protected:
    rpl_environment& env;
    get_seq_wrappers gsw;
    top_datap_skeletons tds;
    compseq_setter compseq;

    std::queue<std::pair<std::string,std::string>> code_lines;
};

#endif
