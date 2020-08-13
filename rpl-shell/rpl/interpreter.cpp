#include "interpreter.hpp"
#include "cpp_parser/cpp_parser.hpp"
#include "rewriting/rewriter.hpp"
#include "utils/mytuple.hpp"
#include "utils/rank.hpp"
#include "utils/utils.hpp"
#include <memory>
#include <iostream>
#include <fstream>

#include "tab_completion.hpp"

// only for load verb...
#include <regex>
#include <unordered_set>
#include "parser/lexer.hpp"
#include "parser/parser.hpp"


using namespace std;

single_node_cloner snc;
void exprecurse(skel_node* n, rpl_environment& env);

///////////////////////////////////////////////////////////////////////////////

//TODO: vedere bene che sono i vari dispatcher

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

/**
 * Performs the assignment operation and adds the new node
 * in the environment
 * @param n assign node operation
 */
void interpreter::visit(assign_node& n) {

    auto* idnode = dynamic_cast<id_node*>(n.rvalue);
    success = true;
    n.rvalue->accept(*this);

    if ( success && !idnode) {
        //TODO: why there is this unrank? TC
        unranktorank2(*n.rvalue, snc);
        env.put(n.id, n.rvalue);
        //add id for tab completion
        tab_completion::add_id(n.id);
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

/**
 * Performs the show operation to the given node
 * @param n show node operation
 */
void interpreter::visit(show_node& n) {
    std::string last_index_access;
    try {

        if ( n.id.empty() && n.parameters.size() == 1) {
            last_index_access = n.parameters[0];
            //TODO: I don't get this line... TC
            cout << utils::to_string( gdispatch[n.parameters[0]]() ) << endl;
            return;
        }

        printer np;
        ann_printer ap;
        printable* p = &ap;
        unranker unrank(env);

        // builds indexes of range of elements to be displayed
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
            //change printable from ann_printer to printer
            p = &np;
        }

        // check full flag
        it = std::find(n.parameters.begin(), n.parameters.end(), par::full);
        bool full = it != n.parameters.end();
        if (full) n.parameters.erase(it);

        for (auto it_loop = begin; it_loop != end; it_loop++ ) {

            tuples.emplace_back();  //before tuples.pushback(mytuple());
            auto& last  = tuples.back();
            auto skptr = (*it_loop)->clone();
            assignres(*skptr, env.get_inputsize());

            if (unrnk) {
                unrank(*skptr);
            }

            if (full) {
                exprecurse(skptr, env);
            }

            for (const string& par : n.parameters)
                if ( par != "show_default" ) {
                    last_index_access = par;
                    printable& shower = *vdispatch[ par ];
                    last.add( unique_ptr<wrapper>(new double_wrapper( shower.print(*skptr))) );
                } else {
                    string idx  = utils::get_idx(it_loop - begin + index, end - begin + index);
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

        //TODO: where does lines_to_print come from?? TC
        if (n.lines_to_print >= 0) {
            int max = n.lines_to_print;
            for (auto it_loop = tuples.begin(); it_loop != tuples.end() && max-- > 0; it_loop++)
                cout << it_loop->tostring() << endl;
        } else {
            size_t max = -n.lines_to_print; //TODO: what??? TC
            size_t start = tuples.size() < max ? 0 : tuples.size() - max;
            for (auto it_loop = tuples.begin() + start; it_loop != tuples.end(); it_loop++)
                cout << it_loop->tostring() << endl;
        }

    } catch (out_of_range& e) {
        err_repo.add( make_shared<error_not_exist>(last_index_access) );
    } catch (std::invalid_argument& e) {
        err_repo.add( make_shared<error_not_exist>(n.id) );
    }
}

/**
 * Performs the set operation
 * @param n set node operation
 */
void interpreter::visit(set_node& n) {
    sdispatch[ n.prop ]( n.value );
}

/**
 * Performs the annotate operation
 * @param n ann node operation
 */
void interpreter::visit(ann_node& n) {
    std::size_t i = n.index < 0 ? 0 : n.index;
    auto ptr = env.get(n.id, i);
    if (ptr != nullptr) {
        bool b = (*adispatch[ n.prop ])( *ptr, n );
        cout << "response: " << (b? "annotated!" : "not annotated!") << endl;
    }
}

/**
 * Performs the rewrite operation
 * @param n rewrite node
 */
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

            if (n.index >= 0) {
                // just remove the selected version
                env.clear( n.id, n.index );
            } else {
                //remove everything
                env.clear(n.id);
            }

            for ( auto& p : _set )
                env.add( id, p.second );
        }

    } catch (invalid_argument& e) {
        err_repo.add( make_shared<error_not_exist>(n.id) );
    } catch (out_of_range& e) {
        err_repo.add( make_shared<error_not_exist>(last_rule) );
    }
}

/**
 * Performs the optimize operation
 * @param n optimize node
 */
void interpreter::visit(opt_node& n) {
    std::string last_opt;
    try {
        auto sub_it = std::find(n.parameters.begin(), n.parameters.end(), "subexp");
        bool subexp = sub_it != n.parameters.end();
        if (subexp) n.parameters.erase(sub_it);

        //for now simple fix, maxresources always at the end
        auto res_it = std::find(n.parameters.begin(), n.parameters.end(), "maxresources");
        if (res_it != n.parameters.end()) {
            n.parameters.erase(res_it);
            n.parameters.emplace_back("maxresources");
        }

        for (const string& opt : n.parameters ) {
            last_opt = opt;
            if (opt == "normalform") {
                //TODO: there should be something?? TC
                //skel_node* newsk = normform( **begin );
                //unranktorank2(*newsk, snc);
                //env.clear( n.id );
                //env.add( n.id, newsk );
            } else {
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
                for ( auto& p : _set ) {
                    env.add( n.id, p.second );
                }
            }
        }
    } catch (invalid_argument& e) {
        err_repo.add( make_shared<error_not_exist>(n.id) );
    } catch (out_of_range& e) {
        err_repo.add( make_shared<error_not_exist>(last_opt) );
    }
}

/**
 * Performs the history operation
 * @param n history node
 */
void interpreter::visit(history_node& n) {
    if (n.id.empty())
        phistory.print();
    else {
        history h(n.id, phistory);
        h.print(true);
    }
}

/**
 * Performs the import operation
 * @param n import node
 */
void interpreter::visit(import_node& n) {
    try {

        string path = utils::get_real_path(n.id);
        if (path.empty()) {
            //there is an error in the path
            err_repo.add(make_shared<error_not_exist>(n.id));
            return;
        }
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
            else if (it->wtype == wrapper_info::map) {
                //TODO: ci dovrei mettere un controllo per quando poi va a generare?
                sk = new seq_node(name, tin, tout, path);
                ((seq_node*) sk)->datap_flag = true;
            } else if (it->wtype == wrapper_info::reduce) {
                sk = new seq_node(name, tin, tout, path);
                ((seq_node*) sk)->datap_flag = true;
            } else {
                cerr << "Error: no type recognized for " << name << endl;
                sk = nullptr;   //FIXME: what it should do?? -> maybe the logic error is for this branch?
            }

            //performs the assignment to the new node
            assign_node a ( name, sk );
            visit(a);
        }
    } catch (std::logic_error&) {
        cerr << "impossible import code from " << n.id << endl;
    }
}

/**
 * Checks if an input line is empty
 * @param line string to be checked
 * @return true iff the line is empty or contains only white spaces
 */
bool lis_empty_input(string& line) {
    return line.empty() || line == string(line.length(), ' ');
}

/**
 * Checks if a line is a comment
 * @param line string to be checked
 * @return true iff the line starts with #
 */
bool lis_comment(string& line) {
    regex comment("^([ ]*)#(.*)");
    return regex_match(line, comment);
}

/**
 * Performs the load operation
 * @param n load node
 */
void interpreter::visit(load_node& n) {
    string path = utils::get_real_path(n.id);
    if (path.empty()) {
        //there is an error in the path
        err_repo.add(make_shared<error_not_exist>(n.id));
        return;
    }
    string line;
    ifstream infile(path);
    while ( getline(infile, line) ) {
        //print_rpl(); cout << line << endl;
        if (lis_empty_input(line) || lis_comment(line))
            continue;

        err_repo.reset();
        lexer _scanner(line, err_repo);
        parser _parser(_scanner, err_repo);

        auto t = _parser.parse();

        // set null
        if (!n.show)
            cout.setstate(std::ios_base::failbit);

        cout << "rplsh> " << line << endl;
        if (err_repo.size() == 0)
            t.first->accept(*this);

        // restore buffer
        if (!n.show)
            cout.clear();

        if (err_repo.size() == 0)
            get_history().add(line);
        if (err_repo.size() > 0)
            cout << err_repo.get(0);
    }
}

/**
 * Performs the generation of code
 * @param n gencode node
 */
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
    } else {
        err_repo.add(make_shared<error_not_exist>(n.id));
    }
}

/**
 * Checks the type of ptr
 * @param ptr node to be checked
 * @return true iff ptr is NOT seq, source, drain or null
 */
bool toclone(skel_node* ptr) {
    return ptr != nullptr && !dynamic_cast<seq_node*>(ptr) &&
        !dynamic_cast<source_node*>(ptr) && !dynamic_cast<drain_node*>(ptr);
}

/**
 * Expands the id_nodes that are children of n
 * @param n pointer to skeleton node
 * @param env environment
 */
void exprecurse(skel_node* n, rpl_environment& env) {
    for (size_t i = 0; i < n->size(); i++) {
        auto* k = dynamic_cast<id_node*>(n->get(i));
        if ( k ) {
            auto ptr = env.get(k->id, k->index);    // shared pointer
            if ( toclone( ptr.get() ) ) {
                n->set( ptr->clone(), i );
                exprecurse( n->get(i), env );
            }
        } else {
            exprecurse( n->get(i), env );
        }
    }
}

/**
 * Checks if it is possible to execute the expand operation
 * without problems. The variable result need to be set at true
 * on the first invocation
 * @param n node that needs to be expanded
 * @param set_id queue of pointers to skeleton nodes
 * @param env environment
 * @param result true only iff the expand operation is possible
 */
void expand_possible(skel_node* n, std::deque<skel_node*>& set_id, rpl_environment const& env, bool& result) {
    auto* k = dynamic_cast<id_node*>(n);
    if (k) {
        auto ptr = env.get(k->id, k->index);
        if (std::find(set_id.begin(), set_id.end(), ptr.get()) != set_id.end()) {
            //the node has been already seen, circular reference!
            result = false;
            return;
        }
        set_id.push_back(ptr.get());
        for (size_t i = 0; i < ptr->size(); ++i) {
            if (!result) return; //already found a problem
            expand_possible(ptr->get(i), set_id, env, result);
        }
    } else {

        if (std::find(set_id.begin(), set_id.end(), n) != set_id.end()) {
            //the node has been already seen, circular reference!
            result = false;
            return;
        }
        set_id.push_back(n);
        for (size_t i = 0; i < n->size(); ++i) {
            if (!result) return; //already found a problem
            expand_possible(n->get(i), set_id, env, result);
        }
    }

    //end of branch, remove id from the set
    if (!set_id.empty())
        set_id.pop_back();
}

/**
 * Performs the expand operation
 * @param n expand node
 */
void interpreter::visit(expand_node& n) {

    try {
        auto range = env.range( n.id );
        std::vector<skel_node*> vec;
        std::deque<skel_node*> set_id;
        for (auto it = range.first; it != range.second; it++) {
            auto& skptr = *it;
            skel_node* tmp = skptr->clone();
            bool res = true;
            expand_possible(tmp, set_id, env, res);
            if (res) {
                exprecurse(tmp, env);
                vec.push_back(tmp);
            } else {
                std::cerr << "Error: cannot expand " << n.id << "! There is a cyclic reference inside." << std::endl;
            }

            set_id.clear();
        }

        env.clear(n.prop);
        for (auto ptr : vec)
            env.add(n.prop, ptr);
    } catch (std::invalid_argument& e) {
        err_repo.add( make_shared<error_not_exist>(n.id) );
    }

}

/**
 * Performs add operation
 * @param n add node
 */
void interpreter::visit(add_node& n) {

    try {
        auto skptr = env.get(n.id, 0);
        auto range = env.range(n.prop);

        std::vector<skel_node*> vec;
        if (!skptr)
            err_repo.add(make_shared<error_not_exist>(n.id));
        else if ( dynamic_cast<source_node*>(skptr.get()) ) {
            //adds the source node
            for (auto it = range.first; it != range.second; it++)
                vec.push_back( new pipe_node( { new id_node{n.id}, (*it)->clone() } ) );
            env.clear(n.prop);
            for (auto ptr : vec)
                env.add(n.prop, ptr);
        } else if ( dynamic_cast<drain_node*>(skptr.get()) ) {
            //adds the drain node
            for (auto it = range.first; it != range.second; it++)
                vec.push_back( new pipe_node( { (*it)->clone(), new id_node{n.id} } ) );
            env.clear(n.prop);
            for (auto ptr : vec)
                env.add(n.prop, ptr);
        } else {
            std::cerr << "error: at the moment add is implemented only for adding source and drain to nodes" << std::endl;
        }

    } catch (std::invalid_argument& e) {
        err_repo.add(make_shared<error_not_exist>(n.prop));
        return;
    }
}

/**
 * Calls the accept of the child of n, if it exist
 * @param n seq node
 */
void interpreter::visit(seq_node& n) {
    if (n.get(0) != nullptr)
        n.get(0)->accept(*this);
}

/**
 * Does nothing
 * @param n source node
 */
void interpreter::visit(source_node& n) {
}

/**
 * Does nothing
 * @param n drain node
 */
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

/**
 * Calls the accept of the children of n
 * @param n comp node
 */
void interpreter::visit(comp_node& n) {
    for (size_t i = 0;  i < n.size(); i++)
        n.get(i)->accept(*this);
}

/**
 * Calls the accept of the children of n
 * @param n pipe node
 */
void interpreter::visit(pipe_node& n) {
    for (size_t i = 0;  i < n.size(); i++)
        n.get(i)->accept(*this);
}

/**
 * Calls the accept of the child of n
 * @param n farm node
 */
void interpreter::visit(farm_node& n) {
    n.get(0)->accept(*this);
}

/**
 * Calls the accept of the child of n
 * @param n map node
 */
void interpreter::visit(map_node& n) {
    n.get(0)->accept(*this);
}

/**
 * Calls the accept of the child of n
 * @param n reduce node
 */
void interpreter::visit(reduce_node& n) {
    n.get(0)->accept(*this);
}

/**
 * Checks if n exists in the environment
 * @param n
 */
void interpreter::visit(id_node& n) {
    auto ptr = env.get(n.id, n.index);                 // check if it exists
    if (ptr == nullptr) {
        success  = false;
        err_repo.add( make_shared<error_not_exist>(n.id) );
    }
}

//#endif
//////////////////// END OLD STYLE

/**
 * @return the history
 */
history& interpreter::get_history() {
    return phistory;
}
