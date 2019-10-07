#ifndef rpl_utils_mytuple_hpp
#define rpl_utils_mytuple_hpp

#include <string>
#include <vector>

/**
 * Wrapper for strings
 */
struct wrapper
{
    virtual ~wrapper()                              = default;
    virtual bool compare( const wrapper& wr ) const = 0;
    std::string tostring() const { return strrep; }
protected:
    wrapper(const std::string& strrep) : strrep(strrep) {}
    std::string strrep;
};

/**
 * Wrapper for a string and a value (double)
 */
struct double_wrapper : public wrapper
{
    double_wrapper( const std::string& str ) : wrapper(str), value(stod(str)) {}
    bool compare( const wrapper& wr ) const {
        const double_wrapper& dwr = dynamic_cast<const double_wrapper&>( wr );
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
    string_wrapper( const std::string& str ) : wrapper(str) , value(str) {}
    bool compare( const wrapper& wr ) const {
        const string_wrapper& swr = dynamic_cast<const string_wrapper&>( wr );
        return this->value < swr.value;
    }
private:
    std::string value;
};


/**
 * Acts as a container for pointers to wrappers
 */
struct mytuple
{
    void add( std::unique_ptr<wrapper>&& term);
    wrapper * get( const size_t pos ) const;
    bool compare( const mytuple& t, size_t pos ) const ;
    std::string tostring() const;
private:
    std::vector<std::unique_ptr<wrapper>> vec;
};

void mytuple::add( std::unique_ptr<wrapper>&& term) {
    vec.push_back( move(term) );
}

wrapper * mytuple::get(const size_t pos) const {
    return vec[pos].get();
}

bool mytuple::compare( const mytuple& t, size_t pos ) const {
    return vec[pos]->compare( *t.get(pos) );
}

std::string mytuple::tostring() const {
    std::string str = "";
    for (auto& ptr : vec )
        str = str + ptr->tostring() + "\t";
    return  str;
}

#endif
