#ifndef tree_matcher_hpp
#define tree_matcher_hpp

#include <vector>

/**
 * Implements subtree pattern matching
 * @tparam T type of the tree nodes, it must have the same
 * iterable interface exposed for a skel_node
 * @tparam S special type for a node, it indicates that a node
 * with this type matches with the entire rooted subtree
 */
template <typename T, typename S>
struct tree_matcher
{
    bool match ( T* root, T *rule);
protected:
    bool match ( T* root, T* rule, bool reset );
    std::vector<T*> leaves;
};

/**
 * If the rule is the same type of S (matches with the entire rooted subtree)
 * root is inserted as leaf, recursively for root and rule; it stops as soon
 * as root is inserted or no insertion is possible
 * @tparam T type of the tree nodes
 * @tparam S special type for a node
 * @param root first tree
 * @param rule second tree
 * @return true if all root is inserted? Sure if some subtree is not inserted
 * it returns false, even if some others have been inserted!
 */
template <typename T, typename S>
bool tree_matcher<T,S>::match(T* root, T* rule)
{
    return match(root, rule, true);
}

/**
 * If the rule is the same type of S (matches with the entire rooted subtree)
 * root is inserted as leaf, recursively for root and rule; it stops as soon
 * as root is inserted or no insertion is possible
 * @tparam T type of the tree nodes
 * @tparam S special type for a node
 * @param root first tree
 * @param rule second tree
 * @param reset useful just for the recursion
 * @return true if all root is inserted? Sure if some subtree is not inserted
 * it returns false, even if some others have been inserted!
 */
template <typename T, typename S>
bool tree_matcher<T,S>::match(T* root, T* rule, bool reset)
{
    if (reset)
        leaves.clear();

    if ( dynamic_cast<S*>(rule) ) {
        leaves.push_back(root);
        return true;
    }

    std::size_t sizel = root->size();
    std::size_t sizer = rule->size();
    bool res = *root == *rule && sizel == sizer;
    //as soon as res=false the for loop stops
    for (std::size_t i = 0; res && i < sizel; i++) {
        T* n1 = root->get(i);
        T* n2 = rule->get(i);
        res = res && match(n1, n2, false);
    }
    return res;
}

#endif
