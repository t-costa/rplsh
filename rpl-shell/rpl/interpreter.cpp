#include "interpreter.hpp"
#include "cpp_parser/cpp_parser.hpp"
#include "rewriting/rewriter.hpp"
#include "utils/mytuple.hpp"
#include "utils/rank.hpp"
#include "utils/utils.hpp"
#include <memory>
#include <iostream>
#include <fstream>

// only for load verb...
#include <regex>
#include "parser/lexer.hpp"
#include "parser/parser.hpp"


using namespace std;

single_node_cloner snc;
void exprecurse(skel_node* n, rpl_environment& env);

///////////////////////////////////////////////////////////////////////////////

interpreter::interpreter(rpl_environment& env, error_container& err_repo) :
    env(env),
    err_repo(err_repo),
    adispatch(env),
    sdispatch(env),
    gdispatch(env),
    odispatch(env),
    vdispatch(env),
    normform(env),
    ff(env),
    success(true)
{}

void interpreter::visit(assign_node& n) {

    id_node* idnode = dynamic_cast<id_node*>(n.rvalue);
    success = true;
    n.rvalue->accept(*this);

    if ( success && !idnode) {
        unranktorank2(*n.rvalue, snc);
        env.put(n.id, n.rvalue);
    } else if ( success ) {
        env.clear(n.id);
        if (idnode->all) {
            auto range = env.range(idnode->id);
            for (auto it = range.first; it != range.second; it++)
                env.add(n.id, (*it)->clone());
        } else {
            auto skel = env.get(idnode->id, idnode->index);
            if (skel)
                env.add(n.id, skel->clone());
        }
        delete n.rvalue;
    } else
        delete n.rvalue;
}

void interpreter::visit(show_node& n) {
    try {

        if ( n.id == "" && n.parameters.size() == 1) {
            cout << utils::to_string( gdispatch[n.parameters[0]]() ) << endl;
            return;
        }

        printer np;
        ann_printer ap;
        printable* p = &ap;
        unranker unrank(env);

        vector<mytuple> tuples;
        auto range = env.range( n.id );
        auto index = n.index < 0 ? 0 : n.index;
        auto begin = range.first + ( n.index < 0 ? 0 : n.index );
        auto end   = n.index < 0 ? range.second : range.first + n.index + 1;

        // check unranked flag
        auto it = std::find(n.parameters.begin(), n.parameters.end(), par::unranked);
        bool unrnk = it != n.parameters.end();
        if (unrnk) n.parameters.erase(it);

        // check noann flag
        it = std::find(n.parameters.begin(), n.parameters.end(), par::noann);
        bool noann = it != n.parameters.end();
        if (noann) {
            n.parameters.erase(it);
            p = &np;
        }

        // check full flag
        it = std::find(n.parameters.begin(), n.parameters.end(), par::full);
        bool full = it != n.parameters.end();
        if (full) n.parameters.erase(it);

        for ( auto it = begin; it != end; it++ ) {

            tuples.push_back(mytuple());
            auto& last  = tuples.back();
            auto skptr = (*it)->clone();
            assignres(*skptr, env.get_inputsize());

            if (unrnk) {
                unrank(*skptr);
            }

            if (full) {
                exprecurse(skptr, env);
            }

            for (const string& par : n.parameters)
                if ( par != "show_default" ) {
                    printable& shower = *vdispatch[ par ];
                    last.add( unique_ptr<wrapper>(new double_wrapper( shower.print(*skptr))) );
                } else {
                    string idx  = utils::get_idx(it - begin + index, end - begin + index);
                    last.add( unique_ptr<wrapper>(new string_wrapper( idx + " : " + p->print(*skptr))) );
                }

            delete skptr;

        }

        // sort the tuples by calling stable_sort multiple times
        // complexity: O(N·log^2(N)) * #parameters
        size_t i = n.parameters.size()-1;
        while ( i-- > 0 )
            std::stable_sort(tuples.begin(), tuples.end(), [&i](const mytuple& t1, const mytuple& t2) {
                return t1.compare(t2, i);
            });

        if (n.lines_to_print >= 0) {
            int max = n.lines_to_print;
            for (auto it = tuples.begin(); it != tuples.end() && max-- > 0; it++)
                cout << it->tostring() << endl;
        } else {
            size_t max = -n.lines_to_print;
            size_t start = tuples.size() < max ? 0 : tuples.size() - max;
            for (auto it = tuples.begin() + start; it != tuples.end(); it++)
                cout << it->tostring() << endl;
        }

    } catch (out_of_range& e) {
        err_repo.add( make_shared<error_not_exist>(n.id) );
    }
}

void interpreter::visit(set_node& n) {
    sdispatch[ n.prop ]( n.value );
}

void interpreter::visit(ann_node& n) {
    std::size_t i = n.index < 0 ? 0 : n.index;
    auto ptr = env.get(n.id, i);
    if (ptr != nullptr) {
        bool b = (*adispatch[ n.prop ])( *ptr, n );
        cout << "response: " << (b? "annotated!" : "not annotated!") << endl;
    }
}

void interpreter::visit(rwr_node& n) {
    string last_rule;
    try {
        string id = n.id;

        /* rec support */
        auto it = std::find(n.parameters.begin(), n.parameters.end(), "rec");
        bool rec = it != n.parameters.end();
        if (rec) n.parameters.erase(it);

        for (const string& rule : n.parameters ) {
            last_rule = rule;
            node_set _set;
            rewriter _rewriter(rec);
            auto range = env.range( n.id );
            auto begin = range.first + ( n.index < 0 ? 0 : n.index );
            auto end   = n.index < 0 ? range.second : range.first + n.index + 1;

            _set = ( rule == "allrules" ) ?
                _rewriter.apply_allrules( begin, end, rdispatch) :
                _rewriter.apply_rule( begin, end, *rdispatch[ rule ]);

            env.clear( id );
            for ( auto& p : _set )
                env.add( id, p.second );
        }

    } catch (invalid_argument& e) {
        err_repo.add( make_shared<error_not_exist>(n.id) );
    } catch (out_of_range& e) {
        err_repo.add( make_shared<error_not_exist>(last_rule) );
    }
}

void interpreter::visit(opt_node& n) {
    try {
        //TODO: prova a mettere il vero tipo di it invece di auto
        auto sub_it = std::find(n.parameters.begin(), n.parameters.end(), "subexp");
        bool subexp = sub_it != n.parameters.end();
        if (subexp) n.parameters.erase(sub_it);

        for (const string& opt : n.parameters ) {
            if (opt == "normalform") {
                //skel_node* newsk = normform( **begin );
                //unranktorank2(*newsk, snc);
                //env.clear( n.id );
                //env.add( n.id, newsk );
            } else {
                //TODO: probabilmente anche qui serve il controllo se n.id esiste ed identificare la regola che non quadra
                auto range = env.range( n.id );
                auto begin = range.first + ( n.index <= 0 ? 0 : n.index );
                auto end   = n.index <= 0 ? range.second : range.first + n.index + 1;

                node_set _set;
                printer print;

                optrule& optrule = *odispatch[ opt ];
                optrule.subexp = subexp;                    // if true, the optrule will be
                                                            // applied recursively in the subexpr
                for (auto it = begin; it != end; it++) {
                    auto& skptr = *it;
                    //cout << (it-begin) << ": " << print(*skptr) << endl;

                    assignres( *skptr, env.get_inputsize() );
                    optrule( *skptr );
                    _set.insert({print(*skptr), skptr->clone()});
                }

                if (n.index >= 0) {
                    // just remove the selected version
                    env.clear( n.id, n.index );
                } else {
                    //remove everything
                    env.clear(n.id);
                }
                for ( auto& p : _set )
                    env.add( n.id, p.second );
            }
        }
    } catch (out_of_range& e) {
        err_repo.add( make_shared<error_not_exist>(n.id) );
    }
}

void interpreter::visit(history_node& n) {
    if (n.id == "")
        phistory.print();
    else {
        history h(n.id, phistory);
        h.print(true);
    }
}

void interpreter::visit(import_node& n) {
    try {

        string path = utils::get_real_path(n.id);
        cout << "importing from: " << path << endl;

        cpp_parser cp(path);
        auto p = cp.parse();

        for (auto it =  p.first; it != p.second; it++) {

            cout << "importing " << it->name << endl;

            skel_node* sk;
            string name = it->name;
            string tin  = it->typein;
            string tout = it->typeout;

            if (it->wtype == wrapper_info::seq)
                sk = new seq_node(name, tin, tout, path);
            else if (it->wtype == wrapper_info::source)
                sk = new source_node(name,tout, path);
            else if (it->wtype == wrapper_info::drain)
                sk = new drain_node(name,tin, path);
            else
                cout << "error: no type recognized" << endl;

            assign_node a ( name, sk );
            visit(a);

        }
    } catch (std::logic_error) {
        cout << "impossible import code from " << n.id << endl;
    }
}

/* TODO it's not very good, rewrite all */
bool lis_empty_input(string& line) {
    return line.empty() || line == string(line.length(), ' ');
}

bool lis_comment(string& line) {
    regex comment("^([ ]*)#(.*)");
    return regex_match(line, comment);
}

void interpreter::visit(load_node& n) {
    // TODO fast implementation of load verb, better rewrite please
    string path = utils::get_real_path(n.id);
    string line;
    ifstream infile(path);
    while ( getline(infile, line) ) {
        //print_rpl(); cout << line << endl;
        if (lis_empty_input(line) || lis_comment(line))
            continue;

        err_repo.reset();
        lexer _scanner(line, err_repo);
        parser _parser(_scanner, err_repo);

        unique_ptr<rpl_node> t = _parser.parse();

        // set null
        if (!n.show)
            cout.setstate(std::ios_base::failbit);

        cout << "rplsh> " << line << endl;
        if (err_repo.size() == 0)
            t->accept(*this);

        // restore buffer
        if (!n.show)
            cout.clear();

        if (err_repo.size() == 0)
            get_history().add(line);
        if (err_repo.size() > 0)
            cout << err_repo.get(0);
    }
}

void interpreter::visit(gencode_node& n) {
    int i = 0;
    string code;
    string fname;
    printer print;
    ranker rank(env);
    unranker unrank(env);

    size_t pos = n.index < 0 ? 0 : n.index;
    auto ptr = env.get(n.id, pos);

    if (ptr != nullptr) {
        // unrank and generate code
        unrank(*ptr);
        code = ff(*ptr);

        // find name and store
        while ( utils::file_exists("ff"+to_string(++i)+".cpp") );
        fname = "ff" + to_string(i) + ".cpp";
        cout << "-- " << fname << endl;
        std::ofstream out(fname);
        out << code;

        // rerank
        rank(*ptr);
    }
}

bool toclone(skel_node* ptr) {
    return ptr != nullptr && !dynamic_cast<seq_node*>(ptr) &&
        !dynamic_cast<source_node*>(ptr) && !dynamic_cast<drain_node*>(ptr);
}

void exprecurse(skel_node* n, rpl_environment& env) {
    for (size_t i = 0; i < n->size(); i++) {
        id_node* k = dynamic_cast<id_node*>(n->get(i));
        if ( k ) {
            auto ptr = env.get(k->id, k->index);    // shared pointer
            if ( toclone( ptr.get() ) ) {
                n->set( ptr->clone(), i );
                exprecurse( n->get(i), env );
            }
        } else
            exprecurse( n->get(i), env );
    }
}

void interpreter::visit(expand_node& n) {

    auto range = env.range( n.id );
    std::vector<skel_node*> vec;
    for (auto it = range.first; it != range.second; it++) {
        auto& skptr = *it;
        skel_node* tmp = skptr->clone();
        exprecurse(tmp, env);
        vec.push_back(tmp);
    }

    env.clear(n.prop);
    for (auto ptr : vec)
        env.add(n.prop, ptr);
}

void interpreter::visit(add_node& n) {
    auto skptr = env.get(n.id, 0);
    auto range = env.range(n.prop);
    std::vector<skel_node*> vec;
    if (!skptr) //TODO better error handling
        std::cout << "error: " << n.id << " does not exist" << std::endl;
    else if ( dynamic_cast<source_node*>(skptr.get()) ) {
        for (auto it = range.first; it != range.second; it++)
            vec.push_back( new pipe_node( { new id_node{n.id}, (*it)->clone() } ) );
        env.clear(n.prop);
        for (auto ptr : vec)
            env.add(n.prop, ptr);
    } else if ( dynamic_cast<drain_node*>(skptr.get()) ) {
        for (auto it = range.first; it != range.second; it++)
            vec.push_back( new pipe_node( { (*it)->clone(), new id_node{n.id} } ) );
        env.clear(n.prop);
        for (auto ptr : vec)
            env.add(n.prop, ptr);
    } else {
        std::cout << "error: at the moment add is implemented only for adding source and drain to nodes" << std::endl;
    }
}



void interpreter::visit(seq_node& n) {
    if (n.get(0) != nullptr)
        n.get(0)->accept(*this);
}

void interpreter::visit(source_node& n) {
}

void interpreter::visit(drain_node& n) {
}

///////////////////// BEGIN EXPANSION
/*#ifdef EXPANSION

bool toclone;

void recurse(skel_node& n, interpreter& interpr, rpl_environment& env) {
    for (size_t i = 0;  i < n.size(); i++) {
        toclone = false;
        n.get(i)->accept(interpr);
        if (toclone) {
            id_node* k = static_cast<id_node*>(n.get(i));
            auto ptr = env.get(k->id, k->index);
            n.set(ptr->clone(), i);
            toclone = false;
            //TODO delete the id_node
            //delete k;
        }
    }
}

void interpreter::visit(comp_node& n) {
    recurse(n, *this, env);
}

void interpreter::visit(pipe_node& n) {
    recurse(n, *this, env);
}

void interpreter::visit(farm_node& n) {
    recurse(n, *this, env);
}

void interpreter::visit(map_node& n) {
    recurse(n, *this, env);
}

void interpreter::visit(reduce_node& n) {
    recurse(n, *this, env);
}

void interpreter::visit(id_node& n) {
    auto ptr = env.get(n.id, n.index);                 // check if it exists
    toclone  = ptr != nullptr && !dynamic_cast<seq_node*>(ptr.get()) &&
        !dynamic_cast<source_node*>(ptr.get()) && !dynamic_cast<drain_node*>(ptr.get());

    if (ptr == nullptr) {
        success  = false;
        err_repo.add( make_shared<error_not_exist>(n.id) );
    }
}
*/
////////////////////////// END EXPANSION

///////////////////////// BEGIN OLD-STYLE
//#else

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
    auto ptr = env.get(n.id, n.index);                 // check if it exists
    if (ptr == nullptr) {
        success  = false;
        err_repo.add( make_shared<error_not_exist>(n.id) );
    }
}

//#endif
//////////////////// END OLD STYLE

history& interpreter::get_history() {
    return phistory;
}
