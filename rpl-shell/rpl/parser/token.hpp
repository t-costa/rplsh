///////////////////////////////////////////////////////////////////////////////
//
//  Token class
//
///////////////////////////////////////////////////////////////////////////////
#ifndef token_hpp
#define token_hpp

#include <string>
#include <map>

struct token
{
    typedef enum
    {
        open = 1,
        close,
        open_square,
        close_square,
        comma,
        equals,
        show,
        set,

        annotate,
        rewrite,
        optimize,
        history,
        import,
        gencode,
        expand,
        load,
        add,
        seq,
        source,
        drain,
        comp,
        pipe,
        farm,
        map,
        reduce,
        divide_conquer,
        integer,
        number,
        bool_true,
        bool_false,
        word,
        file,
        with,
        by,
        in,
        as,
        illegal,
        parameter,
        plus,
        minus,
        eol

    } type;

    /* constructor for a token object */
    token(token::type kind, std::string data, int pos = -1);

    /* static map containing all the <string, tokentype> bindings */
    static std::map<std::string, token::type> st_map;

    /* static map containing some of the <tokentype, string> bindings */
    static std::map<token::type, std::string> ts_map;

    /* public fields of a token obj */
    token::type kind;   // type of the token    (e.g. integer)
    std::string data;   // satellite data       (e.g. "1024")
    int pos;            // position in the text
};

#endif
