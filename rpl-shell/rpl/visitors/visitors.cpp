#include "nodes/skeletons.hpp"
#include "visitors.hpp"
#include "utils/rank.hpp"
#include <iostream>
#include <functional>
#include <algorithm>
#include <tuple>
#include <cmath>

using namespace std;

///////////////////////////////////////////////////////////////////////////////

void printer::tostring(const string& name, const skel_node& n) {
    res += name + "(";
    for (size_t i = 0; i < n.size(); i++) {
        n.get(i)->accept(*this);
        res += (i != n.size()-1 ? "," : "");
    }
    res += ")";
}

void printer::visit(seq_node& n) {
    tostring("seq", n);
}

void printer::visit(source_node& n) {
    tostring("source", n);
}

void printer::visit(drain_node& n) {
    tostring("drain", n);
}

void printer::visit(comp_node& n) {
    tostring("comp", n);
}

void printer::visit(pipe_node& n) {
    tostring("pipe", n);
}

void printer::visit(farm_node& n) {
    tostring("farm", n);
}

void printer::visit(map_node& n) {
    tostring("map", n);
}

void printer::visit(reduce_node& n) {
    tostring("reduce", n);
}

void printer::visit(id_node& n) {
    res += n.id;
}

string printer::print( skel_node& sk ){
    return (*this)( sk );
}

string printer::operator()(skel_node& sk){
    res.clear();
    sk.accept(*this);
    return res;
}

///////////////////////////////////////////////////////////////////////////////

void ann_printer::tostring(const string& name, const string& ann, const skel_node& n) {
    res += name + "(";
    for (size_t i = 0; i < n.size(); i++) {
        n.get(i)->accept(*this);
        res += (i != n.size()-1 ? "," : "");
    }
    res = res + ") " + ann;
}

void ann_printer::tostring(const string& name, const skel_node& n) {
    res += name + "(";
    for (size_t i = 0; i < n.size(); i++) {
        n.get(i)->accept(*this);
        res += (i != n.size()-1 ? "," : "");
    }
    res += ")";
}

void ann_printer::visit(seq_node& n) {
    tostring("seq", "with [ ts: " + to_string(n.servicetime) + "]", n);
}

void ann_printer::visit(source_node& n) {
    tostring("source", "with [ ts: " + to_string(n.servicetime) + "]", n);
}

void ann_printer::visit(drain_node& n) {
    tostring("drain", "with [ ts: " + to_string(n.servicetime) + "]", n);
}

void ann_printer::visit(comp_node& n) {
    tostring("comp", n);
}

void ann_printer::visit(pipe_node& n) {
    tostring("pipe", n);
}

void ann_printer::visit(farm_node& n) {
    tostring("farm", "with [ nw: " + to_string(n.pardegree) + "]", n);
}

void ann_printer::visit(map_node& n) {
    tostring("map", "with [ nw: " + to_string(n.pardegree) + "]", n);
}

void ann_printer::visit(reduce_node& n) {
    tostring("reduce", "with [ nw: " + to_string(n.pardegree) + "]", n);
}

void ann_printer::visit(id_node& n) {
    res += n.id;
}

string ann_printer::print( skel_node& sk ){
    return (*this)( sk );
}

string ann_printer::operator()(skel_node& sk){
    res.clear();
    sk.accept(*this);
    return res;
}

///////////////////////////////////////////////////////////////////////////////

void label_printer::visit( seq_node& n ) {
    str = "seq";
}

void label_printer::visit( source_node& n ) {
    str = "source";
}

void label_printer::visit( drain_node& n ) {
    str = "drain";
}

void label_printer::visit( comp_node& n ) {
    str = "comp";
}

void label_printer::visit( pipe_node& n ) {
    str = "pipe";
}

void label_printer::visit( farm_node& n ) {
    str = "farm";
}

void label_printer::visit( map_node& n ) {
    str = "map";
}

void label_printer::visit( reduce_node& n ) {
    str = "reduce";
}

void label_printer::visit( id_node& n ) {
    str = n.id;
}

string label_printer::operator()( skel_node& sk ) {
    sk.accept(*this);
    return str;
}

///////////////////////////////////////////////////////////////////////////////

void single_node_cloner::visit( seq_node& n ) {
    tmp = new seq_node(n);
}

void single_node_cloner::visit( source_node& n ) {
    tmp = new source_node(n);
}

void single_node_cloner::visit( drain_node& n ) {
    tmp = new drain_node(n);
}

void single_node_cloner::visit( comp_node& n ) {
    tmp = new comp_node{};
}

void single_node_cloner::visit( pipe_node& n ) {
    tmp = new pipe_node{};
}

void single_node_cloner::visit( farm_node& n ) {
    tmp = new farm_node{};
}

void single_node_cloner::visit( map_node& n ) {
    tmp = new map_node{};
}

void single_node_cloner::visit( reduce_node& n ) {
    tmp = new reduce_node{};
}

void single_node_cloner::visit( id_node& n ) {
    tmp = new id_node(n.id);
}

skel_node* single_node_cloner::operator()( skel_node& sk ) {
    sk.accept(*this);
    return tmp;
}

///////////////////////////////////////////////////////////////////////////////

reduce_resources::reduce_resources( rpl_environment& env ) :
    subexp(false), ts(env), getres(env), env(env)
{}

void reduce_resources::visit( seq_node& n ) {
    res = false;
}

void reduce_resources::visit( source_node& n ) {
    res = false;
}

void reduce_resources::visit( drain_node& n ) {
    res = false;
}

typedef std::tuple<skel_node*, double, size_t> sds_t;

template < typename triples_container >
void fill_triples( triples_container& triples, skel_node& n, servicetime& ts, resources& res ) {
    for (size_t i = 0; i < n.size(); i++) {
        triples.push_back( make_tuple(
            n.get(i),
            ts ( *n.get(i) ),
            res( *n.get(i) )
        ) );
    }
}

void reduce_resources::visit( comp_node& n ) {

    std::vector<sds_t> triples;
    fill_triples( triples, n, ts, getres );

    sort (triples.begin(), triples.end(), [](const sds_t& a, const sds_t& b) {
        if (get<2>(a) == get<2>(b))
            return get<1>(b) < get<1>(a);
        return get<2>(a) > get<2>(b);
    });

    for ( auto it = triples.begin(); it != triples.end() && !res; it++ )
        (*this)( *get<0>(*it) );
}

void reduce_resources::visit( pipe_node& n ) {

    std::vector<sds_t> triples;
    fill_triples( triples, n, ts, getres );

    sort (triples.begin(), triples.end(), [](const sds_t& a, const sds_t& b) {
        if (get<1>(a) == get<1>(b))
            return get<2>(b) < get<2>(a);
        return get<1>(a) < get<1>(b);
    });

    for ( auto it = triples.begin(); it != triples.end() && !res; it++ )
        (*this)( *get<0>(*it) );
}

void reduce_resources::visit( farm_node& n ) {
    int dim = env.get_dim();
    res = n.pardegree > 1;
    if ( n.pardegree > dim )
        n.pardegree = dim;
    else if ( res )
        n.pardegree--;
    else
        (*this)( *n.get(0) );
}

void reduce_resources::visit( map_node& n ) {
    assign_resources assignres;
    res = n.pardegree > 1;
    if ( res ) {
        n.pardegree--;
        assignres(n, n.inputsize);
    } else
        (*this)( *n.get(0) );
}

void reduce_resources::visit( reduce_node& n ) {
    assign_resources assignres;
    res = n.pardegree > 1;
    if ( res ) {
        n.pardegree--;
        assignres(n, n.inputsize);
    } else
        (*this)( *n.get(0) );
}

void reduce_resources::visit( id_node& n ) {
    if (subexp) {
        auto skptr = env.get(n.id, n.index);
        (*this)( *skptr );
    } else
        res = false;
}

bool reduce_resources::operator()(skel_node& n) {
    res = false;
    n.accept(*this);
    return res;
}

///////////////////////////////////////////////////////////////////////////////

void assign_resources::visit( seq_node& n ) {
}

void assign_resources::visit( comp_node& n ) {
    for (size_t i = 0; i < n.size(); i++)
        (*this)(*n.get(i), n.inputsize);
}

void assign_resources::visit( pipe_node& n ) {
    for (size_t i = 0; i < n.size(); i++)
        (*this)(*n.get(i), n.inputsize);
}

void assign_resources::visit( farm_node& n ) {
    (*this)(*n.get(0), n.inputsize);
}

void assign_resources::visit( map_node& n ) {
    (*this)(*n.get(0), n.inputsize/n.pardegree);
}

void assign_resources::visit( reduce_node& n ) {
    (*this)(*n.get(0), n.inputsize/n.pardegree);
}

void assign_resources::visit( id_node& n ) {
}

void assign_resources::operator()(skel_node& n, double inputsize) {
    n.inputsize = inputsize;
    n.accept(*this);
}

///////////////////////////////////////////////////////////////////////////////

get_seq_wrappers::get_seq_wrappers( rpl_environment& env ) :
        env(env)
{}

void get_seq_wrappers::visit( seq_node& n ) {
    seq_nodes.push_back(&n);
}

void get_seq_wrappers::visit( source_node& n ) {
    src_nodes.push_back(&n);
}

void get_seq_wrappers::visit( drain_node& n ) {
    drn_nodes.push_back(&n);
}

void get_seq_wrappers::visit( comp_node& n ) {
    for (size_t i = 0; i < n.size(); i++)
        n.get(i)->accept(*this);
}

void get_seq_wrappers::visit( pipe_node& n ) {
    for (size_t i = 0; i < n.size(); i++)
        n.get(i)->accept(*this);
}

void get_seq_wrappers::visit( farm_node& n ) {
    n.get(0)->accept(*this);
}

void get_seq_wrappers::visit( map_node& n ) {
    n.get(0)->accept(*this);
}

void get_seq_wrappers::visit( reduce_node& n ) {
    n.get(0)->accept(*this);
}

void get_seq_wrappers::visit( id_node& n ) {
    auto ptr = env.get(n.id, n.index);
    if (ptr != nullptr)
        ptr->accept(*this);
    else
        cout << n.id << " whaaaat? in get_seq_wrappers::visit(id_node)" << endl;
}

vector<seq_node*> get_seq_wrappers::get_seq_nodes() {
    return seq_nodes;
}

vector<source_node*> get_seq_wrappers::get_source_nodes() {
    return src_nodes;
}

vector<drain_node*> get_seq_wrappers::get_drain_nodes() {
    return drn_nodes;
}

void get_seq_wrappers::operator()(skel_node& n) {
    seq_nodes.clear();
    src_nodes.clear();
    drn_nodes.clear();
    n.accept(*this);
}

///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////

top_datap_skeletons::top_datap_skeletons( rpl_environment& env ) :
        env(env)
{}

void top_datap_skeletons::visit( seq_node& n ) {
}

void top_datap_skeletons::visit( source_node& n ) {
}

void top_datap_skeletons::visit( drain_node& n ) {
}

void top_datap_skeletons::visit( comp_node& n ) {
    for (size_t i = 0; i < n.size(); i++)
        n.get(i)->accept(*this);
}

void top_datap_skeletons::visit( pipe_node& n ) {
    for (size_t i = 0; i < n.size(); i++)
        n.get(i)->accept(*this);
}

void top_datap_skeletons::visit( farm_node& n ) {
    n.get(0)->accept(*this);
}

void top_datap_skeletons::visit( map_node& n ) {
    map_nodes.push_back(&n);
}

void top_datap_skeletons::visit( reduce_node& n ) {
    red_nodes.push_back(&n);
}

void top_datap_skeletons::visit( id_node& n ) {
    auto ptr = env.get(n.id, n.index);
    if (ptr != nullptr)
        ptr->accept(*this);
    else
        cout << n.id << " whaaaat? in top_data_skeletons::visit(id_node)" << endl;
}

vector<map_node*> top_datap_skeletons::get_map_nodes() {
    return map_nodes;
}

vector<reduce_node*> top_datap_skeletons::get_reduce_nodes() {
    return red_nodes;
}

void top_datap_skeletons::operator()(skel_node& n) {
    map_nodes.clear();
    red_nodes.clear();
    n.accept(*this);
}

///////////////////////////////////////////////////////////////////////////////
ranker::ranker( rpl_environment& env ) :
    env(env)
{}

void ranker::visit( seq_node& n ) {
}

void ranker::visit( comp_node& n ) {
    for (size_t i=0; i < n.size(); i++)
        n.get(i)->accept(*this);
}

void ranker::visit( pipe_node& n ) {
    for (size_t i=0; i < n.size(); i++)
        n.get(i)->accept(*this);
}

void ranker::visit( farm_node& n ) {
    n.get(0)->accept(*this);
}

void ranker::visit( map_node& n ) {
    n.get(0)->accept(*this);
}

void ranker::visit( reduce_node& n ) {
    n.get(0)->accept(*this);
}

void ranker::visit( id_node& n ) {
    auto ptr = env.get(n.id, n.index);
    if (ptr != nullptr)
        (*this) (*ptr);
}

void ranker::operator()( skel_node& n ) {
    unranktorank2(n, snc);
    n.accept(*this);
}

///////////////////////////////////////////////////////////////////////////////

unranker::unranker( rpl_environment& env ) :
    env(env)
{}

void unranker::visit( seq_node& n ) {
}

void unranker::visit( comp_node& n ) {
    skel_node* ptr = &n;
    ranktounrank(ptr,ptr);
    for (size_t i=0; i < n.size(); i++)
        n.get(i)->accept(*this);
}

void unranker::visit( pipe_node& n ) {
    skel_node* ptr = &n;
    ranktounrank(ptr,ptr);
    for (size_t i=0; i < n.size(); i++)
        n.get(i)->accept(*this);
}

void unranker::visit( farm_node& n ) {
    n.get(0)->accept(*this);
}

void unranker::visit( map_node& n ) {
    n.get(0)->accept(*this);
}

void unranker::visit( reduce_node& n ) {
    n.get(0)->accept(*this);
}

void unranker::visit( id_node& n ) {
    auto ptr = env.get(n.id, n.index);
    if (ptr != nullptr)
        ptr->accept(*this);
}

void unranker::operator()( skel_node& n ) {
    n.accept(*this);
}


///////////////////////////////////////////////////////////////////////////////

count_stages::count_stages( rpl_environment& env ) :
    env(env), res(0)
{}

void count_stages::visit( seq_node& n ) {
    res += 1;
}

void count_stages::visit( source_node& n ) {
    res += 1;
}

void count_stages::visit( drain_node& n ) {
    res += 1;
}

void count_stages::visit( comp_node& n ) {
    res += 1;
}

void count_stages::visit( pipe_node& n ) {
    for (size_t i = 0; i < n.size(); i++)
        n.get(i)->accept(*this);
}

void count_stages::visit( farm_node& n ) {
    res += 1;
}

void count_stages::visit( map_node& n ) {
    res += 1;
}

void count_stages::visit( reduce_node& n ) {
    res += 1;
}

void count_stages::visit( id_node& n ) {
    auto ptr = env.get(n.id, n.index);
    if (ptr != nullptr)
        ptr->accept(*this);
}

size_t count_stages::operator()( skel_node& n ) {
    res = 0;
    n.accept(*this);
    return res;
}
