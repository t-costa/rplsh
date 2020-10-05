#ifndef rpl_skeletons_hpp
#define rpl_skeletons_hpp

#include <string>
#include <vector>
#include "node_interface.hpp"

/**
 * Skeleton node interface, is an rvalue_node
 */
struct skel_node : public rvalue_node {

    skel_node(std::initializer_list<skel_node*> init);
    virtual ~skel_node();

    void accept(visitor& v)                 override = 0;
    virtual skel_node* clone()                      = 0;
    virtual bool operator==( const skel_node& rhs ) = 0;
    virtual bool operator!=( const skel_node& rhs ) = 0;

    /* common annotations */
    double inputsize;

    /* iterable interface */
    skel_node* get( std::size_t idx ) const;
    skel_node* pop();
    void set( skel_node* sk, std::size_t pos );
    void add( skel_node* sk );
    std::size_t size() const;

protected:
    std::vector<skel_node*> children; //default empty
};

/**
 * Class that implements the interface of the skeleton
 * @tparam skeleton one of the possible skeletons node (farm, pipe...)
 */
template <typename skeleton>
struct concrete_skel_node : skel_node {
    void accept(visitor& v) override;
    virtual void setid(std::size_t id);
    virtual std::size_t getid() const;
    bool operator==( const skel_node& rhs ) override;
    bool operator!=( const skel_node& rhs ) override;
protected:
    explicit concrete_skel_node( skeleton& sk );
    concrete_skel_node( skeleton& sk, std::initializer_list<skel_node*> init);
    concrete_skel_node( skeleton& sk, const skeleton& toclone);
private:
    bool match( const skel_node& rhs );
    skeleton& _sk;
    std::size_t id; //default 0
};

/**
 * Sequential node
 */
struct seq_node : public concrete_skel_node<seq_node> {
    seq_node( std::string name, std::string typein, std::string typeout, std::string file );
    explicit seq_node( double servicetime, bool datap_flag = false );
    seq_node( const seq_node& other );
    skel_node* clone() override;
    double servicetime;
    bool datap_flag;
    std::string name;
    std::string typein;
    std::string typeout;
    std::string file;
};

/**
 * Source node for stream
 */
struct source_node : public concrete_skel_node<source_node> {
    source_node( std::string name, std::string typeout, std::string file );
    explicit source_node( double servicetime );
    source_node( const source_node& other );
    skel_node* clone() override;
    double servicetime;
    std::string name;
    std::string typeout;
    std::string file;
};

/**
 * Drain node for stream
 */
struct drain_node : public concrete_skel_node<drain_node> {
    drain_node( std::string name, std::string typein, std::string file );
    explicit drain_node( double servicetime );
    drain_node( const drain_node& other );
    skel_node* clone() override;
    double servicetime;
    std::string name;
    std::string typein;
    std::string file;
};

/**
 * Composition node
 */
struct comp_node : public concrete_skel_node<comp_node> {
    comp_node( std::initializer_list<skel_node*> init );
    comp_node( const comp_node& other );
    skel_node* clone() override;
    bool compseq;   //default false
};

/**
 * Pipeline node
 */
struct pipe_node : public concrete_skel_node<pipe_node> {
    pipe_node( std::initializer_list<skel_node*> init );
    pipe_node( const pipe_node& other );
    skel_node* clone() override;
};

/**
 * Farm node
 */
struct farm_node : public concrete_skel_node<farm_node> {
    farm_node( std::initializer_list<skel_node*> init );
    explicit farm_node( skel_node* pattexp, int pardegree = 1 );
    farm_node( const farm_node& other );
    skel_node* clone() override;
    int pardegree;
};

/**
 * Map node
 */
struct map_node : public concrete_skel_node<map_node> {
    map_node( std::initializer_list<skel_node*> init );
    explicit map_node( skel_node* pattexp, int pardegree = 1 );
    map_node( const map_node& other );
    skel_node* clone() override;
    int pardegree;
};

/**
 * Reduce node
 */
struct reduce_node : public concrete_skel_node<reduce_node> {
    reduce_node( std::initializer_list<skel_node*> init );
    explicit reduce_node( skel_node* pattexp, int pardegree = 1 );
    reduce_node( const reduce_node& other );
    skel_node* clone() override;
    int pardegree;
};

/**
 * Id node
 */
struct id_node : public concrete_skel_node<id_node> {
    id_node( const std::string& id, const int& index, bool all );
    explicit id_node( const std::string& id );
    id_node( const id_node& other );
    skel_node* clone() override;
    std::string id;
    int index;
    bool all;
};

/**
 * Special node that matches with every other node
 */
struct _ : public concrete_skel_node<_> {
    _();
    _(const _& o);
    skel_node* clone() override;
};

#endif
