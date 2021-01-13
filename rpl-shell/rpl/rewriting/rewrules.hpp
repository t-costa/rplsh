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
    explicit farmintro(rpl_environment& env);
    skel_node* rewrite( skel_node& tree ) override;
};

struct farmelim : rewrule {
    explicit farmelim(rpl_environment& env);
    skel_node* rewrite( skel_node& tree ) override;
};

struct pipeintro : rewrule {
    explicit pipeintro(rpl_environment& env);
    skel_node* rewrite( skel_node& tree ) override;
};

struct pipeelim : rewrule {
    explicit pipeelim(rpl_environment& env);
    skel_node* rewrite( skel_node& tree ) override;
};

struct pipeassoc : rewrule {
    explicit pipeassoc(rpl_environment& env);
    skel_node* rewrite( skel_node& tree ) override;
};

struct compassoc : rewrule {
    explicit compassoc(rpl_environment& env);
    skel_node* rewrite( skel_node& tree ) override;
};

struct mapofcomp : rewrule {
    explicit mapofcomp(rpl_environment& env);
    skel_node* rewrite( skel_node& tree ) override;
};

struct compofmap : rewrule {
    explicit compofmap(rpl_environment& env);
    skel_node* rewrite( skel_node& tree ) override;
};

struct mapofpipe : rewrule {
    explicit mapofpipe(rpl_environment& env);
    skel_node* rewrite( skel_node& tree ) override;
};

struct pipeofmap : rewrule {
    explicit pipeofmap(rpl_environment& env);
    skel_node* rewrite( skel_node& tree ) override;
};

struct mapelim : rewrule {
    explicit mapelim(rpl_environment& env);
    skel_node* rewrite( skel_node& tree ) override;
};

struct reduceelim : rewrule {
    explicit reduceelim(rpl_environment& env);
    skel_node* rewrite( skel_node& tree ) override;
};

struct mapmapelim: rewrule {
    explicit mapmapelim(rpl_environment& env);
    skel_node* rewrite( skel_node& tree ) override;
};

struct farmfarmelim: rewrule {
    explicit farmfarmelim(rpl_environment& env);
    skel_node* rewrite( skel_node& tree ) override;
};

struct compdel: rewrule {
    explicit compdel(rpl_environment& env);
    skel_node* rewrite( skel_node& tree ) override;
};

struct pipedel: rewrule {
    explicit pipedel(rpl_environment& env);
    skel_node* rewrite( skel_node& tree ) override;
};

struct dctomap: rewrule {
    explicit dctomap(rpl_environment& env);
    skel_node* rewrite( skel_node& tree ) override;
};

struct maptodc: rewrule {
    explicit maptodc(rpl_environment& env);
    skel_node* rewrite( skel_node& tree ) override;
};

#endif
