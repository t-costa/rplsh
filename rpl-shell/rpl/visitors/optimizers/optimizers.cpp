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

void optrule::visit( seq_node& n ) {
}

void optrule::visit( comp_node& n ) {
    for (size_t i = 0; i < n.size(); i++)
        (*this)( *n.get(i) );
}

void optrule::visit( pipe_node& n ) {
    for (size_t i = 0; i < n.size(); i++)
        (*this)( *n.get(i) );
}

void optrule::visit( farm_node& n ) {
    (*this)( *n.get(0) );
}

void optrule::visit( map_node& n ) {
    (*this)( *n.get(0) );
}

void optrule::visit( reduce_node& n ) {
    (*this)( *n.get(0) );
}

void optrule::visit( id_node& n ) {
    if (!subexp)
        return;

    auto ptr = env.get(n.id, n.index);
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

void farmopt::visit( farm_node& n ) {
    (*this)( *n.get(0) );
    double tw = ts( *n.get(0) );
    double te = env.get_emitter_time();
    size_t nw = ceil( tw / te );
    n.pardegree = nw;
}

void farmopt::operator()( skel_node& sk ) {
    sk.accept( *this );
}

///////////////////////////////////////////////////////////////////////////////

mapopt::mapopt( rpl_environment& env ) :
    optrule(env)
{}

void mapopt::visit( map_node& n ) {
    /* reassign resources assuming only one worker */
    assign_resources assignres;
    n.pardegree = 1;
    assignres(n, n.inputsize);

    /* compute the "optimal" pardegree */
    double tsc = env.get_scatter_time();
    double tsg = env.get_gather_time();
    double tw  = ts( *n.get(0) );
    size_t nw  = ceil( sqrt( tw / max(tsc,tsg) ) );
    n.pardegree = nw;

    /* reassign resources with the new pardegree */
    assignres(n, n.inputsize);

    /* recurse; TODO or not? */
    (*this)( *n.get(0) );
}

void mapopt::operator()( skel_node& sk ) {
    sk.accept( *this );
}

///////////////////////////////////////////////////////////////////////////////

reduceopt::reduceopt( rpl_environment& env ) :
    optrule(env)
{}

void reduceopt::visit( reduce_node& n ) {
    //std::cout << "reduceopt first: " << n.pardegree << " - " << n.inputsize << std::endl;
    /* reassign resources assuming only one worker */
    assign_resources assignres;
    n.pardegree = 1;
    assignres(n, n.inputsize);

    //std::cout << "reduceopt second: " << n.pardegree << " - " << n.inputsize << std::endl;
    /* compute the optimal number of workers */
    n.pardegree = ( ((double) n.inputsize) * log(2) );
    assignres(n, n.inputsize);
    //std::cout << "npardegree : " << n.pardegree << std::endl;
    //std::cout << "reduceopt third: " << n.pardegree << " - " << n.inputsize << std::endl;
    /* recurse; TODO Or not?*/
    (*this)( *n.get(0) );
}

void reduceopt::operator()( skel_node& sk ) {
    sk.accept( *this );
}

///////////////////////////////////////////////////////////////////////////////

pipeopt::pipeopt( rpl_environment& env ) :
    optrule( env ), balance( env )
{}

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

void pipeopt::operator()( skel_node& n ) {
    n.accept( *this );
}

///////////////////////////////////////////////////////////////////////////////

pipebalance::pipebalance( rpl_environment& env ) :
    optrule(env)
{}

void pipebalance::visit( pipe_node& n ) {
    auto p = findmax( n, ts );
    size_t idx = p.first;
    double max = p.second;
    for (size_t i = 0; i < n.size(); i++)
        if (i != idx)
            (*this)(*n.get(i), max);
}

void pipebalance::visit( farm_node& n ) {
    double tw  = ts( *n.get(0) );
    n.pardegree = ceil( tw / ts_max );
    if (!n.pardegree)
        n.pardegree = 1;
}

void pipebalance::visit( map_node& n ) {
    /* in order to get the inner time...  */
    assign_resources assignres;
    n.pardegree = 1;
    assignres(n, n.inputsize);

    double tw  = ts( *n.get(0) );
    n.pardegree = ceil( tw / ts_max );
    if (!n.pardegree)
        n.pardegree = 1;
    assignres(n, n.inputsize);
}

void pipebalance::visit( reduce_node& n ) {
    // iterative solution
    double t = ts(n);
    assign_resources assignres;

    while (t < ts_max && 1 < n.pardegree) {

        n.pardegree--;
        assignres(n, n.inputsize);
        t = ts(n);

    }

}

void pipebalance::operator()( skel_node& n ) {
    n.accept(*this);
}

void pipebalance::operator()( skel_node& n, double max ) {
    ts_max = max;
    n.accept(*this);
}

///////////////////////////////////////////////////////////////////////////////

maxresources::maxresources( rpl_environment& env ) :
    optrule( env ), reduce_res(env), res(env)
{}

void maxresources::visit( comp_node& n ) {
    while ( res(n) > maxres && reduce_res(n) );
}

void maxresources::visit( pipe_node& n ) {
    while ( res(n) > maxres && reduce_res(n) );
}

void maxresources::visit( farm_node& n ) {
    while ( res(n) > maxres && reduce_res(n) );
}

void maxresources::visit( map_node& n ) {
    while ( res(n) > maxres && reduce_res(n) );
}

void maxresources::visit( reduce_node& n ) {
    while ( res(n) > maxres && reduce_res(n) );
}

void maxresources::operator()( skel_node& n ) {
    maxres = env.get_res();
    reduce_res.subexp = subexp;
    n.accept(*this);
}

///////////////////////////////////////////////////////////////////////////////

twotier::twotier( rpl_environment& env ) :
    optrule(env)
{}

void twotier::visit( map_node& n ) {
    get_seq_wrappers gsw(env);
    gsw(*n.get(0));

    comp_node* comp = new comp_node{};
    auto seqwrappers = gsw.get_seq_nodes();
    for (auto ptr : seqwrappers)
        comp->add(ptr->clone());

    n.set(comp, 0);
}

void twotier::visit( reduce_node& n ) {
    get_seq_wrappers gsw(env);
    gsw(*n.get(0));

    comp_node* comp = new comp_node{};
    auto seqwrappers = gsw.get_seq_nodes();
    for (auto ptr : seqwrappers)
        comp->add(ptr->clone());

    n.set(comp, 0);
}

void twotier::operator()( skel_node& sk ) {
    sk.accept( *this );
}

///////////////////////////////////////////////////////////////////////////////

farmfarmopt::farmfarmopt( rpl_environment& env ) :
    optrule(env)
{}

void farmfarmopt::visit( farm_node& n ) {
    farmelim rule;
    n.set( modifytree(*n.get(0), rule), 0 );
}

void farmfarmopt::operator()( skel_node& sk ) {
    sk.accept( *this );
}

///////////////////////////////////////////////////////////////////////////////

mapmapopt::mapmapopt( rpl_environment& env ) :
    optrule(env)
{}

void mapmapopt::visit( map_node& n ) {
    mapelim rule;
    n.set( modifytree(*n.get(0), rule), 0 );
}

void mapmapopt::operator()( skel_node& sk ) {
    sk.accept( *this );
}

///////////////////////////////////////////////////////////////////////////////

// find the index of the heaviest stage of the pipeline/sequence
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
        if (get<2>(max)-get<1>(max) <= get<2>(cur) - get<1>(cur))
            max = cur;
    }

    return max;
}

void pipe_merge( pipe_node&n, size_t left, size_t right ) {

    std::vector<skel_node*> rnodes; // nodes at right
    std::vector<skel_node*> mnodes; // nodes between left and right
    while ( n.size() != right+1 )
        rnodes.push_back(n.pop());
    while ( n.size() != left)
        mnodes.push_back(n.pop());

    skel_node* comp = new comp_node{};
    for (auto rit = mnodes.rbegin(); rit != mnodes.rend(); rit++)
        comp->add( *rit );

    n.add(comp);
    for (auto rit = rnodes.rbegin(); rit != rnodes.rend(); rit++)
        n.add( *rit );

}

skel_node* modifytree( skel_node& n, rewrule& r ) {
    skel_node* newptr = r.rewrite(n);
    skel_node* tmp;

    while ( newptr && (tmp = r.rewrite(*newptr)) )
        newptr = tmp;

    /* TODO really lazy now, should be written in a more concise way */
    if ( newptr && newptr->size() == 1) {
        newptr->set( modifytree(*newptr->get(0), r), 0 );
    } else if ( newptr && newptr->size() == 2) {
        newptr->set( modifytree(*newptr->get(0), r), 0 );
        newptr->set( modifytree(*newptr->get(1), r), 1 );
    } else if ( n.size() == 1 ) {
        n.set( modifytree(*n.get(0), r), 0 );
    } else if ( n.size() == 2) {
        n.set( modifytree(*n.get(0), r), 0 );
        n.set( modifytree(*n.get(1), r), 1 );
    }

    return newptr == nullptr ? n.clone() : newptr;
}
