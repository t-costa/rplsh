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

void ann_visitor::visit(dc_node &n) {
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

void ann_pardegree::visit(reduce_node &n) {
    n.pardegree = nw;
    result      = true;
}

void ann_pardegree::visit(dc_node &n) {
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

///////////////////////////////////////////////////////////////////////////////

ann_grain::ann_grain(rpl_environment &env) :
    ann_visitor(env), value(0)
{}

void ann_grain::visit(map_node &n) {
    n.grain = scheduling_type == "static" ? -value : value;
    result = true;
}

void ann_grain::visit(reduce_node &n) {
    n.grain = scheduling_type == "static" ? -value : value;
    result = true;
}

bool ann_grain::operator()(skel_node &n, ann_node &a) {
    value = (long) a.value;
    scheduling_type = a.word;
    n.accept(*this);
    return result;
}

///////////////////////////////////////////////////////////////////////////////

ann_step::ann_step(rpl_environment &env) : ann_visitor(env), value(1) {

}

void ann_step::visit(map_node &n) {
    result = (value >= 1);
    n.step = result ? value : 1;
}

void ann_step::visit(reduce_node &n) {
    result = (value >= 1);
    n.step = result ? value : 1;
}

bool ann_step::operator()(skel_node &n, ann_node &a) {
    value = (long) a.value;
    n.accept(*this);
    return result;
}


///////////////////////////////////////////////////////////////////////////////

ann_dc_capable::ann_dc_capable(rpl_environment &env) :
    ann_visitor(env), dc_flag(false)
{}

void ann_dc_capable::visit(seq_node &n) {
    n.dc_flag = dc_flag;
    result = true;
}

bool ann_dc_capable::operator()(skel_node &n, ann_node &a) {
    dc_flag = (a.value != 0);
    n.accept(*this);
    return result;
}

///////////////////////////////////////////////////////////////////////////////

ann_cutoff::ann_cutoff(rpl_environment &env) :
    ann_visitor(env), value(1)
{ }

void ann_cutoff::visit(dc_node &n) {
    result = (value >= 1);
    n.cutoff = result ? value : 1;
//    if (n.transformed) {
//        result = (value >= 1);
//        n.cutoff = result ? value : 1;
//    } else {
//        result = false;
//    }

}

bool ann_cutoff::operator()(skel_node &n, ann_node &a) {
    value = (long) a.value;
    n.accept(*this);
    return result;
}

///////////////////////////////////////////////////////////////////////////////

ann_schedule::ann_schedule(rpl_environment &env) :
    ann_visitor(env), value(2), scheduling_type("tie")
{ }

void ann_schedule::visit(dc_node &n) {
    if (n.transformed) {
        result = (value >= 2);
        n.schedule = result ? value : 2;
        //if explicit zip, negative schedule, otherwise standard tie
        if (scheduling_type == "zip") {
            n.schedule = -n.schedule;
        }
    } else {
        result = false;
    }

}

bool ann_schedule::operator()(skel_node &n, ann_node &a) {
    value = (long) a.value;
    scheduling_type = a.word;
    n.accept(*this);
    return result;
}

