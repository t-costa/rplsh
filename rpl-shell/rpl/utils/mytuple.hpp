#ifndef rpl_utils_mytuple_hpp
#define rpl_utils_mytuple_hpp

#include <string>
#include <utility>
#include <vector>

/**
 * Wrapper for strings
 */
struct wrapper
{
    virtual ~wrapper() = default;
    [[nodiscard]] virtual bool compare( const wrapper& wr ) const = 0;
    [[nodiscard]] std::string tostring() const { return strrep; }
    virtual void print() const = 0;
protected:
    explicit wrapper(std::string  strrep) : strrep(std::move(strrep)) {}
    std::string strrep;
};

/**
 * Wrapper for a string and a value (double)
 */
struct double_wrapper : public wrapper
{
    explicit double_wrapper( const std::string& str ) : wrapper(str), value(stod(str)) {}
    [[nodiscard]] bool compare( const wrapper& wr ) const override {
        const auto& dwr = dynamic_cast<const double_wrapper&>( wr );
        return this->value < dwr.value;
    }
    void print() const override {
        std::cout << value << std::endl;
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
    [[nodiscard]] bool compare( const wrapper& wr ) const override {
        const auto& swr = dynamic_cast<const string_wrapper&>( wr );
        return this->value < swr.value;
    }
    void print() const override {
        std::cout << value << std::endl;
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
    [[nodiscard]] wrapper * get( size_t pos ) const;
    [[nodiscard]] bool compare( const mytuple& t, size_t pos ) const ;
    [[nodiscard]] std::string tostring() const;
    void print() const;
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
    if (pos >= vec.size()) {
        return nullptr;
    }
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
    if (pos >= vec.size()) {
        return false;
    }
    auto other = t.get(pos);
    if (other == nullptr) {
        return true;
    }
    return vec[pos]->compare( *other );
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

void mytuple::print() const {
    for (size_t i=0; i<vec.size(); ++i) {
        std::cout << "vec[" << i << "]: ";
        vec[i]->print();
    }
}

#endif
