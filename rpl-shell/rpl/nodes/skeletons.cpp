#include <iostream>
#include <utility>
#include "skeletons.hpp"
#include "visitors/visitor_interface.hpp"

using namespace std;

///////////////////////////////////////////////////////////////////////////////
// skel_node implementation
///////////////////////////////////////////////////////////////////////////////
skel_node::skel_node( initializer_list<skel_node*> init )
    : inputsize (1), children(init) {}

skel_node::~skel_node() {
    for (auto sk : children)
        delete sk;
}

/**
 * @param idx position of the desired node
 * @return skeleton node, nullptr if idx not present
 */
skel_node* skel_node::get( size_t idx ) const {
    return idx < children.size() ? children[idx] : nullptr;
}

/**
 * @return the last skeleton node if it exists, nullptr otherwise
 */
skel_node* skel_node::pop() {
    if (!children.empty()) {
        skel_node* last = children.back();
        children.pop_back();
        return last;
    }
    return nullptr;
}

/**
 * @param sk node to be added at the end of the list
 */
void skel_node::add( skel_node* sk ) {
    children.push_back(sk);
}

/**
 * Deletes the old node and then sets the new one
 * @param sk new node
 * @param pos position of the old node to be removed and of the new node
 */
void skel_node::set( skel_node* sk, size_t pos) {
    //FIXME: this is not a solution, maybe there is nothing to do?
    if (pos >= children.size() || pos < 0) {
        std::cerr << "Error! Pos is outside the limit in skel_node::set!" << std::endl;
        exit(1);
    }
    delete children[pos];
    children[pos] = sk;
}

/**
 * @return size of the list of children nodes
 */
std::size_t skel_node::size() const {
    return children.size();
}


///////////////////////////////////////////////////////////////////////////////
// concrete_skel_node implementation
///////////////////////////////////////////////////////////////////////////////
template <typename skeleton>
concrete_skel_node<skeleton>::concrete_skel_node( skeleton& sk )
        : skel_node({}), _sk(sk) , id ( 0 ) {}

template <typename skeleton>
concrete_skel_node<skeleton>::concrete_skel_node( skeleton& sk, const skeleton& toclone )
        : skel_node({}), _sk(sk), id ( 0 ) {

    inputsize = toclone.inputsize;
    for (size_t i = 0; i < toclone.size(); i++)
        add( toclone.get(i)->clone() );

}

template <typename skeleton>
concrete_skel_node<skeleton>::concrete_skel_node( skeleton& sk, initializer_list<skel_node*> init)
        : skel_node(init), _sk(sk), id ( 0 ) {}
        
/**
 * Calls the visit operation of the passed visitor
 * @tparam skeleton type of the node
 * @param v visitor that calls the visit on the private node
 */
template <typename skeleton>
void concrete_skel_node<skeleton>::accept( visitor & v ) {
    v.visit(_sk);
}

/**
 * Sets the private variable id
 * @tparam skeleton type of the node
 * @param _id new value of the id
 */
template <typename skeleton>
void concrete_skel_node<skeleton>::setid( size_t _id ) {
    id = _id;
}

/**
 * @tparam skeleton type of the node
 * @return the value of id
 */
template <typename skeleton>
size_t concrete_skel_node<skeleton>::getid() const {
    return id;
}

/**
 * Checks if the passed skel_node is of type skeleton
 * @tparam skeleton type of the node
 * @param rhs node to be checked
 * @return true iff rhs has the same dynamic type of skeleton
 */
template <typename skeleton>
bool concrete_skel_node<skeleton>::match ( const skel_node& rhs ) {
    return dynamic_cast<const skeleton*>(&rhs);
}

template <typename skeleton>
bool concrete_skel_node<skeleton>::operator==( const skel_node& rhs ) {
    return match(rhs);
}

template <typename skeleton>
bool concrete_skel_node<skeleton>::operator!=( const skel_node& rhs ) {
    return !match(rhs);
}


///////////////////////////////////////////////////////////////////////////////
// seq_node implementation
///////////////////////////////////////////////////////////////////////////////
seq_node::seq_node( double servicetime, bool datap_flag ) :
    concrete_skel_node( *this ),
    servicetime(servicetime),
    datap_flag(datap_flag),
    dc_flag(false)
{}

seq_node::seq_node( std::string name, std::string typein, std::string typeout, std::string file ) :
    concrete_skel_node( *this ),
    servicetime(1.0),
    datap_flag(false),
    dc_flag(false),
    name(std::move(name)),
    typein(std::move(typein)),
    typeout(std::move(typeout)),
    file(std::move(file))
{}

//seq_node::seq_node(std::string name, std::string typein, std::string typeout, std::string typein_el,
//                   std::string typeout_el, std::string file) :
//    concrete_skel_node<seq_node>(*this),
//    servicetime(1.0),
//    datap_flag(true),   //it's for a map or reduce
//    dc_flag(false),
//    name(std::move(name)),
//    typein(std::move(typein)),
//    typeout(std::move(typeout)),
//    typein_el(std::move(typein_el)),
//    typeout_el(std::move(typeout_el)),
//    file(std::move(file))
//{}

seq_node::seq_node( const seq_node& other ) :
    concrete_skel_node( *this, other ),
    servicetime(other.servicetime),
    datap_flag(other.datap_flag),
    dc_flag(other.dc_flag),
    name(other.name),
    typein(other.typein),
    typeout(other.typeout),
//    typein_el(other.typein_el),
//    typeout_el(other.typeout_el),
    file(other.file)
{}

skel_node* seq_node::clone() {
    return new seq_node(*this);
}


///////////////////////////////////////////////////////////////////////////////
// source_node implementation
///////////////////////////////////////////////////////////////////////////////
source_node::source_node( double servicetime ) :
    concrete_skel_node( *this ),
    servicetime(servicetime)
{}

source_node::source_node( string name, string typeout, string file ) :
    concrete_skel_node( *this ),
    servicetime(1.0),
    name(std::move(name)),
    typeout(std::move(typeout)),
    file(std::move(file))
{}

source_node::source_node( const source_node& other ) :
    concrete_skel_node( *this, other ),
    servicetime(other.servicetime),
    name(other.name),
    typeout(other.typeout),
    file(other.file)
{}

skel_node* source_node::clone() {
    return new source_node(*this);
}


///////////////////////////////////////////////////////////////////////////////
// drain_node implementation
///////////////////////////////////////////////////////////////////////////////
drain_node::drain_node( double servicetime ) :
    concrete_skel_node( *this ),
    servicetime(servicetime)
{}

drain_node::drain_node( string name, string typein, string file ) :
    concrete_skel_node( *this ),
    servicetime(1.0),
    name(std::move(name)),
    typein(std::move(typein)),
    file(std::move(file))
{}

drain_node::drain_node( const drain_node& other ) :
    concrete_skel_node( *this, other ),
    servicetime(other.servicetime),
    name(other.name),
    typein(other.typein),
    file(other.file)
{}

skel_node* drain_node::clone() {
    return new drain_node(*this);
}


///////////////////////////////////////////////////////////////////////////////
// comp_node implementation
///////////////////////////////////////////////////////////////////////////////
comp_node::comp_node( initializer_list<skel_node*> init )
    : concrete_skel_node( *this, init ), compseq (false) {}

comp_node::comp_node( const comp_node& other )
    : concrete_skel_node( *this, other ), compseq (false) {}

skel_node * comp_node::clone() {
    return new comp_node(*this);
}


///////////////////////////////////////////////////////////////////////////////
// pipe_node implementation
///////////////////////////////////////////////////////////////////////////////
pipe_node::pipe_node( initializer_list<skel_node*> init )
    : concrete_skel_node( *this, init ) {}

pipe_node::pipe_node( const pipe_node& other )
    : concrete_skel_node( *this, other ) {}

skel_node * pipe_node::clone() {
    return new pipe_node(*this);
}


///////////////////////////////////////////////////////////////////////////////
// farm_node implementation
///////////////////////////////////////////////////////////////////////////////
farm_node::farm_node(initializer_list<skel_node*> init)
    : concrete_skel_node( *this, init ), pardegree(1) {}

farm_node::farm_node( skel_node* pattexp, int pardegree )
    : concrete_skel_node( *this ), pardegree( pardegree ) {
    add(pattexp);
}

farm_node::farm_node(const farm_node& other)
        : concrete_skel_node( *this, other ), pardegree(other.pardegree) {}

skel_node* farm_node::clone() {
    return new farm_node(*this);
}

///////////////////////////////////////////////////////////////////////////////
// map_node implementation
///////////////////////////////////////////////////////////////////////////////
map_node::map_node(initializer_list<skel_node*> init)
    : concrete_skel_node( *this, init ), pardegree(1), grain(0), transformed(false) {}

map_node::map_node(skel_node* pattexp, int pardegree)
    : concrete_skel_node( *this ), pardegree( pardegree ), grain(0), transformed(false) {
    add(pattexp);
}

map_node::map_node(const map_node& other)
    : concrete_skel_node( *this, other ), pardegree(other.pardegree), grain(other.grain), transformed(other.transformed) {}

skel_node * map_node::clone() {
    return new map_node(*this);
}


///////////////////////////////////////////////////////////////////////////////
// reduce_node implementation
///////////////////////////////////////////////////////////////////////////////
reduce_node::reduce_node(initializer_list<skel_node*> init)
    : concrete_skel_node( *this, init ), pardegree(1), grain(0) {}

reduce_node::reduce_node( skel_node* pattexp, int pardegree )
    : concrete_skel_node( *this ), pardegree(pardegree), grain(0) {
    add(pattexp);
}

reduce_node::reduce_node( const reduce_node& other )
    : concrete_skel_node( *this, other ), pardegree(other.pardegree), grain(other.grain) {}

skel_node* reduce_node::clone() {
    return new reduce_node(*this);
}

///////////////////////////////////////////////////////////////////////////////
// dc_node implementation
///////////////////////////////////////////////////////////////////////////////
dc_node::dc_node(std::initializer_list<skel_node *> init)
    : concrete_skel_node(*this, init), pardegree(1), transformed(false), cutoff(1), schedule(2) {}

dc_node::dc_node(skel_node *pattexp, int pardegree)
    : concrete_skel_node(*this), pardegree(pardegree), transformed(false), cutoff(1), schedule(2) {
    add(pattexp);
}

dc_node::dc_node(const dc_node &other)
    : concrete_skel_node(*this, other), pardegree(other.pardegree), transformed(other.transformed),
        cutoff(other.cutoff), schedule(other.schedule) {}

skel_node* dc_node::clone() {
    return new dc_node(*this);
}

///////////////////////////////////////////////////////////////////////////////
// id_node implementation
///////////////////////////////////////////////////////////////////////////////
id_node::id_node( string  id, const int& index, bool all )
    : concrete_skel_node( *this ), id(std::move( id )), index(index), all(all) {}

id_node::id_node( string  id )
    : concrete_skel_node( *this ), id(std::move( id )), index(0), all(false) {}

id_node::id_node( const id_node& other )
    : concrete_skel_node( *this, other ), id( other.id ), index(other.index), all(other.all) {}

skel_node* id_node::clone() {
    return new id_node(*this);
}


///////////////////////////////////////////////////////////////////////////////
// special node implementation
///////////////////////////////////////////////////////////////////////////////
_::_()
    : concrete_skel_node( *this ) {}

_::_( const _& other )
    : concrete_skel_node( *this ) {}

skel_node* _::clone() {
    return new _(*this);
}
