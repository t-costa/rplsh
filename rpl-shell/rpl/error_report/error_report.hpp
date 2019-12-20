///////////////////////////////////////////////////////////////////////////////
//
//  Simple classes using for error reporting
//
///////////////////////////////////////////////////////////////////////////////
#ifndef rpl_error_report_hpp
#define rpl_error_report_hpp

#include "style.hpp"
#include <algorithm>
#include <ostream>
#include <memory>
#include <vector>

/**
 * Simple class used for error reporting
 */
struct error
{
    friend ostream& operator<<(ostream& os, error const& err);
    virtual void print(ostream& os) const;
    static string line;
};

/**
 * Class to represent unexpected errors
 */
struct error_unexp : public error
{
    error_unexp(const string& word, int pos);
    void print(ostream& os) const override;
    const string word;
    const int pos;
};

/**
 * Class to represent illegal errors
 */
struct error_illegal : public error
{
    error_illegal(const string& word, int pos);
    void print(ostream& os) const override;
    const string word;
    const int pos;
};

/**
 * Class to represent non existing elements errors
 */
struct error_not_exist : public error
{
    explicit error_not_exist(const string& word);
    void print(ostream& os) const override;
    const string word;
};

/**
 * Class that contains a list of shared_pointers to
 * all the possible errors
 */
struct error_container
{
    void add(shared_ptr<error> err);
    error& get(int pos) const;
    void reset();
    int size();
private:
    std::vector<std::shared_ptr<error>> errors;
};

#endif
