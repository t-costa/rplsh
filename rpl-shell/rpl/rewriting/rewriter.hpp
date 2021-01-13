#pragma once

#include "rewrules.hpp"
#include "nodes/skeletons.hpp"
#include "visitors/visitors.hpp"
#include <unordered_map>

using namespace std;

typedef unordered_map<string, skel_node*>   node_set;
typedef pair<skel_node*, skel_node*>        pair_node;

void combine( skel_node& root, vector<pair_node>&& pairs, node_set& to_set );
void combine (skel_node& root, node_set&& from_set, node_set& to_set);
vector<pair_node> allpairs( node_set&& s1, node_set&& s2);
node_set merge(node_set& s1, node_set& s2);
void delset( node_set& s );

/**
 * Class that applies the rewriting rules
 */
struct rewriter {
    explicit rewriter(bool rec) : rec(rec) {}
    node_set apply_allrules ( skel_node& tree, rr_dispatcher& rr_disp);
    template <typename Iterator>
    node_set apply_allrules ( Iterator& begin, Iterator& end, rr_dispatcher& rr_disp);
    template <typename Iterator>
    node_set apply_rule ( Iterator& begin, Iterator& end, rewrule& r);

private:
    printer print;
    //single_node_cloner snc;
    skel_node* rewrite( skel_node& n, rewrule& r );
    node_set fullrecrewrite( skel_node& n, rewrule& r );
    void insert_or_delete( node_set& set, skel_node* rn );

    //when is set to true, the rewrules are considered as recrewrules:
    //the rewriter will apply the rule recursively modifying the same skel tree
    bool rec;
};

/**
 * Applies the rewrule passed to all the nodes in the range defined by the iterators
 * @tparam Iterator type of the iterator
 * @param begin starting position of the range
 * @param end end of the range, the element pointed by it is not considered
 * @param r rewrule to be applied
 * @return the set of nodes on which the rule has been applied
 */
template <typename Iterator>
node_set rewriter::apply_rule ( Iterator& begin, Iterator& end, rewrule& r) {
    node_set set;
    for ( auto it = begin ; it != end; ++it) {
        auto& skelptr = *it;
        insert_or_delete ( set, rewrite(*skelptr, r) );
    }
    return set;
}

/**
 * Applies all the rules to all the nodes in the passed tree
 * @param tree skeleton tree on which to apply the rules
 * @param rr_disp dispatcher of the rewriting rules
 * @return the set of the new nodes
 */
node_set rewriter::apply_allrules ( skel_node& tree, rr_dispatcher& rr_disp) {
    node_set set;
    vector<node_set> sets;
    for (auto& str : rr_disp.get_allrules())
        sets.push_back( fullrecrewrite(tree, *rr_disp[str]) );
    for (auto& curr : sets)
        set = merge(set, curr);
    return set;
}

/**
 * Applies all the rules to all the subtrees whose root is between begin and end
 * @tparam Iterator type of the iterator
 * @param begin starting point of the range, the node pointed by it is included
 * @param end end point of the range, the node pointed by it is NOT included
 * @param rr_disp dispatcher of the rewriting rules
 * @return the set of the new nodes
 */
template <typename Iterator>
node_set rewriter::apply_allrules ( Iterator& begin, Iterator& end, rr_dispatcher& rr_disp) {
    node_set set;
    vector<node_set> sets;
    for (auto it = begin; it != end; it++) {
        auto& skelptr = *it;
        sets.push_back( apply_allrules(*skelptr, rr_disp) );
    }
    for (auto& curr : sets)
        set = merge(set, curr);
    return set;
}

/**
 * Modifies the passed node using the given rule; if recursion
 * is enabled, the rule is applied to all the children of n
 * @param n skeleton node on which the rewriting has to be applied
 * @param r rule to use for the rewriting
 * @return the modified node if everything goes ok,
 * a clone of the original node with modified children otherwise
 */
skel_node* rewriter::rewrite( skel_node& n, rewrule& r ) {
    skel_node* newptr = r.rewrite(n);
    skel_node* tmp;

    if (rec && (dynamic_cast<pipeassoc*>(&r) ||
                dynamic_cast<compassoc*>(&r) ||
                dynamic_cast<farmintro*>(&r))) {
        std::cout << "Warning: recursive functionality is not available for "
                     "associative rules and farmintro rule. "
                     "They could cause an infinite loop." << std::endl;
    } else {
        /* for rec support */
        while ( rec && newptr && (tmp = r.rewrite(*newptr)) )
            newptr = tmp;

        /* TODO really lazy now, should be written in a more concise way */
        if ( rec && newptr && newptr->size() == 1) {
            newptr->set( rewrite(*newptr->get(0), r), 0 );
        } else if ( rec && newptr && newptr->size() == 2) {
            newptr->set( rewrite(*newptr->get(0), r), 0 );
            newptr->set( rewrite(*newptr->get(1), r), 1 );
        } else if ( rec && n.size() == 1 ) {
            n.set( rewrite(*n.get(0), r), 0 );
        } else if ( rec && n.size() == 2) {
            n.set( rewrite(*n.get(0), r), 0 );
            n.set( rewrite(*n.get(1), r), 1 );
        }
    }

    return newptr == nullptr ? n.clone() : newptr;
}

/**
 * Applies recursively the rule r to the node n and its children
 * @param n node on which to apply the rule
 * @param r rule to be applied
 * @return the set of the modified nodes
 */
node_set rewriter::fullrecrewrite( skel_node& n, rewrule& r ) {
    node_set set;
    unique_ptr<skel_node> rn( rewrite(n, r) );
    insert_or_delete( set, n.clone() );
    insert_or_delete( set, rn->clone() );

    if ( n.size() == 1 )
        combine(n, fullrecrewrite(*n.get(0), r), set);
    else if ( n.size() == 2 ) { // Comp or Pipe
        combine(n, allpairs(fullrecrewrite(*n.get(0),r), fullrecrewrite(*n.get(1), r)), set);
        if (rn != nullptr && rn->size() == 2)
            combine(*rn, allpairs(fullrecrewrite(*rn->get(0),r), fullrecrewrite(*rn->get(1), r)), set);
    }

    return set;
}

/**
 * Inserts the passed node to the set; if the insertion fails
 * the node is deleted
 * @param set set of nodes on which to insert rn
 * @param rn pointer to the node to be inserted
 */
void rewriter::insert_or_delete( node_set& set, skel_node* rn ) {
    if (rn != nullptr) {
        auto p = set.insert({print(*rn), rn});
        if (!p.second)
            delete rn;
    }
}

/**
 * Clones the node n, adds p1 to the copy of n and tries
 * to insert it in s
 * @param s set on which to insert the node
 * @param n node to be cloned
 * @param p1 node to be added to the copy of n
 */
void comb_and_ins( node_set& s, skel_node& n, skel_node* p1 ) {
    single_node_cloner snc;
    skel_node * rn = snc(n);
    printer print;
    rn->add(p1);
    auto p = s.insert({print(*rn), rn});
    if (!p.second)
        delete rn;
}

/**
 * Clones the node n, adds p1 and p2 to the copy and
 * tries to insert it in s
 * @param s set on which to insert the node
 * @param n node to be cloned
 * @param p1 first node to be added to the copy of n
 * @param p2 second node to be added to the copy of n
 */
void comb_and_ins( node_set& s, skel_node& n, skel_node* p1, skel_node* p2 ) {
    single_node_cloner snc;
    skel_node * rn = snc(n);
    printer print;
    rn->add(p1);
    rn->add(p2);
    auto p = s.insert({print(*rn), rn});
    if (!p.second)
        delete rn;
}

/**
 * Calls the comb_and_ins for each pair of nodes in pairs
 * @param root node that will be cloned in comb_and_ins
 * @param pairs vector of pairs of nodes to be added to root
 * @param to_set set on which to insert the root
 */
void combine( skel_node& root, vector<pair_node>&& pairs, node_set& to_set ) {
    for (auto& p : pairs)
        comb_and_ins(to_set, root, p.first, p.second);
}

/**
 * Calls the comb_and_ins for all the nodes in from_set
 * @param root node that will be cloned in comb_and_ins
 * @param from_set list of nodes to be added to root
 * @param to_set set on which to insert the root
 */
void combine (skel_node& root, node_set&& from_set, node_set& to_set) {
    for (auto& it : from_set)
        comb_and_ins(to_set, root, it.second);
}

/**
 * Deletes all the nodes in s
 * @param s set from which the nodes will be deleted
 */
void delset( node_set& s ) {
    for (auto& it : s )
        delete it.second;
}

/**
 * Inserts all the nodes in s2 in s1
 * @param s1 set that will contain all the nodes
 * @param s2 set of nodes to be added to the first one
 * @return the complete set
 */
node_set merge(node_set& s1, node_set& s2) {
    //vector<skel_node*> nodes;
    for (auto& p : s2) {
        auto o = s1.insert(p);
        if (!o.second)
            delete p.second;
    }
    return s1;
}

/**
 * Removes all the elements in the two sets and inserts them
 * in a new vector of all the possible pairs (node in s1, node in s2)
 * @param s1 first set of nodes
 * @param s2 second set of nodes
 * @return the vector of all the possible pairs
 */
vector<pair_node> allpairs( node_set&& s1, node_set&& s2) {
    vector<pair_node> pairs;
    for (auto& it1 : s1 )
        for (auto& it2 : s2)
            pairs.emplace_back(it1.second->clone(), it2.second->clone());
    delset(s1);
    delset(s2);
    return pairs;
}
