#include "optimizers.hpp"
#include "utils/rank.hpp"
#include <tuple>
#include <cassert>
#include <cmath>
#include <iostream>

// useful for farmfarmopt and mapmapopt
#include "rewriting/rewrules.hpp"

using namespace std;

/* implementation of these functions is at the end */
tuple<int, int, int> longestrun( const pipe_node& n, servicetime& ts );
void pipe_merge( pipe_node&n, size_t left, size_t right );
pair<size_t, double> findmax( const skel_node& n, servicetime& ts );
skel_node* modifytree( skel_node& n, rewrule& r );

///////////////////////////////////////////////////////////////////////////////

/**
 * Does nothing
 * @param n seq node
 */
void optrule::visit( seq_node& n ) {
}

/**
 * Calls the visit for the children
 * @param n comp node
 */
void optrule::visit( comp_node& n ) {
    for (size_t i = 0; i < n.size(); i++)
        (*this)( *n.get(i) );
}

/**
 * Calls the visit for the children
 * @param n pipe node
 */
void optrule::visit( pipe_node& n ) {
    for (size_t i = 0; i < n.size(); i++)
        (*this)( *n.get(i) );
}

/**
 * Calls the visit for the child
 * @param n farm node
 */
void optrule::visit( farm_node& n ) {
    (*this)( *n.get(0) );
}

/**
 * Calls the visit for the child
 * @param n map node
 */
void optrule::visit( map_node& n ) {
    (*this)( *n.get(0) );
}

/**
 * Calls the visit for the child
 * @param n reduce node
 */
void optrule::visit( reduce_node& n ) {
    (*this)( *n.get(0) );
}

/**
 * Calls the visit for the child
 * @param n dc node
 */
void optrule::visit(dc_node &n) {
    (*this)(*n.get(0));
}

/**
 * If the option is enabled, recovers
 * n from the environment and calls the visit
 * @param n id node
 */
void optrule::visit( id_node& n ) {
    if (!subexp)
        return;

    auto ptr = env.get(n.id, n.index);
    if (ptr == nullptr) {
        std::cerr << "Error: " << n.id << " is not in the environment" << std::endl;
        return;
    }
    (*this)( *ptr );
}

optrule::optrule( rpl_environment& env ) :
    subexp( false ),
    env( env ),
    ts( env )
{}

///////////////////////////////////////////////////////////////////////////////

farmopt::farmopt( rpl_environment& env ) :
    optrule(env)
{}

/**
 * Calls the (default optimization) visit on the child,
 * recovers service time, emitter time and sets the
 * optimal parallelism degree
 * @param n
 */
void farmopt::visit( farm_node& n ) {
    (*this)( *n.get(0) );
    double tw = ts( *n.get(0) );
    double te = env.get_emitter_time();
    size_t nw = ceil( tw / te );
    n.pardegree = nw;
}

/**
 * Calls the visit on sk
 * @param sk skeleton node
 */
void farmopt::operator()( skel_node& sk ) {
    sk.accept( *this );
}

///////////////////////////////////////////////////////////////////////////////

mapopt::mapopt( rpl_environment& env ) :
    optrule(env)
{}

/**
 * Computes the optimal parallelism degree and sets the correct
 * number of workers, then recurse on the child
 * @param n map node
 */
void mapopt::visit( map_node& n ) {
    /* reassign resources assuming only one worker */
    assign_resources assignres;
    n.pardegree = 1;
    assignres(n, n.inputsize);

    /* compute the "optimal" pardegree */
    double tsc = env.get_scatter_time();
    double tsg = env.get_gather_time();
    double tw  = ts( *n.get(0) );
    size_t nw  = ceil( sqrt( tw / max(tsc,tsg) ) ); //Vanneschi's book 14.1 Map
    n.pardegree = nw;

    /* reassign resources with the new pardegree */
    assignres(n, n.inputsize);

    /* recurse; TODO or not? */
    (*this)( *n.get(0) );
}

/**
 * Calls the visit
 * @param sk skeleton node
 */
void mapopt::operator()( skel_node& sk ) {
    sk.accept( *this );
}

///////////////////////////////////////////////////////////////////////////////

reduceopt::reduceopt( rpl_environment& env ) :
    optrule(env)
{}

/**
 * Computes the optimal parallelism and sets the correct number of workers,
 * then recurse on the child
 * @param n reduce node
 */
void reduceopt::visit( reduce_node& n ) {
    //std::cout << "reduceopt first: " << n.pardegree << " - " << n.inputsize << std::endl;
    /* reassign resources assuming only one worker */
    assign_resources assignres;
    n.pardegree = 1;
    assignres(n, n.inputsize);

    /* compute the "optimal" pardegree */
    double tsc = env.get_scatter_time();
    double tsg = env.get_gather_time();
    double tw  = ts( *n.get(0) );
    size_t nw  = ceil( sqrt( tw / max(tsc,tsg) ) ); //Vanneschi's book 14.1 Map
    n.pardegree = nw;

    /* reassign resources with the new pardegree */
    assignres(n, n.inputsize);

    //std::cout << "reduceopt second: " << n.pardegree << " - " << n.inputsize << std::endl;
    /* compute the optimal number of workers */
    //TODO: I don't get it... TC
//    n.pardegree = static_cast<int>( (n.inputsize) * log(2) );
//    assignres(n, n.inputsize);
    //std::cout << "npardegree : " << n.pardegree << std::endl;
    //std::cout << "reduceopt third: " << n.pardegree << " - " << n.inputsize << std::endl;
    /* recurse; TODO Or not?*/
    (*this)( *n.get(0) );
}

/**
 * Calls the visit on sk
 * @param sk skeleton node
 */
void reduceopt::operator()( skel_node& sk ) {
    sk.accept( *this );
}

///////////////////////////////////////////////////////////////////////////////

dcopt::dcopt(rpl_environment &env) : optrule(env) { }

/**
 * Sets optimum number of workers for a divide and conquer node
 * @param n dc node
 */
void dcopt::visit(dc_node &n) {
    /* reassign resources assuming only one worker */
    assign_resources assignres;
    n.pardegree = 1;
    assignres(n, n.inputsize);

    /* compute the "optimal" pardegree */
    double tsc = env.get_scatter_time();
    double tsg = env.get_gather_time();
    double tw  = ts( *n.get(0) );
    size_t nw  = ceil( sqrt( tw / max(tsc,tsg) ) ); //Vanneschi's book 14.1 Map
    n.pardegree = nw;

    /* reassign resources with the new pardegree */
    assignres(n, n.inputsize);
}

/**
 * Calls the visit on n
 * @param n skeleton node
 */
void dcopt::operator()(skel_node &n) {
    n.accept(*this);
}

///////////////////////////////////////////////////////////////////////////////

pipeopt::pipeopt( rpl_environment& env ) :
    optrule( env ), balance( env )
{}

/**
 * Ranks n, optimizes the children, balances the stages then
 * merges the fastest nodes and unranks
 * @param n pipe node
 */
void pipeopt::visit( pipe_node& n ) {
    skel_node* ptr = &n;
    ranktounrank(ptr, ptr);

    for ( size_t i = 0; i < n.size(); i++ ) {
        (*this)( *n.get(i) );                   // optimize sub pipes
        balance( n );                           // balance the stages
    }

    auto run = longestrun(n, ts);               //<total_sum, start_index, end_index>
    while (get<2>(run) - get<1>(run) > 0 ) {
        pipe_merge(n, get<1>(run), get<2>(run));
        run = longestrun(n, ts);
    }

    single_node_cloner snc;
    unranktorank2(*ptr, snc);

}

/**
 * Calls the visit on n
 * @param n skeleton node
 */
void pipeopt::operator()( skel_node& n ) {
    n.accept( *this );
}

///////////////////////////////////////////////////////////////////////////////

pipebalance::pipebalance( rpl_environment& env ) :
    optrule(env), ts_max(0)
{}

/**
 * Calls the visit for all the children except the longest stage
 * (balances the pipe with the slowest stage)
 * @param n pipe node
 */
void pipebalance::visit( pipe_node& n ) {
    auto p = findmax( n, ts );
    size_t idx = p.first;
    double max = p.second;
    for (size_t i = 0; i < n.size(); i++)
        if (i != idx)
            (*this)(*n.get(i), max);
}

/**
 * Gets service time of the child and sets the par degree
 * of n
 * @param n farm node
 */
void pipebalance::visit( farm_node& n ) {
    // iterative solution
    double t = ts(n);
    assign_resources assignres;

    while (t < ts_max && n.pardegree > 1) {
        n.pardegree--;
        assignres(n, n.inputsize);
        t = ts(n);
    }
//    double tw  = ts( *n.get(0) );
//    n.pardegree = ceil( tw / ts_max );
//    if (!n.pardegree)
//        n.pardegree = 1;
}

/**
 * Sets the par degree of n
 * @param n map node
 */
void pipebalance::visit( map_node& n ) {
    // iterative solution
    double t = ts(n);
    assign_resources assignres;

    while (t < ts_max && n.pardegree > 1) {
        n.pardegree--;
        assignres(n, n.inputsize);
        t = ts(n);
    }
    /* in order to get the inner time...  */
//    assign_resources assignres;
//    n.pardegree = 1;
//    assignres(n, n.inputsize);
//
//    double tw  = ts( *n.get(0) );
//    n.pardegree = ceil( tw / ts_max );
//    if (!n.pardegree)
//        n.pardegree = 1;
//    assignres(n, n.inputsize);
}

/**
 * Gets service time of n then sets the minimum
 * possible par degree for n without exceeding
 * the desired service time
 * @param n reduce node
 */
void pipebalance::visit( reduce_node& n ) {
    // iterative solution
    double t = ts(n);
    assign_resources assignres;

    while (t < ts_max && n.pardegree > 1) {
        n.pardegree--;
        assignres(n, n.inputsize);
        t = ts(n);
    }
}

/**
 * Sets the minimum possible parallelism degree
 * to keep the service time of the node under
 * the maximum of the pipeline
 * @param n
 */
void pipebalance::visit(dc_node &n) {
    // iterative solution
    double t = ts(n);
    assign_resources assignres;

    while (t < ts_max && n.pardegree > 1) {
        n.pardegree--;
        assignres(n, n.inputsize);
        t = ts(n);
    }
//    assign_resources assignres;
//    n.pardegree = 1;
//    assignres(n, n.inputsize);
//
//    double tw  = ts( *n.get(0) );
//    n.pardegree = ceil( tw / ts_max );
//    if (!n.pardegree)
//        n.pardegree = 1;
//    assignres(n, n.inputsize);
}

/**
 * Calls the visit on n
 * @param n skeleton root
 */
void pipebalance::operator()( skel_node& n ) {
    n.accept(*this);
}

/**
 * Sets the maximum time service and calls the visit on n
 * @param n skeleton root
 * @param max maximum time service
 */
void pipebalance::operator()( skel_node& n, double max ) {
    ts_max = max;
    n.accept(*this);
}

///////////////////////////////////////////////////////////////////////////////

maxresources::maxresources( rpl_environment& env ) :
    optrule( env ), reduce_res(env), res(env), maxres (0)
{}

/**
 * While possible, reduces resources
 * @param n comp node
 */
void maxresources::visit( comp_node& n ) {
    while ( res(n) > maxres && reduce_res(n) );
}

/**
 * While possible, reduces resources
 * @param n pipe node
 */
void maxresources::visit( pipe_node& n ) {
    while ( res(n) > maxres && reduce_res(n) );
}

/**
 * While possible, reduces resources
 * @param n farm node
 */
void maxresources::visit( farm_node& n ) {
    while ( res(n) > maxres && reduce_res(n) );
}

/**
 * While possible, reduces resources
 * @param n map node
 */
void maxresources::visit( map_node& n ) {
    while ( res(n) > maxres && reduce_res(n) );
}

/**
 * While possible, reduces resources
 * @param n reduce node
 */
void maxresources::visit( reduce_node& n ) {
    while ( res(n) > maxres && reduce_res(n) );
}

/**
 * While possible, reduces resources
 * @param n dc node
 */
void maxresources::visit(dc_node &n) {
    while ( res(n) > maxres && reduce_res(n) );
}

/**
 * Sets maxres and subexp and starts the visit
 * @param n skeleton root
 */
void maxresources::operator()( skel_node& n ) {
    maxres = env.get_res();
    reduce_res.subexp = subexp;
    n.accept(*this);
}

///////////////////////////////////////////////////////////////////////////////

twotier::twotier( rpl_environment& env ) :
    optrule(env)
{}

/**
 * Gets all the sequential nodes in n and substitutes
 * the child of n with a comp of those nodes
 * @param n map node
 */
void twotier::visit( map_node& n ) {
    get_seq_wrappers gsw(env);
    gsw(*n.get(0), true);

    auto* comp = new comp_node{};
    auto seqwrappers = gsw.get_seq_nodes();
    for (auto ptr : seqwrappers)
        comp->add(ptr->clone());

    n.set(comp, 0);
}

/**
 * Gets all the sequential nodes in n and substitutes
 * the child of n with a comp of those nodes
 * @param n reduce node
 */
void twotier::visit( reduce_node& n ) {
    get_seq_wrappers gsw(env);
    gsw(*n.get(0), true);

    auto* comp = new comp_node{};
    auto seqwrappers = gsw.get_seq_nodes();
    for (auto ptr : seqwrappers)
        comp->add(ptr->clone());

    n.set(comp, 0);
}

/**
 * Starts the visit on sk
 * @param sk skeleton root
 */
void twotier::operator()( skel_node& sk ) {
    sk.accept( *this );
}

///////////////////////////////////////////////////////////////////////////////

farmfarmopt::farmfarmopt( rpl_environment& env ) :
    optrule(env)
{}

/**
 * Removes inner farm from n
 * @param n farm node
 */
void farmfarmopt::visit( farm_node& n ) {
    farmelim rule(env);
    n.set( modifytree(*n.get(0), rule), 0 );
}

/**
 * Starts the visit on sk
 * @param sk skeleton root
 */
void farmfarmopt::operator()( skel_node& sk ) {
    sk.accept( *this );
}

///////////////////////////////////////////////////////////////////////////////

mapmapopt::mapmapopt( rpl_environment& env ) :
    optrule(env)
{}

/**
 * Removes inner map in n
 * @param n map node
 */
void mapmapopt::visit( map_node& n ) {
    mapelim rule(env);
    n.set( modifytree(*n.get(0), rule), 0 );
}

/**
 * Starts the visit on sk
 * @param sk skeleton root
 */
void mapmapopt::operator()( skel_node& sk ) {
    sk.accept( *this );
}

///////////////////////////////////////////////////////////////////////////////

// find the index of the heaviest stage of the pipeline/sequence
/**
 * Finds the index of the heaviest stage of n
 * @param n skeleton node (pipe or comp)
 * @param ts visitor that computes service time
 * @return a couple (index, max service time)
 */
pair<size_t, double> findmax( const skel_node& n, servicetime& ts ) {
    size_t idx = 0;
    double max = 0;
    for ( size_t i = 0; i < n.size(); i++ ) {
        double tmp = ts( *n.get(i) );
        if ( max <= tmp ) {
            idx = i;
            max = tmp;
        }
    }
    return { idx, max };
}

/* find the longest subarray where the sum of its elements
   is bounded to the max stage value of the pipeline */
/**
 * Finds the longest subarray where the sum of its elements
 * is bounded to the max stage value of the pipeline
 * @param n pipeline
 * @param ts visitor that computes service time
 * @return a triple (sum, start index, end index)
 */
tuple<int, int, int> longestrun( const pipe_node& n, servicetime& ts )
{
    // find threshold and fill values vector
    size_t thresh = 0;
    std::vector<size_t> values;
    for ( size_t i = 0; i < n.size(); i++ ) {
        size_t value = ts(*n.get(i));
        thresh = thresh < value ? value : thresh;
        values.push_back(value);
    }

    // find the max run such that the sum of its element is < thresh
    size_t sum = 0;
    tuple<size_t,size_t,size_t> max = make_tuple(0, 0, 0);
    tuple<size_t,size_t,size_t> cur = make_tuple(0, 0, 0);
    for ( size_t i = 0; i < values.size(); i++ ) {
        if ( sum + values[i] <= thresh ) {
            sum += values[i];
            cur = make_tuple( sum, get<1>(cur), i );
        } else {
            sum = values[i];
            cur = make_tuple( sum, i, i );
        }
        //if interval of max <= interval of cur
        if (get<2>(max)-get<1>(max) <= get<2>(cur) - get<1>(cur))
            max = cur;  //found a longer subarray
    }

    return max;
}

/**
 * Substitutes the nodes between [left, right], both ends included,
 * with a sequential composition of those nodes
 * @param n pipeline
 * @param left start index of the sequence
 * @param right end index of the sequence
 */
void pipe_merge( pipe_node&n, size_t left, size_t right ) {
    std::vector<skel_node*> rnodes; // nodes at right
    std::vector<skel_node*> mnodes; // nodes between left and right
    while ( n.size() != right+1 )
        rnodes.push_back(n.pop());
    while ( n.size() != left)
        mnodes.push_back(n.pop());

    auto* comp = new comp_node{};
    //the elements in mnodes are in the inverse order
    for (auto rit = mnodes.rbegin(); rit != mnodes.rend(); rit++) {
        comp->add( *rit );  //adds at the end
    }

    //put back nodes in the same order
    n.add(comp);
    for (auto rit = rnodes.rbegin(); rit != rnodes.rend(); rit++){
        n.add( *rit );
    }
}

/**
 * Applies recursively rule r to n and its children
 * as long as possible
 * @param n skeleton node
 * @param r rewriting rule
 * @return pointer to the new node
 */
skel_node* modifytree( skel_node& n, rewrule& r ) {
    skel_node* newptr = r.rewrite(n);
    skel_node* tmp;

    while ( newptr && (tmp = r.rewrite(*newptr)) )
        newptr = tmp;

    if (newptr) {
        for (size_t i = 0; i < newptr->size(); ++i) {
            newptr->set(modifytree(*newptr->get(i), r), i);
        }
    } else {
        for (size_t i = 0; i < n.size(); ++i) {
            n.set(modifytree(*n.get(i), r), i);
        }
    }


//    if ( newptr && newptr->size() == 1) {
//        newptr->set( modifytree(*newptr->get(0), r), 0 );
//    } else if ( newptr && newptr->size() == 2) {
//        newptr->set( modifytree(*newptr->get(0), r), 0 );
//        newptr->set( modifytree(*newptr->get(1), r), 1 );
//    } else if ( n.size() == 1 ) {
//        n.set( modifytree(*n.get(0), r), 0 );
//    } else if ( n.size() == 2) {
//        n.set( modifytree(*n.get(0), r), 0 );
//        n.set( modifytree(*n.get(1), r), 1 );
//    }

    return newptr == nullptr ? n.clone() : newptr;
}
