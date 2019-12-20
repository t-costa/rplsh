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
    explicit rewrule( skel_node*&& left0, skel_node*&& left1 = nullptr );
    virtual skel_node* rewrite( skel_node& tree ) = 0;

protected:
    std::unique_ptr<skel_node> left0;
    std::unique_ptr<skel_node> left1;
};

struct farmintro : rewrule {
    farmintro();
    skel_node* rewrite( skel_node& tree ) override;
};

struct farmelim : rewrule {
    farmelim();
    skel_node* rewrite( skel_node& tree ) override;
};

struct pipeintro : rewrule {
    pipeintro();
    skel_node* rewrite( skel_node& tree ) override;
};

struct pipeelim : rewrule {
    pipeelim();
    skel_node* rewrite( skel_node& tree ) override;
};

struct pipeassoc : rewrule {
    pipeassoc();
    skel_node* rewrite( skel_node& tree ) override;
};

struct compassoc : rewrule {
    compassoc();
    skel_node* rewrite( skel_node& tree ) override;
};

struct mapofcomp : rewrule {
    mapofcomp();
    skel_node* rewrite( skel_node& tree ) override;
};

struct compofmap : rewrule {
    compofmap();
    skel_node* rewrite( skel_node& tree ) override;
};

struct mapofpipe : rewrule {
    mapofpipe();
    skel_node* rewrite( skel_node& tree ) override;
};

struct pipeofmap : rewrule {
    pipeofmap();
    skel_node* rewrite( skel_node& tree ) override;
};

struct mapelim : rewrule {
    mapelim();
    skel_node* rewrite( skel_node& tree ) override;
};

struct reduceelim : rewrule {
    reduceelim();
    skel_node* rewrite( skel_node& tree ) override;
};

struct mapmapelim: rewrule {
    mapmapelim();
    skel_node* rewrite( skel_node& tree ) override;
};

struct farmfarmelim: rewrule {
    farmfarmelim();
    skel_node* rewrite( skel_node& tree ) override;
};

struct compdel: rewrule {
    compdel();
    skel_node* rewrite( skel_node& tree ) override;
};

struct pipedel: rewrule {
    pipedel();
    skel_node* rewrite( skel_node& tree ) override;
};

#endif
