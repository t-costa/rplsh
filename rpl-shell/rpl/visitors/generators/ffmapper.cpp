#include "ffmapper.hpp"
#include <vector>
#include <algorithm>
#include <iostream>

using namespace std;

ffmapper::ffmapper(rpl_environment& env) :
    env(env),
    res(env)
{}

void ffmapper::visit(seq_node& n) {
    size_t id = startID++ % endID;
    mw.push_back(id);
}

void ffmapper::visit(source_node& n) {
    size_t id = startID++ % endID;
    mw.push_back(id);
}

void ffmapper::visit(drain_node& n) {
    size_t id = startID++ % endID;
    mw.push_back(id);
}

void ffmapper::visit(comp_node& n) {
    // if n.compseq is true its children are
    // sequential nodes. We can use only one
    // ff_node (a single thread) instead use
    // multiple ff_node on the same pipe.
    if (n.compseq) {
        mw.push_back(startID++ % endID);
        return;
    }

    // if n.compseq is false one of the children
    // is parallel: so map ids to thread properly
    size_t _startID = startID;
    size_t _endID   = startID + res(n);
    for (size_t i = 0; i < n.size(); i++) {
        startID = _startID;
        n.get(i)->accept(*this);
    }
    startID = _endID;
}

void ffmapper::visit(pipe_node& n) {
    for (size_t i = 0; i < n.size(); i++)
        n.get(i)->accept(*this);
}

void ffmapper::visit(farm_node& n) {
    // emitter
    mw.push_back(startID++ % endID);

    // recurse n.pardegree times for assign cpu ids to the workers
    for (int i = 0; i < n.pardegree; i++)
        n.get(0)->accept(*this);

    // collector
    mw.push_back(startID++ % endID);
}

// FFCODE MUST IMPLEMENT PROPER OPTIMIZATION WHEN
// PRODUCING FF CODE -> two-tier by augmenting pardegree
void ffmapper::visit(map_node& n) {
    // don't recurse here: two-tier model will
    // be applied if stream parallelism inside
    pardegree par(env);                      // setup pardegree visitor
    size_t numworkers = par(n)+2;            // compute number of workers (considering scatter+gather)
    for (size_t i = 0; i < numworkers; i++)  // cpu mapping for all workers
        mw.push_back(startID++ % endID);
}

// FFCODE MUST IMPLEMENT PROPER OPTIMIZATION WHEN
// PRODUCING FF CODE -> two-tier by augmenting pardegree
void ffmapper::visit(reduce_node& n) {
    // don't recurse here: two-tier model will
    // be applied if stream parallelism inside
    pardegree par(env);                      // setup pardegree visitor
    size_t numworkers = par(n);              // compute number of workers
    for (size_t i = 0; i < numworkers; i++)  // cpu mapping for all workers
        mw.push_back(startID++ % endID);
}

void ffmapper::visit(id_node& n) {
    auto ptr = env.get(n.id, n.index);
    if (ptr != nullptr)
        ptr->accept(*this);
    else
        cout << n.id << " whaaaat? in ffmapper::visit(id_node)" << endl;
}

// clear member vector mw
void ffmapper::clear() {
    mw.clear();
}

vector<size_t> ffmapper::get_worker_mapping() const {
    return mw;
}

const ffmapper& ffmapper::operator()(skel_node& n){
    this->startID = 0;
    this->endID   = std::min(res(n), env.get_res());
    n.accept(*this);
    return *this;
}
