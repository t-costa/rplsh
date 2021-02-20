#ifndef rplsh_types_hpp
#define rplsh_types_hpp

#include <vector>
#include <cstdlib>
#include <memory>
#include <iostream>
#include <algorithm>
#include "wrappers.hpp"

class split {
};

// example of composable subclassing
template <typename Value>
class range : public composable<Value> {
public:
    Value lower;
    Value upper;
    std::size_t grain;

    // constructors
    range();
    range( Value begin, Value end, std::size_t grain = 1 );

    // composable implementation
    bool is_empty() const;
    std::size_t size() const;
    bool is_splittable() const;
};

// constructors
template <typename Value>
range<Value>::range() :
    range(0,0)
{}

template <typename Value>
range<Value>::range( Value begin, Value end, std::size_t grain ) :
    lower(begin),
    upper(end),
    grain(grain)
{}

template <typename Value>
bool range<Value>::is_empty() const {
    return lower == upper;
}

template <typename Value>
std::size_t range<Value>::size() const {
    return upper - lower;
}

template <typename Value>
bool range<Value>::is_splittable() const {
    return lower+grain < upper;
}

///////////////////// Container class /////////////////////
template <typename T>
class Container : public range<std::size_t> {
public:
    Container();                                            // empty constructor
    Container( std::initializer_list<T>& init );            // init constructor
    Container( Container<T>& container, const split& s );   // split constructor

    // interface iterating the container
    void add ( const T& t );
    void push_back ( const T& t );
    void resize( std::size_t value );
    T get ( const std::size_t& idx ) const;
    T& operator[]( const std::size_t& idx ) const;
    T& operator[]( const std::size_t& idx );
    std::size_t size() const;

protected:
    std::shared_ptr<std::vector<T>> vec;
};

template <typename T>
Container<T>::Container() :
    range(0, 0),
    vec ( std::make_shared<std::vector<T>>() )
{}

template <typename T>
Container<T>::Container( std::initializer_list<T>& init ) :
    vec ( std::make_shared<std::vector<T>>(init) ),
    range(0, init.size())
{}

template <typename T>
Container<T>::Container( Container<T>& o, const split& sp ) {
    int m   = (o.lower + o.upper)/2;
    lower   = o.lower + m;
    upper   = o.upper;
    vec     = o.vec;
    o.upper = lower;
}

template <typename T>
void Container<T>::add( const T& t ) {
    vec->push_back(t);
    upper = vec->size();
}

template <typename T>
void Container<T>::push_back( const T& t ) {
    vec->push_back(t);
    upper = vec->size();
}

template <typename T>
void Container<T>::resize( std::size_t value ) {
    vec->reserve(value);
    vec->resize(value);
}

template <typename T>
T Container<T>::get( const std::size_t& idx ) const {
    return (*vec)[idx];
}

template <typename T>
T& Container<T>::operator[]( const std::size_t& idx ) const {
    return (*vec)[idx];
}

template <typename T>
T& Container<T>::operator[]( const std::size_t& idx ) {
    return (*vec)[idx];
}

template <typename T>
std::size_t Container<T>::size() const {
    return vec->size();
}

#endif
