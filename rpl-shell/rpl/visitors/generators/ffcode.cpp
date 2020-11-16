#include "ffcode.hpp"
#include "ffmapper.hpp"
#include <iostream>
#include <string>
#include <map>
#include <set>
#include <regex>
#include <cassert>

using namespace std;

map<string, int> names;
map<string, bool> business_headers;

/**
 * Generates a string with the passed name and an increasing value
 * @param name of the variable
 * @return new name for the variable of the form nameNumber
 */
string new_name(const string& name) {
    int num = 0;
    string apnd;

    try {
        num = ++names.at(name);
    } catch (out_of_range& e) {
        names[name] = 0;
    }

    apnd = !num ? "" : to_string(num);
    return name + apnd;
}

/**
 * Insert a debug print that determines the CPU on which
 * the calling thread is running
 * @param where1
 * @param where2
 * @param tabs
 * @return working code as a string
 */
string dtrace_core( const string& where1, const string& where2, const string& tabs ) {
    stringstream ss;
    ss << tabs << "#ifdef TRACE_CORE\n";
    ss << tabs << "std::cout << \"" << where1 << " -- " << where2 << " -- id = \"";
    ss << tabs << "<< get_my_id() << \" -- tid = \" << std::this_thread::get_id() << \" -- core = \" << sched_getcpu() << std::endl;\n";
    ss << tabs << "#endif\n";
    return ss.str();
}

/**
 * Inserts svc_init stage with debug print
 * @param where
 * @return working code as a string
 */
string svc_init_decl(const string& where) {
    stringstream ss;
    ss << "\tint svc_init() {\n";
    ss << dtrace_core("svc_init", where, "\t\t");
    ss << "\t\treturn 0;\n";
    ss << "\t}\n";
    return ss.str();
}

/**
 * Creates the ff_node calling the function.
 * Typein and typeout must implement copy assignment
 * operator and default constructor
 * @param n node to be converted in code
 * @return working code as string
 */
string stage_declaration( const seq_node& n ) {
    stringstream ss;
    ss << "class " << n.name << "_stage : public ff_node {\n";
    ss << "protected:\n\t" << n.name << " wrapper; \n";
    ss << "public:\n";
    ss << svc_init_decl(n.name) << "\n";
    ss << "\tvoid * svc(void *t) {\n";
    ss << "\t\t" << n.typein << " _in  = *((" << n.typein << "*) t);\n";
    ss << "\t\t" << n.typeout << "* _out  = new " << n.typeout << "();\n";
    ss << "\t\t" << "*_out = wrapper.compute(_in);\n";
    ss << "\t\t" << "delete ((" << n.typein << "*) t);\n";
    ss << "\t\t" << "return (void*) _out;\n";
    ss << "\t}\n";
    ss << "};\n\n";
    return ss.str();
}

/**
 * Creates the ff_node for the source node
 * @param n node to be converted in code
 * @return working code as string
 */
string source_declaration( const source_node& n ) {
    stringstream ss;
    ss << "class " << n.name << "_stage : public ff_node {\n";
    ss << "protected:\n\tstd::unique_ptr<" << n.name << "> src; \n\n";
    ss << "public:\n";
    ss << "\t" << n.name << "_stage() : src(new " << n.name << "()) {}\n";
    ss << svc_init_decl(n.name) << "\n";
    ss << "\tvoid * svc(void *t) {\n";
    ss << "\t\twhile( src->has_next() )\n";
    ss << "\t\t\tff_send_out((void*) src->next() );\n";
    ss << "\t\treturn (NULL);\n";
    ss << "\t}\n";
    ss << "};\n\n";
    return ss.str();
}

/**
 * Creates the ff_node for the drain node
 * @param n node to be converted in code
 * @return working code as string
 */
string drain_declaration( const drain_node& n ) {
    stringstream ss;
    ss << "class " << n.name << "_stage : public ff_node {\n";
    ss << "protected:\n\tstd::unique_ptr<" << n.name << "> drn; \n\n";
    ss << "public:\n";
    ss << "\t" << n.name << "_stage() : drn(new " << n.name << "()) {}\n";
    ss << svc_init_decl(n.name) << "\n";
    ss << "\tvoid * svc(void *t) {\n";
    ss << "\t\tdrn->process((" << n.typein << "*) t);\n";
    ss << "\t\treturn (GO_ON);\n";
    ss << "\t}\n";
    ss << "};\n\n";
    return ss.str();
}

/**
 * Creates the ff_map constructor for the given name
 * @param name of the new ff_map
 * @param nw number of workers
 * @param value argument for the disableScheduler function
 * @return working code as string
 */
string mapred_constructor( const string& name, int nw, bool value ) {
    stringstream ss;
    ss << "\t" << name << "() : ff_Map(" << nw << ") {\n";
    ss << "\t\tpfr.disableScheduler(" << value << ");\n";
    ss << "\t}\n";
    return ss.str();
}

/**
 * Creates the ff_map class for the given node.
 * Works only with two tier model! Inside map
 * only seq and compseq
 * @param n node to be converted in code
 * @param env rpl environment
 * @return working code as string
 */
string map_declaration( map_node& n, rpl_environment& env ) {

    //FIXME: all TODOs not mine! TC

    // two-tier model: inside map nodes only seq or compseq are allowed:
    // if stream/datap inside, ignore it when compile and show a warning

    pardegree nw(env);         // setup the pardegree visitor
    get_seq_wrappers gsw(env); // setup the get_seq_wrappers visitor

    gsw(n); // find the sequential wrappers
    auto src_nodes = gsw.get_source_nodes();
    auto drn_nodes = gsw.get_drain_nodes();
    auto datap_nodes = gsw.get_datap_nodes();
    if (!src_nodes.empty() || !drn_nodes.empty() || ((int) nw(n)) != n.pardegree) {
        cout << "warning: two tier modeling application for generation of code" << endl;
    }

    stringstream ss;
    string typein  = datap_nodes.front()->typein;
    string typeout = datap_nodes.back()->typeout;
    string ffMap   = "ff_Map<"+typein+","+typeout+">";
    ss << "class map" << n.getid() << "_stage : public " << ffMap << " {\n";
    ss << "protected:\n";
    for (size_t i = 0; i < datap_nodes.size(); i++)
        ss << "\t" << datap_nodes[i]->name << " wrapper" << i << ";\n";

    ss << "public:\n";
    ss << mapred_constructor("map" + to_string(n.getid()) + "_stage", n.pardegree, false) << "\n";
    ss << "\t" << typeout << "* svc("<< typein << " *t) {\n";
    //TODO: _task is useless
    ss << "\t\t" << typein << "& _task = *t;\n";
    if (typein != typeout) {
        // if two different "container" types, we need this
        ss << "\t\t" << typeout << "* out = new " << typeout << "();\n";
        ss << "\t\tout->resize(_task.size());\n";
    } else {
        ss << "\t\t" << typeout << "* out = &_task;\n";
    }

    // start parallel for
    size_t i;
    string par;
    ss << "\t\t" << ffMap << "::parallel_for(0, _task.size(),";

    // begin lambda
    ss << "[this, &_task, &out](const long i) {\n";
    for (i = 0; i < datap_nodes.size()-1; i++) {
        par = !i ? "_task[i]" : ("res" + to_string(i-1));
        ss << "\t\t\tauto res" << i << " = wrapper" << i << ".op(" << par << ");\n";
    }
    par = !i ? "_task[i]" : ("res" + to_string(i-1));
    ss << "\t\t\t(*out)[i] = wrapper" << i << ".op(" << par << ");\n";
    ss << "\t\t}";
    // end lambda

    ss << "," << to_string( nw(n) ) << ");\n";
    // end parallel for

    //TODO memory leak of previous node
    ss << "\t\t" << "return out;\n";
    ss << "\t}\n";
    ss << "};\n\n";

    return ss.str();
}

/**
 * Creates an ff_map class for the reduce node
 * Works only with two tier model! Inside map
 * only seq and compseq
 * @param n node to be converted in code
 * @param env rpl environment
 * @return working code as string
 */
string red_declaration( reduce_node& n, rpl_environment& env ) {

    //FIXME: probably same issues of map, need refactoring TC

    // two-tier model: inside reduce nodes only seq or compseq are allowed:
    // if stream/datap inside, ignore it when compile and show a warning
    pardegree nw(env);           // setup the pardegree visitor
    get_seq_wrappers gsw(env);   // setup the get_seq_wrappers visitor
//    bool is_pointer = false;

    gsw(n); // find the sequential wrappers
    auto src_nodes = gsw.get_source_nodes();
    auto drn_nodes = gsw.get_drain_nodes();
    //auto seq_nodes = gsw.get_seq_nodes();
    auto datap_nodes = gsw.get_datap_nodes();
    if (((int) nw(n)) != n.pardegree)
        cout << "warning: two tier modeling will be applied for generation of code" << endl;
    if (!src_nodes.empty() || !drn_nodes.empty())
        cout << "warning: source/drain nodes inside reduce" << endl;

    // in case of multiple sequential nodes:
    // reduce(comp(a,b,c)) == pipe(map(comp(a,b)),c)
    // where a, b map nodes (must implement op)
    // where c is the only node implementing a binary operation f: v1 x v2 -> res
    stringstream ss;
    string typein  = datap_nodes.front()->typein;
    string typeout = datap_nodes.back()->typeout;
    string ffMap   = "ff_Map<"+typein+","+typeout+","+typeout+">";

    ss << "class reduce" << n.getid() << "_stage : public " << ffMap << " {\n";
    ss << "protected:\n";
    for (size_t i = 0; i < datap_nodes.size(); i++)
        ss << "\t" << datap_nodes[i]->name << " wrapper" << i << ";\n";
    ss << "public:\n";
    ss << mapred_constructor("reduce" + to_string(n.getid()) + "_stage", n.pardegree, true) << "\n";
    ss << "\t" << typeout << "* svc("<< typein <<"* t) {\n";
    string task = "_task";
    ss << "\t\t" << typein << "& _task = *t;\n";

    size_t idx = datap_nodes.size()-1;
    ss << "\t\t" << typeout << "* out  = new " << typeout << "(wrapper" << idx << ".identity);\n";
    ss << "\t\tauto reduceF = [this](" << typeout << "& sum, " << typeout << " elem) {sum = wrapper" << idx << ".op(sum, elem);};\n";

    if ( datap_nodes.size() > 1) {
        cout << "warning: reduce(comp(s1, s2, ..., sk, sn)) -> comp(map(s1,...,sk), reduce(sn))" << endl;
        string mapout  = datap_nodes[datap_nodes.size()-2]->typeout;
        //FIXME: from new to unique pointer?
        ss << "\t\t" << mapout << "* mapout = new " << mapout << "();\n";
        ss << "\t\tmapout->resize(_task.size());\n";

        size_t i;
        string par;
        // start parallel for
        ss << "\t\t"<<ffMap<<"::parallel_for(0, _task.size(),";
        // begin lambda
        ss << "[this, &_task, &mapout](const long i) {\n";
        for (i = 0; i < datap_nodes.size()-2; i++) {
            par = !i ? "_task[i]" : ("res" + to_string(i-1));
            ss << "\t\t\tauto res" << i << " = wrapper" << i << ".op(" << par << ");\n";
        }
        par = !i ? "_task[i]" : ("res" + to_string(i-1));
        ss << "\t\t\t(*mapout)[i] = wrapper" << i << ".op(" << par << ");\n";
        ss << "\t\t}";
        // end lambda
        ss << "," << to_string( nw(n) ) << ");\n";
        // end parallel for

        ss << "\t\tauto bodyF = [this,&mapout](const long i, " << typeout << "& sum) {sum = wrapper" << idx <<".op(sum, (*mapout)[i]);};\n";
        ss << "\t\t" << ffMap << "::parallel_reduce(*out, wrapper" << idx << ".identity,0,mapout->size(),bodyF,reduceF," << to_string( nw(n) ) << ");\n";

        ss << "\t\t\ndelete mapout;\n";
    } else {
        ss << "\t\tauto bodyF = [this,&_task](const long i, " << typeout << "& sum) {sum = wrapper" << idx <<".op(sum, _task[i]);};\n";
        ss << "\t\t" << ffMap << "::parallel_reduce(*out, wrapper" << idx << ".identity,0,_task.size(),bodyF,reduceF," << to_string( nw(n) ) << ");\n";
    }

    ss << "\t\t\n" << "return out;\n";
    ss << "\t}\n";
    ss << "};\n\n";

    return ss.str();
}

/**
 * Creates headers and includes
 * @return code for headers and includes as string
 */
string includes() {
    stringstream ss;
    ss << "#include <iostream>\n";
    ss << "#include <vector>\n\n";
    ss << "// specify include directory for fastflow\n";
    ss << "#include <ff/ff.hpp>\n";     //always needed for fastflow 3.0
    ss << "#include <ff/farm.hpp>\n";
    ss << "#include <ff/map.hpp>\n";
    ss << "#include <ff/pipeline.hpp>\n";
    ss << "#include <ff/parallel_for.hpp>\n\n";
    ss << "// specify include directory for RPL-Shell\n";
    ss << "#include <aux/types.hpp>\n";
    ss << "#include <aux/wrappers.hpp>\n";
    ss << "#include <aux/ff_comp.hpp>\n\n";
    ss << "// business code headers\n";
    for (auto& kv : business_headers) {
        ss << "#include <" << kv.first << ">\n";
    }
    ss << "\n\n";
    return ss.str();
}

/**
 * Generates main function
 * @param code body of the main
 * @return working code as string
 */
string main_wrapper( const string& code ) {
    regex nline("\n");
    stringstream ss;
    ss << "int main( int argc, char* argv[] ) {\n";
    ss << "\t" << regex_replace(code, nline, "\n\t" );
    ss << "\n}\n";
    return ss.str();
}

///////////////////////////////////////////////////////////////////////////////

ffcode::ffcode( rpl_environment& env ) :
    env(env),
    gsw(env),
    tds(env),
    compseq(env)
{}

/**
 * Wraps the sequential node
 * @param n sequential node
 */
void ffcode::visit( seq_node& n ) {
    seq_wraps(n.name);
}

/**
 * Wraps the source node
 * @param n source node
 */
void ffcode::visit( source_node& n ) {
    seq_wraps(n.name);
}

/**
 * Wraps the drain node
 * @param n drain node
 */
void ffcode::visit( drain_node& n ) {
    seq_wraps(n.name);
}

/**
 * Calls comp_pipe for a sequential composition
 * or a pipeline
 * @param n composite node
 */
void ffcode::visit( comp_node& n ) {
    if (n.compseq)
        comp_pipe("ff_comp", "comp", n);
    else
        comp_pipe("ff_pipeline", "pipe", n);
}

/**
 * Calls comp_pipe for a pipeline
 * @param n pipe node
 */
void ffcode::visit( pipe_node& n ) {
    comp_pipe("ff_pipeline", "pipe", n);
}

/**
 * Builds a farm with all the needed workers,
 * the collector is set to NULL
 * @param n farm node
 */
void ffcode::visit( farm_node& n ) {

    stringstream ss;
    string wvar = new_name("workers");
    string fvar = new_name("farm");

    ss << "\n// vector of workers of " << fvar << "\n";
    ss << "std::vector<ff_node*> " << wvar << ";\n";

    for (int i = 0; i < n.pardegree; i++) {
        ss << "\n// " << fvar << "'s worker " << (i+1) << "\n";

        n.get(0)->accept(*this);
        auto& p = code_lines.front();
        ss << p.second;
        ss << wvar << ".push_back(&" << p.first << ");\n";
        code_lines.pop();
    }

    ss << "\n// add " << wvar << " to " << fvar << "\n";
    //FIXME: this does not compile, we need types of farm
    //maybe fix: from farm<> to farm
    ss << "ff_farm " << fvar << ";\n";
    ss << fvar << ".add_workers(" << wvar << ");\n";
    ss << fvar << ".add_collector(NULL);\n\n";
    //FIXME: what if collector and emitter are present?

    assert(code_lines.empty());
    code_lines.push({fvar, ss.str()});
}

/**
 * Creates the code line for the map declaration
 * @param n map node
 */
void ffcode::visit( map_node& n ) {
    stringstream ss;
    string var = new_name("_map"+to_string(n.getid())+"_");
    ss << "map" << n.getid() << "_stage " << var << ";\n";
    code_lines.push({var, ss.str()});
}

/**
 * Creates the code line for the reduce declaration
 * @param n reduce node
 */
void ffcode::visit( reduce_node& n ) {
    stringstream ss;
    string var = new_name("_red"+to_string(n.getid())+"_");
    ss << "reduce" << n.getid() << "_stage " << var << ";\n";
    code_lines.push({var, ss.str()});
}

/**
 * Calls the accept on the id_node, if it exist
 * @param n id node
 */
void ffcode::visit( id_node& n ) {
    auto ptr = env.get(n.id, n.index);
    if (ptr != nullptr)
        ptr->accept(*this);
    else {
        //TODO: add err_repo?
        std::cout << n.id << " unexpected error in ffcode visit" << std::endl;
    }
}

/**
 * Retrieves sequential nodes inside composition nodes
 * @param node starting composition node
 * @param set container for the sequential nodes
 */
void add_seq_of_comp(skel_node* node, std::set<skel_node*>& set) {
    size_t i = 0;
    while (node->size() > i) {
        add_seq_of_comp(node->get(i), set);
        i++;
    }
    if (i == 0) {
        //it's a seq
        set.emplace(node);
    }
}
/*
template<typename T, typename U>
struct is_same_type
{
    static const bool value = false;
};

template<typename T>
struct is_same_type<T, T>
{
    static const bool value = true;
};

template<typename T, typename U>
bool eq_types() { return is_same_type<T, U>::value; }*/

/**
 * Builds all the source code
 * Builds all the source code
 * @param n root of the skeletons tree
 * @return code as a string representation
 */
string ffcode::operator()(skel_node& n) {

    ann_printer pr;
    string repr = "// " + pr.print(n) + "\n\n";

    // clear global environment:
    // names, business_headers, queue
    names.clear();
    business_headers.clear();
    queue<pair<string,string>> empty;
    swap( code_lines, empty );

    gsw(n);       // start visit to get seq wrappers
    compseq(n);   // start visit to set compseq nodes
    auto src_nodes = gsw.get_source_nodes();
    auto drn_nodes = gsw.get_drain_nodes();
    auto seq_nodes = gsw.get_seq_nodes();

    tds(n);     // start visit to get datap wrappers
    auto map_nodes = tds.get_map_nodes();
    auto red_nodes = tds.get_reduce_nodes();

    size_t idx;
    string code;
    string decls;

    for (auto src : src_nodes) {
        business_headers[src->file] = true;
        decls += source_declaration(*src);
    }

    for (auto drn : drn_nodes) {
        business_headers[drn->file] = true;
        decls += drain_declaration(*drn);
    }

    //here only nodes that need to be generated
    for (auto seq : seq_nodes) {
        business_headers[seq->file] = true;
        decls += stage_declaration(*seq);
    }

    idx = 0;
    for (auto mapn : map_nodes) {
        mapn->setid(idx++);
        decls += map_declaration(*mapn, env);
    }

    idx = 0;
    for (auto redn : red_nodes) {
        redn->setid(idx++);
        decls += red_declaration(*redn, env);
    }

    n.accept(*this);
    stringstream ss;

    /* mapping */
    ffmapper ffmap(env);
    auto ids = ffmap(n).get_worker_mapping();
    ss << "// worker mapping \n";
    ss << "const char worker_mapping[] = \"";
    for (size_t i = 0; i < ids.size(); i++)
        ss << ids[i] << ( i+1 < ids.size() ? "," : "\";\n");
    ss << "threadMapper::instance()->setMappingList(worker_mapping);\n";
    /* code lines */
    auto p = code_lines.front();
    ss << p.second << "\n";

#ifdef DEBUG
ss << "parameters::sequential = false;\n";
ss << "utils::write(\"" << pr.print(n) << "\", \"./res_ff.txt\");\n";
#endif



    /* run the program */
    ss << p.first << ".run_and_wait_end();\n";

    /* time and stats*/
    ss << "std::cout << \"Spent: \" << ";
    ss <<  p.first << ".ffTime() << \" msecs\" << std::endl;\n\n";
    ss << "#ifdef TRACE_FASTFLOW\n";
    ss << "std::cout << \"Stats: \" << std::endl;\n";
    ss << p.first << ".ffStats(std::cout);\n";
    ss << "#endif\n";

#ifdef DEBUG
ss << "utils::write(\"\\n---------------------\\n\", \"./res_ff.txt\");\n";
#endif

    ss << "return 0;\n";

    code_lines.pop();
    assert(code_lines.empty());

    // main code
    code = repr + includes() + decls + main_wrapper( ss.str() );

    return code;
}

/**
 * Adds declaration of a name to the code lines
 * @param name of the variable
 */
void ffcode::seq_wraps(const string& name) {
    stringstream ss;
    string var = new_name("_"+name);
    ss << name << "_stage " << var << ";\n";
    code_lines.push({var, ss.str()});
}

/**
 * Declares and add stages to a sequential composition or a pipeline
 * @param type sequential comp or pipe
 * @param name name of the stage
 * @param n comp or pipe node
 */
void ffcode::comp_pipe(const string& type, const string& name, skel_node& n) {
    stringstream ss;
    string var = new_name(name);

    // recursion over the children
    // and pick from code_lines
    std::vector<pair<string,string>> vec;
    for (size_t i = 0; i < n.size(); i++) {
        n.get(i)->accept(*this);
        vec.push_back(code_lines.front());
        ss << vec.back().second;
        code_lines.pop();
    }

    ss << type << " " << var << ";\n";
    for (const auto& p : vec)
        ss << var << ".add_stage(&" << p.first << ");\n";
    ss << "\n";

    assert(code_lines.empty());
    code_lines.push({var, ss.str()});
}
