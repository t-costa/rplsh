#include "ffmapper.hpp"
#include <vector>
#include <algorithm>
#include <iostream>

using namespace std;

/*
 *
 * Maps workers
 *
 */

ffmapper::ffmapper(rpl_environment& env) :
    env(env),
    startID(0),
    endID(0),
    res(env)
{}

/**
 * Pushes an incremental id in mw
 * @param n sequential node
 */
void ffmapper::visit(seq_node& n) {
    size_t id = startID++ % endID;
    mw.push_back(id);
}

/**
 * Pushes an incremental id in mw
 * @param n source node
 */
void ffmapper::visit(source_node& n) {
    size_t id = startID++ % endID;
    mw.push_back(id);
}

/**
 * Pushes an incremental id in mw
 * @param n drain node
 */
void ffmapper::visit(drain_node& n) {
    size_t id = startID++ % endID;
    mw.push_back(id);
}

/**
 * If the node is all sequential, pushes
 * an incremental id in mw; otherwise maps
 * ids to thread properly, calling the accept
 * on each node in n
 * @param n comp node
 */
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

/**
 * Calls the accept for every node in the pipe
 * @param n pipe node
 */
void ffmapper::visit(pipe_node& n) {
    for (size_t i = 0; i < n.size(); i++)
        n.get(i)->accept(*this);
}

/**
 * Pushes emitter, collector and all the nodes needed
 * for the pardegree
 * @param n
 */
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
/**
 * Pushes an incremental id in mw for each node in the map,
 * considering scatter and gather
 * @param n map node
 */
void ffmapper::visit(map_node& n) {
    // don't recurse here: two-tier model will
    // be applied if stream parallelism inside
    pardegree par(env);                         // setup pardegree visitor
    size_t numworkers = par(n) + 2;             // compute number of workers (considering scatter+gather)
    for (size_t i = 0; i < numworkers; i++)        // cpu mapping for all workers
        mw.push_back(startID++ % endID);
}

// FFCODE MUST IMPLEMENT PROPER OPTIMIZATION WHEN
// PRODUCING FF CODE -> two-tier by augmenting pardegree
/**
 * Pushes an incremental id in mw for each worker
 * @param n reduce node
 */
void ffmapper::visit(reduce_node& n) {
    // don't recurse here: two-tier model will
    // be applied if stream parallelism inside
    pardegree par(env);                         // setup pardegree visitor
    size_t numworkers = par(n) + 2;             // compute number of workers (considering scatter+gather)
    for (size_t i = 0; i < numworkers; i++)        // cpu mapping for all workers
        mw.push_back(startID++ % endID);
}

void ffmapper::visit(dc_node &n) {
    //TODO: non so se va bene
    pardegree par(env);
    size_t numworkers = par(n);
    for (size_t i = 0; i < numworkers; ++i) {
        mw.push_back(startID ++ %endID);
    }
}

/**
 * If the nodes exists in the environment, calls the accept,
 * otherwise error
 * @param n id node
 */
void ffmapper::visit(id_node& n) {
    auto ptr = env.get(n.id, n.index);
    if (ptr != nullptr)
        ptr->accept(*this);
    else
        cout << n.id << " whaaaat? in ffmapper::visit(id_node)" << endl;
}

/**
 * Clear member vector mw
 */
void ffmapper::clear() {
    mw.clear();
}

vector<size_t> ffmapper::get_worker_mapping() const {
    return mw;
}

/**
 * Starts collecting ids for all the nodes and sets
 * start and end id
 * @param n root of the skeleton tree
 * @return the mapping structure
 */
const ffmapper& ffmapper::operator()(skel_node& n){
    this->startID = 0;
    this->endID   = std::min(res(n), env.get_res());
    n.accept(*this);
    return *this;
}
