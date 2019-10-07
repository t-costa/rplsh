#ifndef rpl_environment_hpp
#define rpl_environment_hpp

#include <map>
#include <vector>
#include <utility>
#include <memory>
#include <iostream>

// useful typedefs
template <typename T>
using ptr = std::shared_ptr<T>;

template <typename T>
using set = std::vector<ptr<T>>;

template <typename T>
using set_iterator = typename set<T>::iterator;

/**
 * Contains the table with the bindings key -> { value1, value2, ... }
 * The environment is implemented internally as a map of sets
 * @tparam K type of the key
 * @tparam V type of the values
 */
template <typename K, typename V>
struct environment
{
    typedef set_iterator<V> it;

    /// returns a range containing all the elements with given key
    /**
     * Returns a range containing all the elements with given key
     * @param key key to be searched
     * @return a pair of set iterators to the beginning and end
     * of the content of the environment at the given key, empty
     * if the key is not present
     */
    std::pair<it, it> range( const K& key );

    /**
     * Gets an element for the given key at position pos
     * @param key key to be searched
     * @param pos position in the list of values, default pos = 0
     * @return a shared pointer to the desired element,
     * nullptr if not found
     */
    ptr<V> get(const K& key, std::size_t pos = 0);

    /**
     * Adds <key, value> pair in the environment, if the key
     * is already present the old value is substituted by the new value
     * @param key to be inserted
     * @param value to be inserted
     */
    void put(const K& key, V* value);

    /**
     * Adds the <key, value> pair in the environment, the value is
     * inserted as a shared_pointer
     * @param key to be inserted
     * @param value to be inserted
     */
    void add(const K& key, V* value);

    /**
     * Modifies the content in position pos
     * @param pos position to be modified
     * @param value new value
     */
    //void modify(it pos, V* value);

    /**
     * Clears the content of a given key, key included
     * @param key position in the env to be removed
     */
    void clear( const K& key );

    /**
     * Removes the element el of a given key
     * @param key position in the env
     * @param pos element in env[key] to be removed
     */
    void clear(const K& key, const int pos);

private:
    std::map<K, set<V>> env;
};

template <typename K, typename V>
std::pair<set_iterator<V>, set_iterator<V>> environment<K,V>::range( const K& key )
{
    auto it = env.find(key);
    //TODO: se la chiave non esiste perchè dovrebbe crearne una fittizia? non dovrebbe essere errore?
//    if (it == env.end()) {
//        env[key] = {};
//    }
    if (it == env.end()) {
        throw std::invalid_argument("key does not exist");
    }
    return std::make_pair(env[key].begin(), env[key].end());
}

template <typename K, typename V>
ptr<V> environment<K,V>::get( const K& key, std::size_t pos )
{
    auto it = env.find(key);
    return it != env.end() ?
        it->second.at(pos) :
        nullptr;
}

template <typename K, typename V>
void environment<K,V>::put(const K& key, V* value)
{
    auto it = env.find(key);
    if (it != env.end())
        it->second.clear();
    add(key, value);
}

template <typename K, typename V>
void environment<K,V>::add(const K& key, V* value)
{
    auto it = env.find(key);
    if (it == env.end())
        env.insert({});
    env[key].push_back(ptr<V>(value));
}

template <typename K, typename V>
void environment<K,V>::clear(const K& k)
{
    env.erase(k);
}

template<typename K, typename V>
void environment<K, V>::clear(const K& key, const int pos) {
    if (env.find(key) != env.end())
    {
        std::cout << "sto eliminando la cosa singola" << std::endl;
        env[key].erase(env[key].begin() + pos);
    }
}

#endif
