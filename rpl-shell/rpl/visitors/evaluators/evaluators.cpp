#include "nodes/skeletons.hpp"
#include "evaluators.hpp"
#include <iostream>
#include <functional>
#include <algorithm>
#include <tuple>
#include <cmath>

using namespace std;

///////////////////////////////////////////////////////////////////////////////

template <typename Visitor, typename BinaryOp>
double compute(Visitor& v, skel_node& n, BinaryOp op) {
    double ts = 0;
    for (size_t i = 0; i < n.size() ; i++)
        ts = op(ts, v(*n.get(i)));
    return ts;
}

///////////////////////////////////////////////////////////////////////////////

eval_visitor::eval_visitor(rpl_environment& env) :
    env( env )
{}

///////////////////////////////////////////////////////////////////////////////

servicetime::servicetime(rpl_environment& env) :
    eval_visitor( env )
{}

void servicetime::visit(seq_node& n) {
    res = n.servicetime;
}

void servicetime::visit(comp_node& n) {
    res = compute(*this, n, std::plus<double>());
}

void servicetime::visit(pipe_node& n) {
    res = compute(*this, n, [](double a, double b){return std::max(a, b);});
}

void servicetime::visit(farm_node& n) {
    res = (*this)(*n.get(0)) / n.pardegree;
}

void servicetime::visit(map_node& n) {
    res = (*this)(*n.get(0)) / n.pardegree;
}

// TODO WRONG -> for reduce is important the dim of input
void servicetime::visit(reduce_node& n) {
    res = log2( (*this)(*n.get(0)) );
}

void servicetime::visit(id_node& n) {
    try {
        auto ptr = env.get(n.id);
        res = (*this)(*ptr);
    } catch (out_of_range& e) {
        //TODO handle in a better way
        cout << "error, not found: " << n.id << endl;
    }
}

string servicetime::print( skel_node& sk ){
    return std::to_string( (*this)( sk ) );
}

double servicetime::operator()(skel_node& sk){
    sk.accept(*this);
    return res;
}

///////////////////////////////////////////////////////////////////////////////

latencytime::latencytime(rpl_environment& env) :
    eval_visitor( env )
{}


void latencytime::visit(seq_node& n) {
    res = n.servicetime;
}

void latencytime::visit(comp_node& n) {
    res = compute(*this, n, std::plus<double>());
}

void latencytime::visit(pipe_node& n) {
    res = compute(*this, n, std::plus<double>());
}

void latencytime::visit(farm_node& n) {
    res = (*this)( *n.get(0) );
}

void latencytime::visit(map_node& n) {
    res = (*this)( *n.get(0) ) / n.pardegree;
}

// TODO WRONG -> for reduce is important the dim of input
void latencytime::visit(reduce_node& n) {
    res = log2( (*this)( *n.get(0) ) );
}

void latencytime::visit(id_node& n) {
    try {
        auto ptr = env.get(n.id);
        res = (*this)(*ptr);
    } catch (out_of_range& e) {
        //TODO handle in a better way
        cout << "error, not found: " << n.id << endl;
    }
}

string latencytime::print( skel_node& sk ){
    return std::to_string( (*this)( sk ) );
}

double latencytime::operator()(skel_node& sk){
    sk.accept(*this);
    return res;
}

///////////////////////////////////////////////////////////////////////////////

// TODO here the stream size is something "global"
// user should set it through some shell verbs
// like "set_dimension(_)" and get with "get_dimension"
// better ideas?

completiontime::completiontime( rpl_environment& env ) :
    eval_visitor( env ),
    lat( env ),
    ts( env )
{}

void completiontime::visit( seq_node& n ) {
    res = n.servicetime;
}

void completiontime::visit( comp_node& n ) {
    res = compute(*this, n, std::plus<double>());
}

void completiontime::visit( pipe_node& n ) {
    /* latency of the first stage + ts*dimension */
    res = lat(*n.get(0)) + env.get_dim() * ts(n);
}

void completiontime::visit( farm_node& n ) {
    res = env.get_dim() / n.pardegree * lat(n);
}

void completiontime::visit( map_node& n ) {
    res = env.get_dim() / n.pardegree * ts(n);
}

void completiontime::visit( reduce_node& n ) {
    res = lat(n);
}

void completiontime::visit( id_node& n ) {
    try {
        auto ptr = env.get(n.id);
        res = (*this)(*ptr);
    } catch (out_of_range& e) {
        //TODO handle in a better way
        cout << "error, not found: " << n.id << endl;
    }
}

string completiontime::print( skel_node& n ){
    return std::to_string( (*this)( n ));;
}

double completiontime::operator()(skel_node& n){
    n.accept(*this);
    return res;
}

///////////////////////////////////////////////////////////////////////////////

pardegree::pardegree( rpl_environment& env ) :
    eval_visitor( env )
{}

void pardegree::visit( seq_node& n ) {
    res = 1;
}

void pardegree::visit( comp_node& n ) {
    res = compute(*this, n, [](double a, double b){return std::max(a, b);});
}

void pardegree::visit( pipe_node& n ) {
    res = compute(*this, n, std::plus<size_t>());
}

void pardegree::visit( farm_node& n ) {
    res = n.pardegree * (*this)(*n.get(0));
}

void pardegree::visit( map_node& n ) {
    res = n.pardegree * (*this)(*n.get(0));
}

// TODO also reduce should be have a configurable
// parallelism degree like farm and map!!
void pardegree::visit( reduce_node& n ) {
    res = 1;
}

void pardegree::visit( id_node& n ) {
    try {
        auto ptr = env.get(n.id);
        res = (*this)(*ptr);
    } catch (out_of_range& e) {
        //TODO handle in a better way
        cout << "error, not found: " << n.id << endl;
    }
}

string pardegree::print( skel_node& n ){
    return std::to_string( (*this)( n ) );;
}

size_t pardegree::operator()(skel_node& n){
    n.accept(*this);
    return res;
}

///////////////////////////////////////////////////////////////////////////////

resources::resources( rpl_environment& env ) :
    eval_visitor( env )
{}

void resources::visit( seq_node& n ) {
    res = 1;
}

void resources::visit( comp_node& n ) {
    res = compute(*this, n, [](size_t a, size_t b){return std::max(a, b);});
}

void resources::visit( pipe_node& n ) {
    res = compute(*this, n, std::plus<size_t>());
}

void resources::visit( farm_node& n ) {
    res = n.pardegree * (*this)(*n.get(0)) + 2;  //emitter + collector
}

void resources::visit( map_node& n ) {
    res = n.pardegree * (*this)(*n.get(0)) + 2;  //scatter + gather
}

// TODO also reduce should be have a configurable
// parallelism degree like farm and map!!
void resources::visit( reduce_node& n ) {
    res = 1;
}

void resources::visit( id_node& n ) {
    try {
        auto ptr = env.get(n.id);
        res = (*this)(*ptr);
    } catch (out_of_range& e) {
        //TODO handle in a better way
        cout << "error, not found: " << n.id << endl;
    }
}

string resources::print( skel_node& n ){
    return std::to_string( (*this)( n ) );;
}

size_t resources::operator()(skel_node& n){
    n.accept(*this);
    return res;
}