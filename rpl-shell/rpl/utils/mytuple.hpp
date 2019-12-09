#ifndef rpl_utils_mytuple_hpp
#define rpl_utils_mytuple_hpp

#include <string>
#include <vector>

/**
 * Wrapper for strings
 */
struct wrapper
{
    virtual ~wrapper() = default;
    virtual bool compare( const wrapper& wr ) const = 0;
    std::string tostring() const { return strrep; }
protected:
    explicit wrapper(const std::string& strrep) : strrep(strrep) {}
    std::string strrep;
};

/**
 * Wrapper for a string and a value (double)
 */
struct double_wrapper : public wrapper
{
    explicit double_wrapper( const std::string& str ) : wrapper(str), value(stod(str)) {}
    bool compare( const wrapper& wr ) const override {
        const auto& dwr = dynamic_cast<const double_wrapper&>( wr );
        return this->value < dwr.value;
    }
private:
    double value;
};

/**
 * Wrapper for a string and a value (string)
 */
struct string_wrapper : public wrapper
{
    explicit string_wrapper( const std::string& str ) : wrapper(str) , value(str) {}
    bool compare( const wrapper& wr ) const override {
        const auto& swr = dynamic_cast<const string_wrapper&>( wr );
        return this->value < swr.value;
    }
private:
    std::string value;
};


/**
 * Acts as a container for unique pointers to wrappers
 */
struct mytuple
{
    void add( std::unique_ptr<wrapper>&& term);
    wrapper * get( size_t pos ) const;
    bool compare( const mytuple& t, size_t pos ) const ;
    std::string tostring() const;
private:
    std::vector<std::unique_ptr<wrapper>> vec;
};

/**
 * Moves term to the container
 * @param term pointer to wrapper to be added
 */
void mytuple::add( std::unique_ptr<wrapper>&& term) {
    vec.push_back( move(term) );
}

/**
 * @param pos index inside the container
 * @return the element in position pos
 */
wrapper * mytuple::get(const size_t pos) const {
    return vec[pos].get();
}

/**
 * Calls the compare between the element in position pos
 * in this container and the element in t in position pos
 * @param t second container
 * @param pos position of the two elements to be compared
 * @return true iff the element in this is smaller
 */
bool mytuple::compare( const mytuple& t, size_t pos ) const {
    return vec[pos]->compare( *t.get(pos) );
}

/**
 * @return a string representation of the content of this
 */
std::string mytuple::tostring() const {
    std::string str;
    for (auto& ptr : vec ) {
        str += ptr->tostring() + "\t";
    }
    return  str;
}

#endif
