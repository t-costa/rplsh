#ifndef rpl_utils_rank_hpp
#define rpl_utils_rank_hpp

//TODO: non capisco niente...

/**
 * Flattens the skeleton tree, merging together nodes of the
 * same type when possible. The number of children of the nodes
 * might grow.
 * @tparam Node type of the skeleton node
 * @param curr_node node that is currently visited
 * @param root_node root of the current subtree
 */
template <typename Node>
void ranktounrank(Node* curr_node, Node* root_node)
{
    // recurse if the two nodes have the same type
    if ( *curr_node == *root_node && curr_node->size() > 1 )
    {
        std::vector<Node*> vec;
        while (curr_node->size() != 0)
            vec.push_back(curr_node->pop());

        for ( auto rit = vec.rbegin(); rit != vec.rend(); ++rit )
            ranktounrank( *rit, root_node );

        if (curr_node != root_node)
            delete curr_node;
    }
    else if ( *curr_node != *root_node)
        root_node->add(curr_node);
}

/**
 * Called after a ranktounrank, transforms the skeleton tree
 * back to its original shape (at most two children per node)
 * @tparam Node type of the nodes
 * @tparam NewNodeFunctor type of the function used to clone the nodes
 * @param n starting node
 * @param snc function used to clone the nodes
 */
template <typename Node, typename NewNodeFunctor>
void unranktorank2( Node& n, NewNodeFunctor& snc )
{
    // rank the children first
    for ( size_t i = 0; i < n.size(); i++)
        unranktorank2( *n.get(i), snc );

    // rank the current node
    while (n.size() > 2)
    {
        Node* newnode = snc(n);
        Node* last2 = n.pop();
        Node* last1 = n.pop();
        newnode->add(last1);
        newnode->add(last2);
        n.add(newnode);
    }
}

#endif
