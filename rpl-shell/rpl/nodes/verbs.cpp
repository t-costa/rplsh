
///////////////////////////////////////////////////////////////////////////////
// Basically almost all these classes have only constructors
///////////////////////////////////////////////////////////////////////////////

#include "verbs.hpp"
#include "visitors/visitor_interface.hpp"

using namespace std;
///////////////////////////////////////////////////////////////////////////////
// verb_node implementation
///////////////////////////////////////////////////////////////////////////////
template <typename verb>
verb_node<verb>::verb_node( verb& _verb, const string& id, const string& prop )
        : _verb(_verb), id(id), index(0), prop(prop), parameters({}) {}

template <typename verb>
verb_node<verb>::verb_node( verb& _verb, const string& id, const std::vector<string>& parameters )
        : _verb(_verb), id(id), index(0), prop(""), parameters(parameters) {}

template <typename verb>
verb_node<verb>::verb_node( verb& _verb, const pair<string,int>& id, const string& prop )
        : _verb(_verb), id(id.first), index(id.second), prop(prop), parameters({}) {}

template <typename verb>
verb_node<verb>::verb_node( verb& _verb, const pair<string,int>& id, const std::vector<string>& parameters )
        : _verb(_verb), id(id.first), index(id.second), prop(""), parameters(parameters) {}

/**
 * Calls the visit operation on the passed visitor for this verb node
 * @tparam verb type of the verb node
 * @param v visitor on which execute the visit
 */
template <typename verb>
void verb_node<verb>::accept( visitor& v ) {
    v.visit( _verb );
}


///////////////////////////////////////////////////////////////////////////////
// assign_node implementation
///////////////////////////////////////////////////////////////////////////////
assign_node::assign_node(const string& id, skel_node* rvalue)
    : id(id), rvalue(rvalue) {}

void assign_node::accept(visitor &v) {
    v.visit( *this );
}


///////////////////////////////////////////////////////////////////////////////
// show_node implementation
///////////////////////////////////////////////////////////////////////////////
show_node::show_node(const string& id, const int& lines, const vector<string>& parameters)
    : verb_node(*this, id, parameters), lines_to_print(lines) {}

show_node::show_node(const pair<string,int>& id, const int& lines, const vector<string>& parameters)
    : verb_node(*this, id, parameters), lines_to_print(lines) {}


///////////////////////////////////////////////////////////////////////////////
// set_node implementation
///////////////////////////////////////////////////////////////////////////////
set_node::set_node(const string& id, const string& prop, double value)
    : verb_node(*this, id, prop), value(value) {}


///////////////////////////////////////////////////////////////////////////////
// ann_node implementation
///////////////////////////////////////////////////////////////////////////////
ann_node::ann_node(const pair<string,int>& id, const string& prop,
     double value, const string& word)
    : verb_node(*this, id, prop), value(value), word(word) {}


///////////////////////////////////////////////////////////////////////////////
// rwr_node implementation
///////////////////////////////////////////////////////////////////////////////
rwr_node::rwr_node(const string& id, const vector<string>& parameters)
    : verb_node(*this, id, parameters) {}

rwr_node::rwr_node(const pair<string,int>& id, const vector<string>& parameters)
    : verb_node(*this, id, parameters) {}


///////////////////////////////////////////////////////////////////////////////
// opt_node implementation
///////////////////////////////////////////////////////////////////////////////
opt_node::opt_node(const string& id, const vector<string>& parameters)
    : verb_node(*this, id, parameters) {}

opt_node::opt_node(const pair<string,int>& id, const vector<string>& parameters)
    : verb_node(*this, id, parameters) {}


///////////////////////////////////////////////////////////////////////////////
// history_node implementation
///////////////////////////////////////////////////////////////////////////////
history_node::history_node(const string& id)
    : verb_node(*this, id, id) {}


///////////////////////////////////////////////////////////////////////////////
// import_node implementation
///////////////////////////////////////////////////////////////////////////////
import_node::import_node(const string& id)
    : verb_node(*this, id, id) {}


///////////////////////////////////////////////////////////////////////////////
// gencode_node implementation
///////////////////////////////////////////////////////////////////////////////
gencode_node::gencode_node(const string& id, int index)
    : verb_node(*this, id, id) {
        this->index = index;
    }

void gencode_node::set_location(const std::string &t_location) {
    m_location = t_location;
}

void gencode_node::set_name(const std::string &t_name) {
    m_name = t_name;
}

std::string gencode_node::get_location() {
    return m_location;
}

std::string gencode_node::get_name() {
    return m_name;
}


///////////////////////////////////////////////////////////////////////////////
// expand_node implementation
///////////////////////////////////////////////////////////////////////////////
expand_node::expand_node(const string& idA, const string& idB)
    : verb_node(*this, idA, idB) {}


///////////////////////////////////////////////////////////////////////////////
// add_node implementation
///////////////////////////////////////////////////////////////////////////////
add_node::add_node(const string& idA, const string& idB)
    : verb_node(*this, idA, idB) {}


///////////////////////////////////////////////////////////////////////////////
// load_node implementation
///////////////////////////////////////////////////////////////////////////////
load_node::load_node(const string& id, bool showoutput)
    : verb_node(*this, id, id), show(showoutput) {}