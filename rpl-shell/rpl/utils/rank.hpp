#ifndef rpl_utils_rank_hpp
#define rpl_utils_rank_hpp

//TODO: non capisco niente...

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
