#ifndef rpl_rewrules_hpp
#define rpl_rewrules_hpp

#include <memory>

#include "nodes/tree_matcher.hpp"
#include "nodes/skeletons.hpp"

struct rewrule;

/**
 * Abstract class for applying all the rewriting rules; it is a tree matcher
 */
struct rewrule : public tree_matcher <skel_node, _>
{
    rewrule( skel_node*&& left0, skel_node*&& left1 = nullptr );
    virtual skel_node* rewrite( skel_node& tree ) = 0;

protected:
    std::unique_ptr<skel_node> left0;
    std::unique_ptr<skel_node> left1;
};

struct farmintro : rewrule {
    farmintro();
    skel_node* rewrite( skel_node& tree );
};

struct farmelim : rewrule {
    farmelim();
    skel_node* rewrite( skel_node& tree );
};

struct pipeintro : rewrule {
    pipeintro();
    skel_node* rewrite( skel_node& tree );
};

struct pipeelim : rewrule {
    pipeelim();
    skel_node* rewrite( skel_node& tree );
};

struct pipeassoc : rewrule {
    pipeassoc();
    skel_node* rewrite( skel_node& tree );
};

struct compassoc : rewrule {
    compassoc();
    skel_node* rewrite( skel_node& tree );
};

struct mapofcomp : rewrule {
    mapofcomp();
    skel_node* rewrite( skel_node& tree );
};

struct compofmap : rewrule {
    compofmap();
    skel_node* rewrite( skel_node& tree );
};

struct mapofpipe : rewrule {
    mapofpipe();
    skel_node* rewrite( skel_node& tree );
};

struct pipeofmap : rewrule {
    pipeofmap();
    skel_node* rewrite( skel_node& tree );
};

struct mapelim : rewrule {
    mapelim();
    skel_node* rewrite( skel_node& tree );
};

struct reduceelim : rewrule {
    reduceelim();
    skel_node* rewrite( skel_node& tree );
};

struct mapmapelim: rewrule {
    mapmapelim();
    skel_node* rewrite( skel_node& tree );
};

struct farmfarmelim: rewrule {
    farmfarmelim();
    skel_node* rewrite( skel_node& tree );
};

struct compdel: rewrule {
    compdel();
    skel_node* rewrite( skel_node& tree );
};

struct pipedel: rewrule {
    pipedel();
    skel_node* rewrite( skel_node& tree );
};

#endif
