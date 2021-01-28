#include "rewrules.hpp"
#include <iostream>
#include "../utils/rank.hpp"

using namespace std;

///////////////////////////////////////////////////////////////////////////////
//                                                                           //
//          Some useful (for reading purposes) functions and macros          //
//                                                                           //
///////////////////////////////////////////////////////////////////////////////
typedef skel_node* skelptr;

#define __ (new _())                                               // I'm sorry
#define POS(i) (leaves[i]->clone())

inline skelptr Pipe(skelptr s1, skelptr s2) {
    return new pipe_node{s1, s2};
}

inline skelptr Pipe(skelptr s) {
    return new pipe_node{s};
}

inline skelptr Comp(skelptr s1, skelptr s2) {
    return new comp_node{s1, s2};
}

inline skelptr Comp(skelptr s) {
    return new comp_node{s};
}

inline skelptr Farm(skelptr s) {
    return new farm_node(s);
}

inline skelptr Map(skelptr s) {
    return new map_node(s);
}

inline skelptr Reduce(skelptr s) {
    return new reduce_node(s);
}

inline skelptr Dc(skelptr s) {
    return new dc_node(s);
}

void restore_annotations(rpl_environment& env, skelptr n, skelptr old_0, skelptr old_1 = nullptr) {
    auto m = dynamic_cast<map_node*>(n);

    if (m == nullptr) return;

    //retrieve from id if needed
    map_node* m0 = nullptr, *m1 = nullptr;
    auto id_0 = dynamic_cast<id_node*>(old_0);
    if (id_0 != nullptr) {
        auto ptr = env.get(id_0->id, id_0->index);
        m0 = dynamic_cast<map_node*>(ptr.get());
    } else {
        //old_0 is map
        m0 = dynamic_cast<map_node*>(old_0);
    }
    if (old_1 == nullptr) {
        //going from 2 map to 1, reduce pardegree
        m->pardegree =  m0->pardegree / 2;
        m->step = m0->step;
        m->grain = m0->grain;
        m->transformed = m0->transformed;
        return;
    }
    //retrieve second node
    auto id_1 = dynamic_cast<id_node*>(old_1);
    if (id_1 != nullptr) {
        auto ptr = env.get(id_1->id, id_1->index);
        m1 = dynamic_cast<map_node*>(ptr.get());
    } else {
        //old_1 is map
        m1 = dynamic_cast<map_node*>(old_1);
    }
    //going from 2 map to 1, increase pardegree
    m->pardegree = m0->pardegree + m1->pardegree;
    m->step = std::max(m0->step, m1->step);
    m->grain = (std::abs(m0->grain) > std::abs(m1->grain)) ?
               m0->grain :
               m1->grain;
    m->transformed = m0->transformed || m1->transformed;

}

//void restore_annotations(map_node* n, map_node* old_0, map_node* old_1 = nullptr) {
//    //report eventual annotations to the new map
//    if (old_1 != nullptr) {
//        n->pardegree = std::max(old_0->pardegree, old_1->pardegree);
//        n->step = std::max(old_0->step, old_1->step);
//        n->grain = (std::abs(old_0->grain) > std::abs(old_1->grain)) ?
//                   old_0->grain :
//                   old_1->grain;
//        n->transformed = old_0->transformed || old_1->transformed;
//    } else {
//        n->pardegree = old_0->pardegree;
//        n->step = old_0->step;
//        n->grain = old_0->grain;
//        n->transformed = old_0->transformed;
//    }
//}


///////////////////////////////////////////////////////////////////////////////

rewrule::rewrule( rpl_environment& env, skel_node*&& left0, skel_node*&& left1 ) :
    env( env ),
    left0( left0 ),
    left1( left1 )
{}

///////////////////////////////////////////////////////////////////////////////

farmintro::farmintro(rpl_environment& env) : rewrule (
    env, __
){}

/**
 * If possible, builds a farm around the biggest possible
 * pattern tree
 * @param tree tree of patterns to be refactored
 * @return a pointer to the newly created node,
 * nullptr if the rule cannot be applied
 */
skel_node* farmintro::rewrite( skel_node& tree ) {
    return match( &tree, left0.get() ) ?
        Farm( POS(0) ) :
        nullptr;
}

///////////////////////////////////////////////////////////////////////////////

farmelim::farmelim(rpl_environment& env) : rewrule (
    env, Farm(__)
){}

/**
 * If possible, removes the outermost farm
 * @param tree tree of patterns to be refactored
 * @return a pointer to the newly created node,
 * nullptr if the rule cannot be applied
 */
skel_node* farmelim::rewrite( skel_node& tree ) {
    return match( env, &tree, left0.get() ) ?
        POS(0) : nullptr;
}

///////////////////////////////////////////////////////////////////////////////

pipeintro::pipeintro(rpl_environment& env) : rewrule (
    env, Comp(__, __)
){}

/**
 * If possible, builds a pipe around the first two leaves
 * @param tree tree of patterns to be refactored
 * @return a pointer to the newly created node,
 * nullptr if the rule cannot be applied
 */
skel_node* pipeintro::rewrite( skel_node& tree ) {
    return match(env, &tree, left0.get()) ?
        Pipe( POS(0) , POS(1) ) :
        nullptr;
}

///////////////////////////////////////////////////////////////////////////////

pipeelim::pipeelim(rpl_environment& env) : rewrule (
    env, Pipe(__,__)
){}

/**
 * If possible, it transforms the outermost pipe in a composition
 * @param tree tree of patterns to be refactored
 * @return a pointer to the newly created node,
 * nullptr if the rule cannot be applied
 */
skel_node* pipeelim::rewrite( skel_node& tree ) {
    return match(env, &tree, left0.get()) ?
        Comp( POS(0), POS(1) ) :
        nullptr;
}

///////////////////////////////////////////////////////////////////////////////

pipeassoc::pipeassoc(rpl_environment& env) : rewrule (
    env,
    Pipe(Pipe(__,__),__),
    Pipe(__,Pipe(__,__))
){}

/**
 * If possible, builds together two different pipes
 * @param tree tree of patterns to be refactored
 * @return a pointer to the newly created node,
 * nullptr if the rule cannot be applied
 */
skel_node* pipeassoc::rewrite( skel_node& tree ) {
    skel_node* r = nullptr;
    if ( match(env, &tree, left0.get()) )
        return Pipe(POS(0), Pipe(POS(1), POS(2)));
    if ( match(env, &tree, left1.get()))
        return Pipe(Pipe(POS(0), POS(1)), POS(2));
    return r;
}

///////////////////////////////////////////////////////////////////////////////

compassoc::compassoc(rpl_environment& env) : rewrule (
    env,
    Comp(Comp(__,__),__),
    Comp(__,Comp(__,__))
){}

/**
 * If possible, composes two different comps
 * @param tree tree of patterns to be refactored
 * @return a pointer to the newly created node,
 * nullptr if the rule cannot be applied
 */
skel_node* compassoc::rewrite( skel_node& tree ) {
    skel_node* r = nullptr;
    if ( match(env, &tree, left0.get()) )
        return Comp(POS(0), Comp(POS(1), POS(2)));
    if ( match(env, &tree, left1.get()))
        return Comp(Comp(POS(0), POS(1)), POS(2));
    return r;
}

///////////////////////////////////////////////////////////////////////////////

mapofcomp::mapofcomp(rpl_environment& env) : rewrule (
    env, Comp(Map(__), Map(__))
){}

/**
 * If possible, transforms a composition into a map of comp
 * @param tree tree of patterns to be refactored
 * @return a pointer to the newly created node,
 * nullptr if the rule cannot be applied
 */
skel_node* mapofcomp::rewrite( skel_node& tree ) {
//    return match(env, &tree, left0.get()) ?
//        Map(Comp(POS(0), POS(1))) :
//        nullptr;
    if (match(env, &tree, left0.get())) {
        auto m = Map(Comp(POS(0), POS(1)));

        restore_annotations(env, m, tree.get(0), tree.get(1));

        return m;

//        //recovers actual node from id, if needed
//        auto m0 = tree.get(0), m1 = tree.get(1);
//        auto id_0 = dynamic_cast<id_node*>(m0);
//        auto id_1 = dynamic_cast<id_node*>(m1);
//        if (id_0 != nullptr) {
//            auto ptr = env.get(id_0->id, id_0->index);
//            m0 = ptr.get();
//        }
//        if (id_1 != nullptr) {
//            auto ptr = env.get(id_1->id, id_1->index);
//            m1 = ptr.get();
//        }
//        restore_annotations(m, dynamic_cast<map_node*>(m0), dynamic_cast<map_node*>(m1));
//
//        return m;
    } else {
        return nullptr;
    }
}

///////////////////////////////////////////////////////////////////////////////

compofmap::compofmap(rpl_environment& env) : rewrule (
    env, Map(Comp(__,__))
){}

/**
 * If possible, creates a composition of two different maps
 * @param tree tree of patterns to be refactored
 * @return a pointer to the newly created node,
 * nullptr if the rule cannot be applied
 */
skel_node* compofmap::rewrite( skel_node& tree ) {
//    return match(env, &tree, left0.get()) ?
//        Comp(Map(POS(0)), Map(POS(1))) :
//        nullptr;

    if (match(env, &tree, left0.get())) {
        auto m0 = Map(POS(0));
        auto m1 = Map(POS(1));

        //annotate m0 as the original map
        restore_annotations(env, m0, &tree);
        //keep the same annotations for the other map
        restore_annotations(env, m1, &tree);
//
//        auto id = dynamic_cast<id_node*>(&tree);
//        if (id != nullptr) {
//            auto ptr = env.get(id->id, id->index);
//            auto original = dynamic_cast<map_node*>(ptr.get());
//            restore_annotations(m0, original);
//            restore_annotations(m1, original);
//        } else {
//            auto original = dynamic_cast<map_node*>(&tree);
//            restore_annotations(m0, original);
//            restore_annotations(m1, original);
//        }

        return Comp(m0, m1);
    } else {
        return nullptr;
    }
}

///////////////////////////////////////////////////////////////////////////////

mapofpipe::mapofpipe(rpl_environment& env) : rewrule (
    env, Pipe(Map(__), Map(__))
){}

/**
 * If possible, builds a map on top of the pipe
 * @param tree tree of patterns to be refactored
 * @return a pointer to the newly created node,
 * nullptr if the rule cannot be applied
 */
skel_node* mapofpipe::rewrite( skel_node& tree ) {
//    return match(env, &tree, left0.get()) ?
//        Map(Pipe(POS(0), POS(1))) :
//        //Todo: annotate map
//        nullptr;

    if (match(env, &tree, left0.get())) {
        auto m = Map(Pipe(POS(0), POS(1)));

        restore_annotations(env, m, tree.get(0), tree.get(1));

//        //recovers actual node from id, if needed
//        auto m0 = tree.get(0), m1 = tree.get(1);
//        auto id_0 = dynamic_cast<id_node*>(m0);
//        auto id_1 = dynamic_cast<id_node*>(m1);
//        if (id_0 != nullptr) {
//            auto ptr = env.get(id_0->id, id_0->index);
//            m0 = ptr.get();
//        }
//        if (id_1 != nullptr) {
//            auto ptr = env.get(id_1->id, id_1->index);
//            m1 = ptr.get();
//        }
//        restore_annotations(m, dynamic_cast<map_node*>(m0), dynamic_cast<map_node*>(m1));

        return m;
    } else {
        return nullptr;
    }
}

///////////////////////////////////////////////////////////////////////////////

pipeofmap::pipeofmap(rpl_environment& env) : rewrule (
    env, Map(Pipe(__,__))
){}

/**
 * If possible, builds a pipe around two maps
 * @param tree tree of patterns to be refactored
 * @return a pointer to the newly created node,
 * nullptr if the rule cannot be applied
 */
skel_node* pipeofmap::rewrite( skel_node& tree ) {
//    return match(env, &tree, left0.get()) ?
//        Pipe(Map(POS(0)), Map(POS(1))) :
//        //TODO: restore annotations of old map to the new
//        nullptr;

    if (match(env, &tree, left0.get())) {
        auto m0 = Map(POS(0));
        auto m1 = Map(POS(1));

        //annotate m0 as the original map
        restore_annotations(env, m0, &tree);
        //keep the same annotations for the other map
        restore_annotations(env, m1, &tree);

//        auto id = dynamic_cast<id_node*>(&tree);
//        if (id != nullptr) {
//            auto ptr = env.get(id->id, id->index);
//            auto original = dynamic_cast<map_node*>(ptr.get());
//            restore_annotations(m0, original);
//            restore_annotations(m1, original);
//        } else {
//            auto original = dynamic_cast<map_node*>(&tree);
//            restore_annotations(m0, original);
//            restore_annotations(m1, original);
//        }

        return Pipe(m0, m1);
    } else {
        return nullptr;
    }
}

///////////////////////////////////////////////////////////////////////////////

mapelim::mapelim(rpl_environment& env) : rewrule (
    env, Map(__)
){}

/**
 * If possible, removes the outermost map
 * @param tree tree of patterns to be refactored
 * @return a pointer to the newly created node,
 * nullptr if the rule cannot be applied
 */
skel_node* mapelim::rewrite( skel_node& tree ) {
    return match(env, &tree, left0.get()) ?
        POS(0) :
        nullptr;
}

///////////////////////////////////////////////////////////////////////////////

reduceelim::reduceelim(rpl_environment& env) : rewrule (
    env, Reduce(__)
){}

/**
 * If possible, removes the outermost reduce skeleton
 * @param tree tree of patterns to be refactored
 * @return a pointer to the newly created node,
 * nullptr if the rule cannot be applied
 */
skel_node* reduceelim::rewrite( skel_node& tree ) {
    return match(env, &tree, left0.get()) ?
        POS(0) :
        nullptr;
}

///////////////////////////////////////////////////////////////////////////////

mapmapelim::mapmapelim(rpl_environment& env) : rewrule (
    env, Map(Map(__))
){}

/**
 * If possible, merges together two different subsequent maps
 * @param tree tree of patterns to be refactored
 * @return a pointer to the newly created node,
 * nullptr if the rule cannot be applied
 */
skel_node* mapmapelim::rewrite( skel_node& tree ) {
//    return match(env, &tree, left0.get()) ?
//        Map(POS(0)) :
//        //TODO: combine annotations of old maps
//        nullptr;

    if (match(env, &tree, left0.get())) {
        auto m = Map((POS(0)));

        restore_annotations(env, m, &tree, tree.get(0));

//        //recovers actual node from id, if needed
//        auto m0 = &tree, m1 = tree.get(0);
//        auto id_0 = dynamic_cast<id_node*>(m0);
//        auto id_1 = dynamic_cast<id_node*>(m1);
//        if (id_0 != nullptr) {
//            auto ptr = env.get(id_0->id, id_0->index);
//            m0 = ptr.get();
//        }
//        if (id_1 != nullptr) {
//            auto ptr = env.get(id_1->id, id_1->index);
//            m1 = ptr.get();
//        }
//
//        restore_annotations(m, dynamic_cast<map_node*>(m0), dynamic_cast<map_node*>(m1));

        return m;
    } else {
        return nullptr;
    }
}

///////////////////////////////////////////////////////////////////////////////

farmfarmelim::farmfarmelim(rpl_environment& env) : rewrule (
    env, Farm(Farm(__))
){}

/**
 * If possible, merges together two different subsequent farms
 * @param tree tree of patterns to be refactored
 * @return a pointer to the newly created node,
 * nullptr if the rule cannot be applied
 */
skel_node* farmfarmelim::rewrite( skel_node& tree ) {
//    return match(env, &tree, left0.get()) ?
//        Farm(POS(0)) :
//        nullptr;
    if (match(env, &tree, left0.get())) {
        auto f = dynamic_cast<farm_node*>(Farm(POS(0)));
        auto f0 = &tree, f1 = tree.get(0);
        auto id_0 = dynamic_cast<id_node*>(f0);
        auto id_1 = dynamic_cast<id_node*>(f1);
        if (id_0 != nullptr) {
            auto ptr = env.get(id_0->id, id_0->index);
            f0 = ptr.get();
        }
        if (id_1 != nullptr) {
            auto ptr = env.get(id_1->id, id_1->index);
            f1 = ptr.get();
        }
        f->pardegree = dynamic_cast<farm_node*>(f0)->pardegree + dynamic_cast<farm_node*>(f1)->pardegree;
        return f;
    } else {
        return nullptr;
    }
}

///////////////////////////////////////////////////////////////////////////////

compdel::compdel(rpl_environment& env) : rewrule (
    env, Comp(__)
){}

/**
 * If possible, removes the outermost sequential composition
 * @param tree tree of patterns to be refactored
 * @return a pointer to the newly created node,
 * nullptr if the rule cannot be applied
 */
skel_node* compdel::rewrite( skel_node& tree ) {
    return match(env, &tree, left0.get()) ?
        POS(0) :
        nullptr;
}

///////////////////////////////////////////////////////////////////////////////

pipedel::pipedel(rpl_environment& env) : rewrule (
    env, Pipe(__)
){}

/**
 * If possible, removes the outermost pipeline
 * @param tree tree of patterns to be refactored
 * @return a pointer to the newly created node,
 * nullptr if the rule cannot be applied
 */
skel_node* pipedel::rewrite( skel_node& tree ) {
    return match(env, &tree, left0.get()) ?
        POS(0) :
        nullptr;
}

///////////////////////////////////////////////////////////////////////////////

dctomap::dctomap(rpl_environment& env) : rewrule (
    env, Dc(__)
) {}

/**
 * If possible, substitutes the outermost map
 * node with a divide and conquer node
 * @param tree tree of patterns to be refactored
 * @return a pointer to the newly created node,
 * nullptr if the rule cannot be applied
 */
skel_node * dctomap::rewrite(skel_node &tree) {
    if (match(env, &tree, left0.get())) {
        //if i'm here, tree is a dc_node
        auto n = dynamic_cast<map_node*>(Map(POS(0)));
        auto old = dynamic_cast<dc_node&>(tree);
        //annotate or reset the value
        n->transformed = !(old.transformed);
        return n;
    } else {
        return nullptr;
    }
}

///////////////////////////////////////////////////////////////////////////////

maptodc::maptodc(rpl_environment& env) : rewrule(
    env, Map(__)
){}

/**
 * If possible, substitutes the outermost dc
 * node with a map node
 * @param tree tree of patterns to be refactored
 * @return a pointer to the newly created node,
 * nullptr if the rule cannot be applied
 */
skel_node * maptodc::rewrite(skel_node &tree) {
    if (match(env, &tree, left0.get())) {
        //if I'm here, tree is a map node
        auto n = dynamic_cast<dc_node*>(Dc(POS(0)));
        auto old = dynamic_cast<map_node&>(tree);
        //annotate or reset the value
        n->transformed = !(old.transformed);
        return n;
    } else {
        return nullptr;
    }
}
