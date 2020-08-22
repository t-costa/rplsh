#include "annotators.hpp"
#include "nodes/verbs.hpp"
#include <iostream>

using namespace std;

///////////////////////////////////////////////////////////////////////////////
/**
 * Basically is the base implementation for the other annotators:
 * result is always set to false
 * @param env rpl environment
 */
ann_visitor::ann_visitor( rpl_environment& env ) :
    env ( env ), result (false)
{}

void ann_visitor::visit( seq_node& n ) {
    result = false;
}

void ann_visitor::visit( source_node& n ) {
    result = false;
}

void ann_visitor::visit( drain_node& n ) {
    result = false;
}

void ann_visitor::visit( comp_node& n ) {
    result = false;
}

void ann_visitor::visit( pipe_node& n ) {
    result = false;
}

void ann_visitor::visit( farm_node& n ) {
    result = false;
}

void ann_visitor::visit( map_node& n ) {
    result = false;
}

void ann_visitor::visit( reduce_node& n ) {
    result = false;
}

void ann_visitor::visit( id_node& n ) {
    result = false;
}

///////////////////////////////////////////////////////////////////////////////

ann_servicetime::ann_servicetime( rpl_environment& env ) :
    ann_visitor ( env ), servicetime ( 1 )
{}

void ann_servicetime::visit( seq_node& n ) {
    n.servicetime = servicetime;
    result        = true;
}

void ann_servicetime::visit( source_node& n ) {
    n.servicetime = servicetime;
    result        = true;
}

void ann_servicetime::visit( drain_node& n ) {
    n.servicetime = servicetime;
    result        = true;
}

/**
 * Sets the value for service time and starts the visit on n
 * @param n skeleton node
 * @param a annotation
 * @return true iff at least a node is annotated (seq/source/drain)
 */
bool ann_servicetime::operator()( skel_node& n, ann_node& a ) {
    if (a.value < 0) {
        cout << "warning: bad annotation value" << endl;
        return false;
    }
    servicetime = a.value;
    n.accept( *this );
    return result;
}

///////////////////////////////////////////////////////////////////////////////

ann_pardegree::ann_pardegree( rpl_environment& env ) :
    ann_visitor ( env ), nw ( 1 )
{}

void ann_pardegree::visit( farm_node& n ) {
    n.pardegree = nw;
    result      = true;
}

void ann_pardegree::visit( map_node& n ) {
    n.pardegree = nw;
    result      = true;
}

/**
 * Sets the value of workers and starts the visit on n
 * @param n skeleton node
 * @param a annotation
 * @return true iff at least a node is set (farm/map)
 */
bool ann_pardegree::operator()( skel_node& n, ann_node& a ) {
    if (a.value <= 0) {
        cout << "warning: bad annotation value" << endl;
        return false;
    }
    this->nw = a.value;
    n.accept( *this );
    return result;
}

///////////////////////////////////////////////////////////////////////////////

ann_datap::ann_datap( rpl_environment& env ) :
    ann_visitor ( env ), datap ( false )
{}

void ann_datap::visit( seq_node& n ) {
    n.datap_flag = datap;
    result       = true;
}

/**
 * Sets the value for the flag of data parallelization and
 * starts the visit on n
 * @param n skeleton node
 * @param a annotation
 * @return true iff at least a sequential node is set
 */
bool ann_datap::operator()( skel_node& n, ann_node& a ) {
    this->datap = ( a.value != 0 );
    n.accept( *this );
    return result;
}

///////////////////////////////////////////////////////////////////////////////

ann_inside_data_parallel::ann_inside_data_parallel(rpl_environment &env) :
    ann_visitor( env ) , inside_data_parallel (false)
{}

void ann_inside_data_parallel::visit(seq_node &n) {
    n.inside_map_red = inside_data_parallel;
    result = true;
}

void ann_inside_data_parallel::visit(comp_node& n) {
    for (size_t i=0; i<n.size(); ++i) {
        n.get(i)->accept(*this);  //TODO: CHECK!
    }
}

void ann_inside_data_parallel::visit(id_node& n) {
    auto ptr = env.get(n.id, n.index);
    if (ptr != nullptr)
        ptr->accept(*this);
}

bool ann_inside_data_parallel::operator()(skel_node &n, ann_node &a) {
    this->inside_data_parallel = (a.value != 0);
    n.accept(*this);
    return result;
}

///////////////////////////////////////////////////////////////////////////////

ann_typein::ann_typein( rpl_environment& env ) :
    ann_visitor ( env )
{}

void ann_typein::visit( seq_node& n ) {
    n.typein = type;
    result   = true;
}

void ann_typein::visit( drain_node& n ) {
    n.typein = type;
    result   = true;
}

/**
 * Sets the type of the input and starts the visit on n
 * @param n skeleton node
 * @param a annotation
 * @return true iff at least a node is annotated (seq/drain), false otherwise
 */
bool ann_typein::operator()( skel_node& n, ann_node& a ) {
    type = a.word;
    n.accept(*this);
    return result;
}

///////////////////////////////////////////////////////////////////////////////

ann_typeout::ann_typeout( rpl_environment& env ) :
    ann_visitor ( env )
{}

void ann_typeout::visit( seq_node& n ) {
    n.typeout = type;
    result    = true;
}

void ann_typeout::visit( source_node& n ) {
    n.typeout = type;
    result    = true;
}

/**
 * Sets the type of the output and starts the visit on n
 * @param n skeleton node
 * @param a annotation
 * @return true iff at least a node is annotated (seq/source), false otherwise
 */
bool ann_typeout::operator()( skel_node& n, ann_node& a ) {
    type = a.word;
    n.accept(*this);
    return result;
}
