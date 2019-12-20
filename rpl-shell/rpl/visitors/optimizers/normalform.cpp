#include "normalform.hpp"

normalform::normalform( rpl_environment& env ) :
    env( env ),
    success(true)
{}

/**
 * Adds a clone of n in leaves
 * @param n seq node
 */
void normalform::visit( seq_node& n ) {
    leaves.push_back( n.clone() );
}

/**
 * Calls the visit for each child
 * @param n comp node
 */
void normalform::visit( comp_node& n ) {
    for (size_t i = 0; i < n.size(); i++)
        n.get(i)->accept(*this);
}

/**
 * Calls the visit for each child
 * @param n pipe node
 */
void normalform::visit( pipe_node& n ) {
    for (size_t i = 0; i < n.size(); i++)
        n.get(i)->accept(*this);
}

/**
 * Calls the visit for the child
 * @param n farm node
 */
void normalform::visit( farm_node& n ) {
    n.get(0)->accept(*this);
}

/**
 * Sets success to false
 * @param n map node
 */
void normalform::visit( map_node& n ) {
    success = false;
}

/**
 * Sets success to false
 * @param n reduce node
 */
void normalform::visit( reduce_node& n ) {
    success = false;
}

/**
 * Get range of n and checks what to do based on the type of n
 * @param n id node
 */
void normalform::visit( id_node& n ) {
    try {
        auto p = env.range( n.id );
        skel_node& f = **p.first;
        //if more than one element in range I guess...
        if ( p.second - p.first != 1 )
            success = false;
        else if ( ! dynamic_cast<seq_node*>( &f ) )
            f.accept(*this);
        else
            leaves.push_back( f.clone() );
    } catch (std::invalid_argument& e) {
        std::cerr << "Error! " << n.id << " not found in the environment! (normalform)" << std::endl;
    }
}

/**
 * Starts the visit; if the visit is successful, builds an
 * equivalent skeleton tree in normal form
 * @param n skeleton root
 * @return a pointer to the normal form if successful, nullptr otherwise
 */
skel_node* normalform::operator()( skel_node& n ) {
    success = true;
    n.accept(*this);

    if ( !success ) {
        for (auto l : leaves) delete l;
        leaves.clear();
        return nullptr;
    }

    auto* c = new comp_node({});
    for (auto l : leaves)
        c->add(l);

    return new farm_node({c});
}
