#include "interpreter.hpp"
#include "rewriter.hpp"
#include <memory>
#include "utils/mytuple.hpp"

using namespace std;

single_node_cloner snc;

///////////////////////////////////////////////////////////////////////////////

void unrank2rank( skel_node& n )
{
    // 2-rank the children
    for ( size_t i = 0; i < n.size(); i++)
        unrank2rank( *n.get(i) );

    // 2-rank the current node
    while (n.size() > 2) {
        skel_node* newnode = snc(n);
        skel_node* last2 = n.pop();
        skel_node* last1 = n.pop();
        newnode->add(last1);
        newnode->add(last2);
        n.add(newnode);
    }
}

///////////////////////////////////////////////////////////////////////////////

interpreter::interpreter(rpl_environment& env, error_container& err_repo)
    : env(env), err_repo(err_repo), odispatch(env), vdispatch(env), success(true) {}

void interpreter::visit(assign_node& n) {
    n.rvalue->accept(*this);                // recurse for semantic check in skel tree
    if ( success ) {
        unrank2rank(*n.rvalue);
        env.put(n.id, n.rvalue);
    }
    else
        delete n.rvalue;
}

/* dirty implementation */
void interpreter::visit(show_node& n) {
    try {

        auto range = env.range( n.id );

        if ( n.parameters.size() > 0 )
        {
            vector<mytuple> tuples;

            // fill the tuples
            for (auto it = range.first; it != range.second; it++) {
                tuples.push_back(mytuple());
                auto& last  = tuples.back();
                auto& skptr = *it;
                for (const string& par : n.parameters) {
                    auto& shower = *vdispatch[ par ];
                    if ( par != "show_default" )
                        last.add( unique_ptr<wrapper>(new double_wrapper(shower.print(*skptr))) );
                    else
                        last.add( unique_ptr<wrapper>(new string_wrapper(shower.print(*skptr))) );
                }
            }

            // sort the tuples by calling stable_sort multiple times
            // complexity: O(N·log^2(N)) * #parameters
            size_t i = n.parameters.size()-1;
            while ( i-- > 0 )
                std::stable_sort(tuples.begin(), tuples.end(), [&i](const mytuple& t1, const mytuple& t2) {
                    return t1.compare(t2, i);
                });

            for (auto& t : tuples)
                cout << t.tostring() << endl;


        } else {

            auto& shower = *vdispatch[ n.prop ];
            for (auto it = range.first; it != range.second; it++) {
                cout << (it - range.first) << ") ";
                auto& skptr = *it;
                cout << shower.print( *skptr ) << endl;
            }

        }

    } catch (out_of_range& e) {
        err_repo.add( make_shared<error_not_exist>(n.id) );
    }
}

void interpreter::visit(set_node& n) {

}

void interpreter::visit(ann_node& n) {

}

void interpreter::visit(rwr_node& n) {
    try {

        string id = n.id;
        string rule = n.prop;
        auto p = env.range(id);
        node_set _set;
        rewriter _rewriter;
        _set = ( rule == "allrules" ) ?
            _rewriter.apply_allrules( p.first, p.second, rdispatch) :
            _rewriter.apply_rule( p.first, p.second, *rdispatch[ rule ]);

        env.clear( id );
        for ( auto& p : _set )
            env.add( id, p.second );

    } catch (out_of_range& e) {
        err_repo.add( make_shared<error_not_exist>(n.id) );
    }
}

void interpreter::visit(opt_node& n) {
    try {

        auto p = env.range( n.id );
        optrule& optrule = *odispatch[ n.prop ];
        for (auto it = p.first; it != p.second; it++) {
            auto& skptr = *it;
            optrule( *skptr );
        }

    } catch (out_of_range& e) {
        err_repo.add( make_shared<error_not_exist>(n.id) );
    }
}

void interpreter::visit(seq_node& n) {
    if (n.get(0) != nullptr)
        n.get(0)->accept(*this);
}

void interpreter::visit(comp_node& n) {
    for (size_t i = 0;  i < n.size(); i++)
        n.get(i)->accept(*this);
}

void interpreter::visit(pipe_node& n) {
    for (size_t i = 0;  i < n.size(); i++)
        n.get(i)->accept(*this);
}

void interpreter::visit(farm_node& n) {
    n.get(0)->accept(*this);
}

void interpreter::visit(map_node& n) {
    n.get(0)->accept(*this);
}

void interpreter::visit(reduce_node& n) {
    n.get(0)->accept(*this);
}

void interpreter::visit(id_node& n) {
    try {
        auto ptr = env.get(n.id);                 // check if it exists
    } catch (out_of_range& e) {
        success  = false;
        err_repo.add( make_shared<error_not_exist>(n.id) );
    }
}
