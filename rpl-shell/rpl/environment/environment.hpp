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

    std::pair<it, it> range( const K& key );
    ptr<V> get(const K& key, std::size_t pos = 0) const;
    void put(const K& key, V* value);
    void add(const K& key, V* value);
    void clear( const K& key );
    void clear(const K& key, int pos);
    std::vector<K> get_keys();

private:
    std::map<K, set<V>> env;
};

/**
 * Returns a range containing all the elements with given key
 * @param key key to be searched
 * @return a pair of set iterators to the beginning and end
 * of the content of the environment at the given key, empty
 * if the key is not present
 */
template <typename K, typename V>
std::pair<set_iterator<V>, set_iterator<V>> environment<K,V>::range( const K& key )
{
    auto it = env.find(key);
//    if (it == env.end()) {
//        env[key] = {};
//    }
    //key does not exist, raise error
    if (it == env.end()) {
        throw std::invalid_argument("key does not exist");
    }
    return std::make_pair(env[key].begin(), env[key].end());
}

/**
 * Gets an element for the given key at position pos
 * @param key key to be searched
 * @param pos position in the list of values, default pos = 0
 * @return a shared pointer to the desired element,
 * nullptr if not found
 */
template <typename K, typename V>
ptr<V> environment<K,V>::get( const K& key, std::size_t pos ) const
{
    //FIXME: manca controllo su pos < max size
    auto it = env.find(key);
    try {
        if (it != env.end()) {
            return it->second.at(pos);
        } else {
            return nullptr;
        }
    } catch (std::exception& e) {
        std::cerr << "Error: index out of bound." << std::endl;
        return nullptr;
    }
    /*return it != env.end() ?
        it->second.at(pos) :
        nullptr;*/
}

/**
 * Adds <key, value> pair in the environment, if the key
 * is already present the old value is substituted by the new value
 * @param key to be inserted
 * @param value to be inserted
 */
template <typename K, typename V>
void environment<K,V>::put(const K& key, V* value)
{
    auto it = env.find(key);
    if (it != env.end())
        it->second.clear();
    add(key, value);
}

/**
 * Adds the <key, value> pair in the environment, the value is
 * inserted as a shared_pointer
 * @param key to be inserted
 * @param value to be inserted
 */
template <typename K, typename V>
void environment<K,V>::add(const K& key, V* value)
{
    auto it = env.find(key);
    if (it == env.end())
        env.insert({});
    env[key].push_back(ptr<V>(value));
}

/**
 * Clears the content of a given key, key included
 * @param key position in the env to be removed
 */
template <typename K, typename V>
void environment<K,V>::clear(const K& k)
{
    env.erase(k);
}

/**
 * Removes the element el of a given key
 * @param key position in the env
 * @param pos element in env[key] to be removed
 */
template<typename K, typename V>
void environment<K, V>::clear(const K& key, const int pos) {
    if (env.find(key) != env.end())
    {
        env[key].erase(env[key].begin() + pos);
    }
}

/**
 * @return the list of keys in the environment
 */
template<typename K, typename V>
std::vector<K> environment<K, V>::get_keys() {
    std::vector<K> keys;
    for (auto& c : env) {
        keys.emplace_back(c.first);
    }
    return keys;
}

#endif
