#include "compseq_setter.hpp"
#include <iostream>

using namespace std;

bool result;

compseq_setter::compseq_setter(rpl_environment& env) :
    env(env)
{}

void compseq_setter::visit(seq_node& n) {
    result = true;
}

void compseq_setter::visit(source_node& n) {
    // reason: having a comp(source, ..., drain) program
    // compiled in a FF program with the ff_comp
    // auxiliary object will never work properly:
    // ff_comp doesn't implement the "run_and_wait_end"
    // method
    result = false;
}

void compseq_setter::visit(drain_node& n) {
    // same above
    result = false;
}

void compseq_setter::visit(comp_node& n) {
    bool rvar = true;
    for (size_t i = 0; i < n.size(); i++) {
        n.get(i)->accept(*this);
        rvar = rvar && result;
    }
    n.compseq = rvar;
}

void compseq_setter::visit(pipe_node& n) {
    for (size_t i = 0; i < n.size(); i++)
        n.get(i)->accept(*this);
    result = false;
}

void compseq_setter::visit(farm_node& n) {
    n.get(0)->accept(*this);
    result = false;
}

void compseq_setter::visit(map_node& n) {
    // we use a two-tier model: we don't care
    // to recurse
    result = false;
}

void compseq_setter::visit(reduce_node& n) {
    // as above
    result = false;
}

void compseq_setter::visit(id_node& n) {
    auto ptr = env.get(n.id, n.index);
    if (ptr != nullptr)
        ptr->accept(*this);
}

void compseq_setter::operator()(skel_node& n){
    result = false;
    n.accept(*this);
}
