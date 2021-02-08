#include "evaluators.hpp"
#include "nodes/skeletons.hpp"
#include "visitors/visitors.hpp"
#include <iostream>
#include <algorithm>
#include <tuple>
#include <cmath>

using namespace std;

///////////////////////////////////////////////////////////////////////////////

/**
 * For each child of n, computes the operation op
 * on the children after the visit, accumulating the result
 * @tparam Visitor type of the visitor
 * @tparam BinaryOp type of the function
 * @param v visitor for the children
 * @param n skeleton node
 * @param op function to be computed
 * @return the accumulated result
 */
template <typename Visitor, typename BinaryOp>
double compute(Visitor& v, skel_node& n, BinaryOp op) {
    double ts = 0;
    for (size_t i = 0; i < n.size() ; i++)
        ts = op(ts, v(*n.get(i)));
    return ts;
}

///////////////////////////////////////////////////////////////////////////////

eval_visitor::eval_visitor(rpl_environment& env) :
    env( env ), global_inputsize(1)
{}

///////////////////////////////////////////////////////////////////////////////

servicetime::servicetime(rpl_environment& env) :
    eval_visitor( env ), res (0), start(true),
    computing_reduce(false),
    partial_res_map(0), partial_res_red(0)
{}

/**
 * Sets the value of res
 * @param n sequential node
 */
void servicetime::visit(seq_node& n) {
    if (n.datap_flag)
        res = n.servicetime * global_inputsize;
    else
        res = n.servicetime;
}

/**
 * Sets the value of res
 * @param n source node
 */
void servicetime::visit(source_node& n) {
    res = n.servicetime;
}

/**
 * Sets the value of res
 * @param n drain node
 */
void servicetime::visit(drain_node& n) {
    res = n.servicetime;
}

/**
 * Computes the sum of the service time of the children and sets res
 * @param n comp node
 */
void servicetime::visit(comp_node& n) {
    if (computing_reduce) {
        //only the last one is reduce, the others are map
        for (size_t i=0; i<n.size()-1; ++i) {
            partial_res_map += (*this)(*n.get(i));
        }
        //the last one is for reduce
        partial_res_red = (*this)(*n.get(n.size()-1));
    } else {
        res = compute(*this, n, std::plus<double>());
    }
}

/**
 * Computes the maximum of the service time of the children and sets res
 * @param n pipe node
 */
void servicetime::visit(pipe_node& n) {
    res = compute(*this, n, [](double a, double b){return std::max(a, b);});
}

/**
 * Calls the visit on the child and sets res
 * @param n farm node
 */
void servicetime::visit(farm_node& n) {
    res = std::max((*this)(*n.get(0)) / n.pardegree, env.get_emitter_time());
}

/**
 * Calls the visit on the child and sets res
 * ASSUMPTION: inputsize has been already propagated
 * @param n map node
 */
void servicetime::visit(map_node& n) {
    res = std::max((*this)(*n.get(0)) / n.pardegree, env.get_scatter_time());
}

/**
 * Calls the visit on the child and sets res
 * ASSUMPTION: input size has been already propagated
 * @param n reduce node
 */
void servicetime::visit(reduce_node& n) {
    // assuming Tf is the servicetime  of the reduce function f (datap):
    // ts(n) = Tf / nw + log2(nw) * Tf
    //trying empirically it seems pretty much the same as map...

//    res = std::max((*this)(*n.get(0)) / n.pardegree, env.get_scatter_time());

    int nw = n.pardegree;
    computing_reduce = true;
    partial_res_red = 0;
    partial_res_map = 0;
    auto res_child = (*this)(*n.get(0));
    double ts_map = 0, ts_red = 0;

    if (partial_res_map != 0 || partial_res_red != 0) {
        //i think it's comp case
        ts_map = partial_res_map/nw;
        ts_red = partial_res_red/nw + partial_res_red*log2(nw)/global_inputsize;
    } else {
        //reduce(seq)
        ts_red = res_child/nw + res_child*log2(nw)/global_inputsize;
    }

    //clean
    computing_reduce = false;
    partial_res_red = 0;
    partial_res_map = 0;

    res = std::max(ts_map+ts_red, env.get_scatter_time());

//    int nw = n.pardegree;
//    res = (*this)(*n.get(0));            // res == Tf
//    res = res/nw + log2(nw) * res;
//    the inputsize for nodes after the reduce is 1
    global_inputsize = 1;
}

/**
 * Calls the visit on the child and sets res
 * ASSUMPTION: inputsize has been already propagated
 * @param n dc node
 */
void servicetime::visit(dc_node &n) {
    //like a map but implemented with a farm
    res = std::max((*this)(*n.get(0)) / n.pardegree, env.get_emitter_time());
}

/**
 * Searches for the node in the environment,
 * propagates input size and sets res calling the visit
 * @param n id node
 */
void servicetime::visit(id_node& n) {
    try {
        // we need to assign resources before
        auto ptr = env.get(n.id, n.index);
        assign_resources assignres;
//        assignres(*ptr, n.inputsize);
        assignres(*ptr, global_inputsize);
        res = (*this)(*ptr);
    } catch (out_of_range& e) {
        cout << "error, not found: " << n.id << endl;
    }
}

/**
 * @param sk skeleton node
 * @return a string representation of the service time of sk
 */
string servicetime::print( skel_node& sk ){
    //reset start for future calls
    start = true;
    return std::to_string( (*this)( sk ) );
}

/**
 * Starts the visit
 * @param sk skeleton root
 * @return the value of the service time of sk
 */
double servicetime::operator()(skel_node& sk){
    if (start) {
        //inputsize unchanged until reduce
        global_inputsize = sk.inputsize;
        start = false;
    }
    sk.accept(*this);
    return res;
}

void servicetime::reset_start() {
    start = true;
}

///////////////////////////////////////////////////////////////////////////////

latencytime::latencytime(rpl_environment& env) :
    eval_visitor( env ), res (0), start(true),
    computing_reduce(false),
    partial_res_map(0), partial_res_red(0)
{}

/**
 * Sets the value for res
 * @param n seq node
 */
void latencytime::visit(seq_node& n) {
    if (n.datap_flag)
        res = n.servicetime * global_inputsize;
    else
        res = n.servicetime;
}

/**
 * Sets the value for res
 * @param n source node
 */
void latencytime::visit(source_node& n) {
    res = n.servicetime;
}

/**
 * Sets the value for res
 * @param n drain node
 */
void latencytime::visit(drain_node& n) {
    res = n.servicetime;
}

/**
 * Computes the sum of the latencies of the children and sets res
 * @param n comp node
 */
void latencytime::visit(comp_node& n) {
    if (computing_reduce) {
        //only the last one is reduce, the others are map
        for (size_t i=0; i<n.size()-1; ++i) {
            partial_res_map += (*this)(*n.get(i));
        }
        //the last one is for reduce
        partial_res_red = (*this)(*n.get(n.size()-1));
    } else {
        res = compute(*this, n, std::plus<double>());
    }
}

/**
 * Computes the sum of the latencies of the children and sets res
 * @param n
 */
void latencytime::visit(pipe_node& n) {
    res = compute(*this, n, std::plus<double>());
}

/**
 * Sets res as sum of emitter, child and gather
 * @param n farm node
 */
void latencytime::visit(farm_node& n) {
    res = env.get_emitter_time() + (*this)( *n.get(0) ) + env.get_collector_time();
}

/**
 * Sets res as sum of emitter, child and gather
 * @param n map node
 */
void latencytime::visit(map_node& n) {
    res = env.get_scatter_time() + (*this)( *n.get(0) ) / n.pardegree + env.get_gather_time();
}

/**
 * Calls the visit on the child and sets res
 * ASSUMPTION: inputsize has been already propagated
 * @param n reduce node
 */
void latencytime::visit(reduce_node& n) {
    //just like ts

    int nw = n.pardegree;
    computing_reduce = true;
    partial_res_red = 0;
    partial_res_map = 0;
    auto res_child = (*this)(*n.get(0));
    double ts_map = 0, ts_red = 0;

    if (partial_res_map != 0 || partial_res_red != 0) {
        //i think it's comp case
        ts_map = partial_res_map/nw;
        ts_red = partial_res_red/nw + partial_res_red*log2(nw)/global_inputsize;
    } else {
        //reduce(seq)
        ts_red = res_child/nw + res_child*log2(nw)/global_inputsize;
    }

    //clean
    computing_reduce = false;
    partial_res_red = 0;
    partial_res_map = 0;

    res = std::max(ts_map+ts_red, env.get_scatter_time());

    global_inputsize = 1;
}

/**
 * Calls the visit on the child and sets res
 * ASSUMPTION: inputsize has been already propagated
 * @param n dc node
 */
void latencytime::visit(dc_node &n) {
    //like a map, but implemented with a farm
    res = env.get_emitter_time() + (*this)( *n.get(0) ) / n.pardegree + env.get_scatter_time();
}

/**
 * Searches for n in the environment, propagates inputsize
 * and calls the visit assigning res
 * @param n id node
 */
void latencytime::visit(id_node& n) {
    try {
        auto ptr = env.get(n.id, n.index);
        assign_resources assignres;
//        assignres(*ptr, n.inputsize);
        assignres(*ptr, global_inputsize);
        res = (*this)(*ptr);
    } catch (out_of_range& e) {
        cout << "error, not found: " << n.id << endl;
    }
}

/**
 * @param sk skeleton node
 * @return a string representation of the latency of sk
 */
string latencytime::print( skel_node& sk ){
    start = true;
    return std::to_string( (*this)( sk ) );
}

/**
 * Starts the visit on sk
 * @param sk skeleton root
 * @return value of latency of sk
 */
double latencytime::operator()(skel_node& sk) {
    if (start) {
        global_inputsize = sk.inputsize;
        start = false;
    }
    sk.accept(*this);
    return res;
}

void latencytime::reset_start() {
    start = true;
}

///////////////////////////////////////////////////////////////////////////////

completiontime::completiontime( rpl_environment& env ) :
    eval_visitor( env ),
    lat( env ),
    ts( env ),
    res (0),
    start (true)
{}

/**
 * Sets res
 * @param n seq node
 */
void completiontime::visit( seq_node& n ) {
    if (n.datap_flag)
        res = n.servicetime * global_inputsize;
    else
        res = n.servicetime;

    res *= env.get_dim();
}

/**
 * Sets res
 * @param n source node
 */
void completiontime::visit(source_node& n) {
    res = n.servicetime * env.get_dim();
}

/**
 * Sets res
 * @param n drain node
 */
void completiontime::visit(drain_node& n) {
    res = n.servicetime * env.get_dim();
}

/**
 * Computes the completion time of the children and sets res
 * @param n
 */
void completiontime::visit( comp_node& n ) {
    res = compute(*this, n, std::plus<double>());
}

/**
 * Counts the stages and service time of the pipe and sets res
 * @param n pipe node
 */
void completiontime::visit( pipe_node& n ) {
    /* ts*(num_stages-1) + ts*dimension */
    /* more precise: (dimension-1)*Ts + L */
    ts.reset_start();
    lat.reset_start();
    double t = ts(n);
    double l = lat(n);
//    count_stages cs(env);
//    res = static_cast<double>(cs(n)-1)*t + env.get_dim()*t;
    res = std::max(((long) env.get_dim() - 1), (long) 0) * t + l;
}

/**
 * Computes the completion time of n and sets res
 * @param n farm node
 */
void completiontime::visit( farm_node& n ) {
    //    res = static_cast<double>(env.get_dim())/n.pardegree*lat(n);  //old definition (I think wrong)
    // approx for long stream
    ts.reset_start();
    res = env.get_emitter_time() +  env.get_dim()*ts(n) + env.get_collector_time();
}

/**
 * Computes completion time of n and sets res
 * @param n map node
 */
void completiontime::visit( map_node& n ) {
    //res = env.get_dim() * lat(n); //old definition (I think wrong)
    ts.reset_start();
    res = env.get_scatter_time() + env.get_dim() * ts(n) + env.get_gather_time();
}

/**
 * Computes latency of n and sets res
 * @param n reduce node
 */
void completiontime::visit( reduce_node& n ) {
    //res = env.get_dim() * lat(n); //old definition (I think wrong)
    ts.reset_start();
    res = env.get_scatter_time() + env.get_dim() * ts(n) + env.get_gather_time();
    global_inputsize = 1;
}

/**
 * Computes completion time of n and sets res
 * @param n dc node
 */
void completiontime::visit(dc_node &n) {
    //like a map but implemented with farm
    ts.reset_start();
    res = env.get_emitter_time() +  env.get_dim()*ts(n) + env.get_collector_time();
}

/**
 * Searches for n in the environment, propagates
 * inputsize, starts the visit on n and sets res
 * @param n id node
 */
void completiontime::visit( id_node& n ) {
    try {
        auto ptr = env.get(n.id, n.index);
        assign_resources assignres;
//        assignres(*ptr, n.inputsize);
        assignres(*ptr, global_inputsize);
        res = (*this)(*ptr);
    } catch (out_of_range& e) {
        cout << "error, not found: " << n.id << endl;
    }
}

/**
 * @param n skeleton node
 * @return returns a string representation of the completion time of sk
 */
string completiontime::print( skel_node& n ){
    start = true;
    return std::to_string( (*this)( n ));
}

/**
 * Starts the visit
 * @param n skeleton root
 * @return value of completion time for n
 */
double completiontime::operator()(skel_node& n){
    if (start) {
        global_inputsize = n.inputsize;
        start = false;
    }
    n.accept(*this);
    return res;
}

///////////////////////////////////////////////////////////////////////////////

pardegree::pardegree( rpl_environment& env ) :
    eval_visitor( env ), res (0)
{}

/**
 * Sets res to 1
 * @param n seq node
 */
void pardegree::visit( seq_node& n ) {
    res = 1;
}

/**
 * Sets res to 1
 * @param n source node
 */
void pardegree::visit( source_node& n ) {
    res = 1;
}

/**
 * Sets res to 1
 * @param n drain node
 */
void pardegree::visit( drain_node& n ) {
    res = 1;
}

/**
 * Computes the sum of pardegree of children and sets res
 * @param n comp node
 */
void pardegree::visit( comp_node& n ) {
    res = compute(*this, n, [](double a, double b){return std::max(a, b);});
}

/**
 * Computes the sum of pardegree of children and sets res
 * @param n pipe node
 */
void pardegree::visit( pipe_node& n ) {
    res = compute(*this, n, std::plus<size_t>());
}

/**
 * Computes pardegree of child and sets res
 * @param n farm node
 */
void pardegree::visit( farm_node& n ) {
    res = n.pardegree * (*this)(*n.get(0));
}

/**
 * Computes pardegree of child and sets res
 * @param n map node
 */
void pardegree::visit( map_node& n ) {
    res = n.pardegree * (*this)(*n.get(0));
}

/**
 * Computes pardegree of child and sets res
 * @param n reduce node
 */
void pardegree::visit( reduce_node& n ) {
    res = n.pardegree * (*this)(*n.get(0));
}

/**
 * Computes pardegree of child and sets res
 * @param n dc node
 */
void pardegree::visit(dc_node &n) {
    res = n.pardegree * (*this)(*n.get(0));
}

/**
 * Searches n in the environment, starts the visit
 * and sets res
 * @param n id node
 */
void pardegree::visit( id_node& n ) {
    try {
        auto ptr = env.get(n.id, n.index);
        res = (*this)(*ptr);
    } catch (out_of_range& e) {
        cout << "error, not found: " << n.id << endl;
    }
}

/**
 * @param n skeleton node
 * @return a string representation of the parallelism degree fo n
 */
string pardegree::print( skel_node& n ){
    return std::to_string( (*this)( n ) );
}

/**
 * Starts the visit
 * @param n skeleton root
 * @return value of the parallelism degree
 */
size_t pardegree::operator()(skel_node& n){
    n.accept(*this);
    return res;
}

///////////////////////////////////////////////////////////////////////////////

resources::resources( rpl_environment& env ) :
    eval_visitor( env ), res (0)
{}

/**
 * Sets res to 1
 * @param n seq node
 */
void resources::visit( seq_node& n ) {
    res = 1;
}

/**
 * Sets res to 1
 * @param n source node
 */
void resources::visit( source_node& n ) {
    res = 1;
}

/**
 * Sets res to 1
 * @param n drain node
 */
void resources::visit( drain_node& n ) {
    res = 1;
}

/**
 * Computes the max of the children and sets res
 * @param n comp node
 */
void resources::visit( comp_node& n ) {
    res = compute(*this, n, [](size_t a, size_t b){return std::max(a, b);});
}

/**
 * Computes the sum of the resources of the children and sets res
 * @param n pipe node
 */
void resources::visit( pipe_node& n ) {
    res = compute(*this, n, std::plus<size_t>());
}

/**
 * Computes the number of resources of the child and sets res
 * @param n farm node
 */
void resources::visit( farm_node& n ) {
    res = n.pardegree * (*this)(*n.get(0)) + 2; //emitter + collector
}

/**
 * Computes the number of resources of the child and sets res
 * @param n map node
 */
void resources::visit( map_node& n ) {
    res = n.pardegree * (*this)(*n.get(0)) + 2; //scatter + gather
    if (n.grain > 0) res++; //dynamic scheduler
}

/**
 * Computes the number of resources of the child and sets res
 * @param n reduce node
 */
void resources::visit( reduce_node& n ) {
    res = n.pardegree * (*this)(*n.get(0)) + 2; //scatter + gather
    if (n.grain > 0) res++; //dynamic scheduler
}

/**
 * Computes the number of resources of the child and sets res
 * @param n dc node
 */
void resources::visit(dc_node &n) {
    res = n.pardegree * (*this)(*n.get(0)) + 2; //technically emitter+collector
}

/**
 * Searches n in the environment, starts the visit and sets res
 * @param n id node
 */
void resources::visit( id_node& n ) {
    try {
        auto ptr = env.get(n.id, n.index);
        res = (*this)(*ptr);
    } catch (out_of_range& e) {
        cout << "error, not found: " << n.id << endl;
    }
}

/**
 * @param n skeleton node
 * @return a string representation of the resources of n
 */
string resources::print( skel_node& n ){
    return std::to_string( (*this)( n ) );
}

/**
 * Starts the visit
 * @param n skeleton root
 * @return Number of used resources
 */
size_t resources::operator()(skel_node& n){
    n.accept(*this);
    return res;
}
