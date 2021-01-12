#ifndef rpl_rewrules_hpp
#define rpl_rewrules_hpp

#include <memory>

#include "nodes/tree_matcher.hpp"
#include "nodes/skeletons.hpp"
#include "environment/rpl_environment.hpp"

struct rewrule;

/**
 * Abstract class for applying all the rewriting rules; it is a tree matcher
 */
struct rewrule : public tree_matcher <skel_node, _>
{
    explicit rewrule( rpl_environment& env, skel_node*&& left0, skel_node*&& left1 = nullptr );
    virtual skel_node* rewrite( skel_node& tree ) = 0;

protected:
    rpl_environment& env;
    std::unique_ptr<skel_node> left0;
    std::unique_ptr<skel_node> left1;
};

struct farmintro : rewrule {
    farmintro(rpl_environment& env);
    skel_node* rewrite( skel_node& tree ) override;
};

struct farmelim : rewrule {
    farmelim(rpl_environment& env);
    skel_node* rewrite( skel_node& tree ) override;
};

struct pipeintro : rewrule {
    pipeintro(rpl_environment& env);
    skel_node* rewrite( skel_node& tree ) override;
};

struct pipeelim : rewrule {
    pipeelim(rpl_environment& env);
    skel_node* rewrite( skel_node& tree ) override;
};

struct pipeassoc : rewrule {
    pipeassoc(rpl_environment& env);
    skel_node* rewrite( skel_node& tree ) override;
};

struct compassoc : rewrule {
    compassoc(rpl_environment& env);
    skel_node* rewrite( skel_node& tree ) override;
};

struct mapofcomp : rewrule {
    mapofcomp(rpl_environment& env);
    skel_node* rewrite( skel_node& tree ) override;
};

struct compofmap : rewrule {
    compofmap(rpl_environment& env);
    skel_node* rewrite( skel_node& tree ) override;
};

struct mapofpipe : rewrule {
    mapofpipe(rpl_environment& env);
    skel_node* rewrite( skel_node& tree ) override;
};

struct pipeofmap : rewrule {
    pipeofmap(rpl_environment& env);
    skel_node* rewrite( skel_node& tree ) override;
};

struct mapelim : rewrule {
    mapelim(rpl_environment& env);
    skel_node* rewrite( skel_node& tree ) override;
};

struct reduceelim : rewrule {
    reduceelim(rpl_environment& env);
    skel_node* rewrite( skel_node& tree ) override;
};

struct mapmapelim: rewrule {
    mapmapelim(rpl_environment& env);
    skel_node* rewrite( skel_node& tree ) override;
};

struct farmfarmelim: rewrule {
    farmfarmelim(rpl_environment& env);
    skel_node* rewrite( skel_node& tree ) override;
};

struct compdel: rewrule {
    compdel(rpl_environment& env);
    skel_node* rewrite( skel_node& tree ) override;
};

struct pipedel: rewrule {
    pipedel(rpl_environment& env);
    skel_node* rewrite( skel_node& tree ) override;
};

struct dctomap: rewrule {
    dctomap(rpl_environment& env);
    skel_node* rewrite( skel_node& tree ) override;
};

struct maptodc: rewrule {
    maptodc(rpl_environment& env);
    skel_node* rewrite( skel_node& tree ) override;
};

#endif
