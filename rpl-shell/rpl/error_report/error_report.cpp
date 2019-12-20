#include "error_report.hpp"
#include <string>

using namespace std;

// "pretty print" styles
const style pp_err(color::err, font::bold);
const style pp_def(color::def, font::normal);

// common print func
void print(ostream &os, const string &tok, const string &info, int pos) {
    auto offset = rplsh::name.size() + pos;
    os << pp_err << string(offset, ' ');
    os << pp_err << string( max<int>(tok.size(), 1), '^') << endl;
    os << pp_def << rplsh::info;
    os << pp_err << "error";
    os << pp_def << ": token '" << tok << "' " << info << endl;
}

///////////////////////////////////////////////////////////////////////////////
//
//  Implementation of the base class error
//
///////////////////////////////////////////////////////////////////////////////
void error::print(ostream& os) const {
    os << "error default\n";
}

ostream& operator<<(ostream& os, error const& err) {
    err.print(os);
    return os;
}

string error::line;

///////////////////////////////////////////////////////////////////////////////
//
//  Implementation of the derived class error_unexp
//
///////////////////////////////////////////////////////////////////////////////
error_unexp::error_unexp(const string& word, const int pos)
    : word(word), pos(pos) {
    }

void error_unexp::print(ostream& os) const {
    ::print(os, word, "is unexpected", pos);
}

///////////////////////////////////////////////////////////////////////////////
//
//  Implementation of the derived class error_not_exist
//
///////////////////////////////////////////////////////////////////////////////
error_not_exist::error_not_exist(const string& word)
    : word(word) {
    }

void error_not_exist::print(ostream& os) const {
    os << pp_def << rplsh::info;
    os << pp_err << "error";
    os << pp_def << ": variable '" << word
       << "' not exist in the environment" << endl;
}

///////////////////////////////////////////////////////////////////////////////
//
//  Implementation of the derived class error_illegal
//
///////////////////////////////////////////////////////////////////////////////
error_illegal::error_illegal(const string& word, int pos)
    : word(word), pos(pos) {}

void error_illegal::print(ostream& os) const {
    ::print(os, word, "is illegal", pos);
}

///////////////////////////////////////////////////////////////////////////////
//
//  Implementation of the class error_container
//
///////////////////////////////////////////////////////////////////////////////
error& error_container::get(int pos) const {
    return *errors[pos];
}

void error_container::add(shared_ptr<error> err) {
    errors.push_back(move(err));
}

void error_container::reset() {
    errors.clear();
}

int error_container::size() {
    return errors.size();
}
