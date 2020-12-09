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

/**
 * Constructs a string representation of n
 * @param name skeleton name
 * @param n skeleton node
 */
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

void printer::visit(dc_node &n) {
    tostring("divide_and_conquer", n);
}

void printer::visit(id_node& n) {
    res += n.id;
}

/**
 * Calls operator(), does not actually print
 * @param sk skeleton node
 * @return string representation of sk
 */
string printer::print( skel_node& sk ){
    return (*this)( sk );
}

/**
 * @param sk skeleton node
 * @return string representation of sk
 */
string printer::operator()(skel_node& sk){
    res.clear();
    sk.accept(*this);
    return res;
}

///////////////////////////////////////////////////////////////////////////////

/**
 * Prints the string representation of n and adds at the end the annotations
 * given in ann (service time or par degree)
 * @param name type of the skeleton
 * @param ann annotations for n
 * @param n skeleton node
 */
void ann_printer::tostring(const string& name, const string& ann, const skel_node& n) {
    res += name + "(";
    for (size_t i = 0; i < n.size(); i++) {
        n.get(i)->accept(*this);
        res += (i != n.size()-1 ? "," : "");
    }
    res = res + ") " + ann;
}

/**
 * Prints the string representation of n without annotations
 * (for the nodes that have no annotations)
 * @param name type of the skeleton
 * @param n skeleton node
 */
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

void ann_printer::visit(dc_node &n) {
    tostring("divide_and_conquer", "with [nw " + to_string(n.pardegree) + "]", n);
}

void ann_printer::visit(id_node& n) {
    res += n.id;
}

/**
 * Calls operator(), does not actually print
 * @param sk skeleton node
 * @return string representation of sk with annotations
 */
string ann_printer::print( skel_node& sk ){
    return (*this)( sk );
}

/**
 * @param sk skeleton node
 * @return string representation of sk with annotations
 */
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

void label_printer::visit(dc_node &n) {
    str = "divide_and_conquer";
}

void label_printer::visit( id_node& n ) {
    str = n.id;
}

/**
 * @param sk skeleton node
 * @return the name of the type of sk
 */
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

void single_node_cloner::visit(dc_node &n) {
    tmp = new dc_node{};
}

void single_node_cloner::visit( id_node& n ) {
    tmp = new id_node(n.id);
}

/**
 * Creates a new skeleton node f the same type of sk
 * @param sk node to be cloned
 * @return a pointer to a new node of the same type of sk
 */
skel_node* single_node_cloner::operator()( skel_node& sk ) {
    sk.accept(*this);
    return tmp;
}

///////////////////////////////////////////////////////////////////////////////

/**
 * It reduces the resources of the first possible node
 * @param env environment
 */
reduce_resources::reduce_resources( rpl_environment& env ) :
    subexp(false), res(false), ts(env), getres(env), env(env)
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

/**
 * Adds to triples a tuple (node, servicetime, resources)
 * for each child of n
 * @tparam triples_container template for the list of triples
 * @param triples list of triples
 * @param n starting skeleton node
 * @param ts reference for the service time visitor
 * @param res reference for the resource visitor
 */
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

/**
 * It sorts the nodes in the comp by resources and service time,
 * then calls the visit for each one of its children
 * @param n comp node
 */
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

/**
 * It sorts the nodes in the pipe by service time and resources,
 * then calls the visit for each one of its children
 * @param n pipe node
 */
void reduce_resources::visit( pipe_node& n ) {
    std::vector<sds_t> triples;
    fill_triples( triples, n, ts, getres );

    sort (triples.begin(), triples.end(), [](const sds_t& a, const sds_t& b) {
        if (get<1>(a) == get<1>(b))
            return get<2>(b) < get<2>(a);
        return get<1>(a) < get<1>(b);
    });

    //TODO: why should stop if res == true?
    for ( auto it = triples.begin(); it != triples.end() && !res; it++ )
        (*this)( *get<0>(*it) );
}

/**
 * If possible, it reduces the resources to n,
 * otherwise continues the visit to the child
 * @param n farm node
 */
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

/**
 * If possible, it reduces resources to n
 * and it assigns resources to n, otherwise
 * it continues the visit to the child
 * @param n map node
 */
void reduce_resources::visit( map_node& n ) {
    assign_resources assignres;
    res = n.pardegree > 1;
    if ( res ) {
        n.pardegree--;
        assignres(n, n.inputsize);
    } else
        (*this)( *n.get(0) );
}

/**
 * If possible, it reduces resources to n
 * and it assigns resources to n, otherwise
 * it continues the visit to the child
 * @param n reduce node
 */
void reduce_resources::visit( reduce_node& n ) {
    assign_resources assignres;
    res = n.pardegree > 1;
    if ( res ) {
        n.pardegree--;
        assignres(n, n.inputsize);
    } else
        (*this)( *n.get(0) );
}

void reduce_resources::visit(dc_node &n) {
    //TODO: controlla correttezza!
    assign_resources assignres;
    res = n.pardegree > 1;
    if (res) {
        n.pardegree--;
        assignres(n, n.inputsize);
    } else {
        (*this)(*n.get(0));
    }
}

/**
 * If the sub expressions should be checked, it
 * continues the visit searching n in the env,
 * otherwise it stops with res=false
 * @param n id node
 */
void reduce_resources::visit( id_node& n ) {
    if (subexp) {
        auto skptr = env.get(n.id, n.index);
        if (skptr != nullptr)
            (*this)( *skptr );
        else
            res = false;    //TODO: just this or raise error? TC
    } else
        res = false;
}

/**
 * If possible, it reduces the resources of a node
 * @param n starting skeleton tree
 * @return true iff resources have been reduced
 */
bool reduce_resources::operator()(skel_node& n) {
    res = false;
    n.accept(*this);
    return res;
}

///////////////////////////////////////////////////////////////////////////////

/**
 * Does nothing
 * @param n seq node
 */
void assign_resources::visit( seq_node& n ) {
}

/**
 * Calls the visit for each child
 * @param n comp node
 */
void assign_resources::visit( comp_node& n ) {
    for (size_t i = 0; i < n.size(); i++)
        (*this)(*n.get(i), n.inputsize);
}

/**
 * Calls the visit for each child
 * @param n pipe node
 */
void assign_resources::visit( pipe_node& n ) {
    for (size_t i = 0; i < n.size(); i++)
        (*this)(*n.get(i), n.inputsize);
}

/**
 * Calls the visit for the child
 * @param n farm node
 */
void assign_resources::visit( farm_node& n ) {
    (*this)(*n.get(0), n.inputsize);
}

/**
 * Calls the visit for the child
 * @param n map node
 */
void assign_resources::visit( map_node& n ) {
    (*this)(*n.get(0), n.inputsize/n.pardegree);
}

/**
 * Calls the visit for the child
 * @param n reduce node
 */
void assign_resources::visit( reduce_node& n ) {
    (*this)(*n.get(0), n.inputsize/n.pardegree);
}

void assign_resources::visit(dc_node &n) {
    //TODO: controlla correttezza
    (*this)(*n.get(0), n.inputsize/n.pardegree);
}

/**
 * Does nothing
 * @param n id node
 */
void assign_resources::visit( id_node& n ) {
}

/**
 * Sets the desired input size and calls the visit for n
 * @param n skeleton node
 * @param inputsize real input size for the node
 */
void assign_resources::operator()(skel_node& n, double inputsize) {
    n.inputsize = inputsize;
    n.accept(*this);
}

///////////////////////////////////////////////////////////////////////////////

/**
 * Builds the lists for all the different sequential/drain/source nodes
 * @param env environment
 */
get_seq_wrappers::get_seq_wrappers( rpl_environment& env ) :
        env(env), inside_datap(false)
{}

/**
 * Adds n to the list of sequential nodes
 * @param n seq node
 */
void get_seq_wrappers::visit( seq_node& n ) {
    if (!inside_datap)
        seq_nodes.push_back(&n);
    else
        datap_nodes.push_back(&n);
}

/**
 * Adds n to the list of source nodes
 * @param n source node
 */
void get_seq_wrappers::visit( source_node& n ) {
    src_nodes.push_back(&n);
}

/**
 * Adds n to the list of drain nodes
 * @param n drain node
 */
void get_seq_wrappers::visit( drain_node& n ) {
    drn_nodes.push_back(&n);
}

/**
 * Calls the visit for all the children
 * @param n comp node
 */
void get_seq_wrappers::visit( comp_node& n ) {
    for (size_t i = 0; i < n.size(); i++)
        n.get(i)->accept(*this);
}

/**
 * Calls the visit for all the children
 * @param n pipe node
 */
void get_seq_wrappers::visit( pipe_node& n ) {
    for (size_t i = 0; i < n.size(); i++)
        n.get(i)->accept(*this);
}

/**
 * Calls the visit for the child
 * @param n farm node
 */
void get_seq_wrappers::visit( farm_node& n ) {
    n.get(0)->accept(*this);
}

/**
 * Calls the visit for the child
 * @param n map node
 */
void get_seq_wrappers::visit( map_node& n ) {
    inside_datap = true;
    n.get(0)->accept(*this);
    inside_datap = false;
}

/**
 * Calls the visit for the child
 * @param n reduce node
 */
void get_seq_wrappers::visit( reduce_node& n ) {
    inside_datap = true;
    n.get(0)->accept(*this);
    inside_datap = false;
}

void get_seq_wrappers::visit(dc_node &n) {
    //not considered datap
    n.get(0)->accept(*this);
}

/**
 * If n exists in the environment, calls the visit for n
 * @param n id node
 */
void get_seq_wrappers::visit( id_node& n ) {
    auto ptr = env.get(n.id, n.index);
    if (ptr != nullptr)
        ptr->accept(*this);
    else
        cerr << n.id << "Unexpected error in get_seq_wrappers::visit(id_node)" << endl;
}

/**
 * @return list of sequential nodes
 */
vector<seq_node*> get_seq_wrappers::get_seq_nodes() {
    return seq_nodes;
}

/**
 * @return list of nodes inside data parallel patterns
 */
vector<seq_node*> get_seq_wrappers::get_datap_nodes() {
    return datap_nodes;
}

/**
 * @return list of source nodes
 */
vector<source_node*> get_seq_wrappers::get_source_nodes() {
    return src_nodes;
}

/**
 * @return list of drain nodes
 */
vector<drain_node*> get_seq_wrappers::get_drain_nodes() {
    return drn_nodes;
}

/**
 * Clears all the lists and start the visit
 * @param n root node
 */
void get_seq_wrappers::operator()(skel_node& n) {
    seq_nodes.clear();
    src_nodes.clear();
    drn_nodes.clear();
    n.accept(*this);
}

///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////

/**
 * Builds the lists for the topmost data parallel patterns
 * @param env environment
 */
top_datap_skeletons::top_datap_skeletons( rpl_environment& env ) :
        env(env)
{}

/**
 * Does nothing
 * @param n seq node
 */
void top_datap_skeletons::visit( seq_node& n ) {
}

/**
 * Does nothing
 * @param n source node
 */
void top_datap_skeletons::visit( source_node& n ) {
}

/**
 * Does nothing
 * @param n drain node
 */
void top_datap_skeletons::visit( drain_node& n ) {
}

/**
 * Calls the visit for the children
 * @param n comp node
 */
void top_datap_skeletons::visit( comp_node& n ) {
    for (size_t i = 0; i < n.size(); i++)
        n.get(i)->accept(*this);
}

/**
 * Calls the visit for the children
 * @param n pipe node
 */
void top_datap_skeletons::visit( pipe_node& n ) {
    for (size_t i = 0; i < n.size(); i++)
        n.get(i)->accept(*this);
}

/**
 * Calls the visit for the child
 * @param n farm node
 */
void top_datap_skeletons::visit( farm_node& n ) {
    n.get(0)->accept(*this);
}

/**
 * Adds n to the list of map nodes
 * @param n map node
 */
void top_datap_skeletons::visit( map_node& n ) {
    //I'm taking only the topmost nodes
    map_nodes.push_back(&n);
}

/**
 * Adds n to the list of reduce nodes
 * @param n reduce node
 */
void top_datap_skeletons::visit( reduce_node& n ) {
    //I'm taking only the topmost nodes
    red_nodes.push_back(&n);
}

void top_datap_skeletons::visit(dc_node &n) {
    n.get(0)->accept(*this);
}

/**
 * If n exists in the environment, calls the visit for n
 * @param n id node
 */
void top_datap_skeletons::visit( id_node& n ) {
    auto ptr = env.get(n.id, n.index);
    if (ptr != nullptr)
        ptr->accept(*this);
    else
        cerr << n.id << "Unexpected error in top_data_skeletons::visit(id_node)" << endl;
}

/**
 * @return list of map nodes
 */
vector<map_node*> top_datap_skeletons::get_map_nodes() {
    return map_nodes;
}

/**
 * @return list of reduce nodes
 */
vector<reduce_node*> top_datap_skeletons::get_reduce_nodes() {
    return red_nodes;
}

/**
 * Recovers map and reduce nodes
 * @param n root node
 */
void top_datap_skeletons::operator()(skel_node& n) {
    map_nodes.clear();
    red_nodes.clear();
    n.accept(*this);
}

///////////////////////////////////////////////////////////////////////////////
ranker::ranker( rpl_environment& env ) :
    env(env)
{}

/**
 * Does nothing
 * @param n seq node
 */
void ranker::visit( seq_node& n ) {
}

/**
 * Calls the visit for the children
 * @param n comp node
 */
void ranker::visit( comp_node& n ) {
    for (size_t i=0; i < n.size(); i++)
        n.get(i)->accept(*this);
}

/**
 * Calls the visit for the children
 * @param n pipe node
 */
void ranker::visit( pipe_node& n ) {
    for (size_t i=0; i < n.size(); i++)
        n.get(i)->accept(*this);
}

/**
 * Calls the visit for the child
 * @param n farm node
 */
void ranker::visit( farm_node& n ) {
    n.get(0)->accept(*this);
}

/**
 * Calls the visit for the child
 * @param n map node
 */
void ranker::visit( map_node& n ) {
    n.get(0)->accept(*this);
}

/**
 * Calls the visit for the child
 * @param n reduce node
 */
void ranker::visit( reduce_node& n ) {
    n.get(0)->accept(*this);
}

void ranker::visit(dc_node &n) {
    n.get(0)->accept(*this);
}

/**
 * If the node exists in the environment,
 * unranks the node and continues the visit
 * @param n id node
 */
void ranker::visit( id_node& n ) {
    auto ptr = env.get(n.id, n.index);
    if (ptr != nullptr)
        (*this) (*ptr);
}

/**
 * Calls unrank on n and starts the visit
 * @param n skeleton node
 */
void ranker::operator()( skel_node& n ) {
    unranktorank2(n, snc);
    n.accept(*this);
}

///////////////////////////////////////////////////////////////////////////////

unranker::unranker( rpl_environment& env ) :
    env(env)
{}

/**
 * Does nothing
 * @param n seq node
 */
void unranker::visit( seq_node& n ) {
}

/**
 * Calls rank for n and continues the visit for the children
 * @param n comp node
 */
void unranker::visit( comp_node& n ) {
    skel_node* ptr = &n;
    ranktounrank(ptr,ptr);
    for (size_t i=0; i < n.size(); i++)
        n.get(i)->accept(*this);
}

/**
 * Calls rank for n and continues the visit for the children
 * @param n pipe node
 */
void unranker::visit( pipe_node& n ) {
    skel_node* ptr = &n;
    ranktounrank(ptr,ptr);
    for (size_t i=0; i < n.size(); i++)
        n.get(i)->accept(*this);
}

/**
 * Calls the visit for the child
 * @param n farm node
 */
void unranker::visit( farm_node& n ) {
    n.get(0)->accept(*this);
}

/**
 * Calls the visit for the child
 * @param n map node
 */
void unranker::visit( map_node& n ) {
    n.get(0)->accept(*this);
}

/**
 * Calls the visit for the child
 * @param n reduce node
 */
void unranker::visit( reduce_node& n ) {
    n.get(0)->accept(*this);
}

void unranker::visit(dc_node &n) {
    //TODO: controlla correttezza
    n.get(0)->accept(*this);
}

/**
 * If the node exists in the environment,
 * continues the visit
 * @param n id node
 */
void unranker::visit( id_node& n ) {
    auto ptr = env.get(n.id, n.index);
    if (ptr != nullptr)
        ptr->accept(*this);
}

/**
 * Calls the visit on n
 * @param n skeleton node
 */
void unranker::operator()( skel_node& n ) {
    n.accept(*this);
}


///////////////////////////////////////////////////////////////////////////////

/**
 * Counts the number of stages in the skeleton tree
 * @param env environment
 */
count_stages::count_stages( rpl_environment& env ) :
    env(env), res(0)
{}

/**
 * Increase counter
 * @param n seq node
 */
void count_stages::visit( seq_node& n ) {
    res += 1;
}

/**
 * Increase counter
 * @param n source node
 */
void count_stages::visit( source_node& n ) {
    res += 1;
}

/**
 * Increase counter
 * @param n drain node
 */
void count_stages::visit( drain_node& n ) {
    res += 1;
}

/**
 * Increase counter
 * @param n comp node
 */
void count_stages::visit( comp_node& n ) {
    res += 1;
}

/**
 * Calls the visit for the children
 * @param n pipe node
 */
void count_stages::visit( pipe_node& n ) {
    for (size_t i = 0; i < n.size(); i++)
        n.get(i)->accept(*this);
}

/**
 * Increase counter
 * @param n farm node
 */
void count_stages::visit( farm_node& n ) {
    res += 1;
}

/**
 * Increase counter
 * @param n map node
 */
void count_stages::visit( map_node& n ) {
    res += 1;
}

/**
 * Increase counter
 * @param n reduce node
 */
void count_stages::visit( reduce_node& n ) {
    res += 1;
}

void count_stages::visit(dc_node &n) {
    res += 1;
}

/**
 * If n exists in the environment, calls the visit for n
 * @param n id node
 */
void count_stages::visit( id_node& n ) {
    auto ptr = env.get(n.id, n.index);
    if (ptr != nullptr)
        ptr->accept(*this);
}

/**
 * Starts the visit
 * @param n root node
 * @return number of stages
 */
size_t count_stages::operator()( skel_node& n ) {
    res = 0;
    n.accept(*this);
    return res;
}

///////////////////////////////////////////////////////////////////////////////

/**
 * Checks if nodes inside data parallel pattern
 * are annotate accordingly
 * @param env
 */
check_datap::check_datap(rpl_environment &env) :
    env(env), inside_datap(false), res(true) { }

void check_datap::visit(source_node &n) {
    res = !inside_datap;
}

void check_datap::visit(drain_node &n) {
    res = !inside_datap;
}

void check_datap::visit(seq_node &n) {
    res = !inside_datap || n.datap_flag;
}

void check_datap::visit(pipe_node &n) {
    //TODO: questo in realtà controlla il two-tier,
    // che ok, può comunque essere utile, ma forse non qui
    if (!inside_datap) {
        for (size_t i = 0; i < n.size() && res; i++)
            n.get(i)->accept(*this);
    } else {
        res = false;
    }
}

void check_datap::visit(comp_node &n) {
    //map/reduce(comp(seq, seq)) it's ok
    for (size_t i = 0; i < n.size() && res; i++)
        n.get(i)->accept(*this);
}

void check_datap::visit(farm_node &n) {
    //TODO: questo in realtà controlla il two-tier,
    // che ok, può comunque essere utile, ma forse non qui
    if (!inside_datap) {
        n.get(0)->accept(*this);
    } else {
        res = false;
    }
}

void check_datap::visit(map_node &n) {
    inside_datap = true;
    n.get(0)->accept(*this);
    inside_datap = false;
}

void check_datap::visit(reduce_node &n) {
    inside_datap = true;
    n.get(0)->accept(*this);
    inside_datap = false;
}

void check_datap::visit(dc_node &n) {
    //TODO: è possibile in realtà?
    if (!inside_datap) {
        n.get(0)->accept(*this);
    } else {
        res = false;
    }
}

void check_datap::visit(id_node &n) {
    auto ptr = env.get(n.id, n.index);
    if (ptr != nullptr)
        ptr->accept(*this);
}

/**
 * true iff there are NO map/reduce nodes OR
 * there are map/reduce && the contain
 * ONLY seq nodes with datap==true
 * (NO source/drain/farm/pipe/dc)
 * @param n
 * @return
 */
bool check_datap::operator()(skel_node &n) {
    res = true;
    n.accept(*this);
    return res;
}

///////////////////////////////////////////////////////////////////////////////

/**
 * Checks if the node inside the divide and conquer
 * node is annotated accordingly
 * @param env
 */
check_dc::check_dc(rpl_environment &env) :
    env(env), inside_dc(false), res(true) {}

void check_dc::visit(seq_node &n) {
    res = !inside_dc || n.dc_flag;
}

void check_dc::visit(source_node &n) {
    res = !inside_dc;
}

void check_dc::visit(drain_node &n) {
    res = !inside_dc;
}

void check_dc::visit(comp_node &n) {
    if (!inside_dc) {
        for (size_t i = 0; i < n.size() && res; i++)
            n.get(i)->accept(*this);
    } else {
        res = false;
    }
}

void check_dc::visit(pipe_node &n) {
    if (!inside_dc) {
        for (size_t i = 0; i < n.size() && res; i++)
            n.get(i)->accept(*this);
    } else {
        res = false;
    }
}

void check_dc::visit(farm_node &n) {
    if (!inside_dc) {
        n.get(0)->accept(*this);
    } else {
        res = false;
    }
}

void check_dc::visit(map_node &n) {
    if (!inside_dc) {
        n.get(0)->accept(*this);
    } else {
        res = false;
    }
}

void check_dc::visit(reduce_node &n) {
    if (!inside_dc) {
        n.get(0)->accept(*this);
    } else {
        res = false;
    }
}

void check_dc::visit(dc_node &n) {
    inside_dc = true;
    n.get(0)->accept(*this);
    inside_dc = false;
}

void check_dc::visit(id_node &n) {
    auto ptr = env.get(n.id, n.index);
    if (ptr != nullptr)
        ptr->accept(*this);
}

/**
 * Returns true iff there are NO dc nodes
 * OR there are dc nodes && they contain
 * only a single sequential node annotated
 * accordingly
 * @param n
 * @return
 */
bool check_dc::operator()(skel_node &n) {
    res = true;
    n.accept(*this);
    return res;
}
