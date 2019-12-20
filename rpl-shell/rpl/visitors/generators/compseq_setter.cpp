#include "compseq_setter.hpp"
#include <iostream>

using namespace std;

bool result;

/*
 *
 * The only modification that I see is on the value of the
 * field n.compseq when n is a comp node, basically I think
 * it checks if the composition contains only seq nodes
 *
 * */

compseq_setter::compseq_setter(rpl_environment& env) :
    env(env)
{}

/**
 * If the visit is on a sequential node, the result is true
 * @param n seq node
 */
void compseq_setter::visit(seq_node& n) {
    result = true;
}

/**
 * The result is false
 * @param n source node
 */
void compseq_setter::visit(source_node& n) {
    // reason: having a comp(source, ..., drain) program
    // compiled in a FF program with the ff_comp
    // auxiliary object will never work properly:
    // ff_comp doesn't implement the "run_and_wait_end"
    // method
    result = false;
}

/**
 * The result is false
 * @param n drain node
 */
void compseq_setter::visit(drain_node& n) {
    // same above
    result = false;
}

/**
 * The result is true iff it's true for all the nodes
 * in the composition. The accept is called on every node
 * @param n comp node
 */
void compseq_setter::visit(comp_node& n) {
    bool rvar = true;
    for (size_t i = 0; i < n.size(); i++) {
        n.get(i)->accept(*this);
        rvar = rvar && result;
    }
    n.compseq = rvar;
}

/**
 * The accept is called on every node and the result is false
 * @param n pipe node
 */
void compseq_setter::visit(pipe_node& n) {
    for (size_t i = 0; i < n.size(); i++)
        n.get(i)->accept(*this);
    result = false;
}

/**
 * The accept is called on the first node in n and
 * the result is false
 * @param n farm node
 */
void compseq_setter::visit(farm_node& n) {
    n.get(0)->accept(*this);
    result = false;
}

/**
 * Two tier model, no recurse, result false
 * @param n map node
 */
void compseq_setter::visit(map_node& n) {
    // we use a two-tier model: we don't care
    // to recurse
    result = false;
}

/**
 * Two tier model, no recurse, result false
 * @param n reduce node
 */
void compseq_setter::visit(reduce_node& n) {
    // as above
    result = false;
}

/**
 * If the node exists in the environment, the accept
 * is called on the node
 * @param n id node
 */
void compseq_setter::visit(id_node& n) {
    auto ptr = env.get(n.id, n.index);
    if (ptr != nullptr)
        ptr->accept(*this);
}

/**
 * Result is set to false and the accept on the root is called
 * @param n root of the skeleton
 */
void compseq_setter::operator()(skel_node& n){
    result = false;
    n.accept(*this);
}
