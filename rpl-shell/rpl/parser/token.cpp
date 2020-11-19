#include "token.hpp"
#include "parameters.hpp"

using namespace std;

token::token(type kind, string data, int pos) :
    kind(kind),
    data(data),
    pos(pos)
{}

map<string, token::type> token::st_map =
{
   /* symbols */
   {"(",        token::open},
   {")",        token::close},
   {"[",        token::open_square},
   {"]",        token::close_square},
   {",",        token::comma},
   {"=",        token::equals},
   {"+",        token::plus },
   {"-",        token::minus },
   {"true",     token::bool_true },
   {"false",    token::bool_false },

   /* verb's keywords */
   {"with",     token::with},
   {"by",       token::by},
   {"in",       token::in},
   {"as",       token::as},

   /* verbs */
   {"show",     token::show},
   {"set",      token::set},
   {"annotate", token::annotate},
   {"rewrite",  token::rewrite},
   {"optimize", token::optimize},
   {"history",  token::history},
   {"import",   token::import},
   {"gencode",  token::gencode},
   {"expand",   token::expand},
   {"add",      token::add},
   {"load",     token::load},

   /* skeleton patterns */
   {"seq",      token::seq},
   {"Seq",      token::seq},
   {"source",   token::source},
   {"Source",   token::source},
   {"drain",    token::drain},
   {"Drain",    token::drain},
   {"comp",     token::comp},
   {"Comp",     token::comp},
   {"pipe",     token::pipe},
   {"Pipe",     token::pipe},
   {"farm",     token::farm},
   {"Farm",     token::farm},
   {"map",      token::map},
   {"Map",      token::map},
   {"reduce",   token::reduce},
   {"Reduce",   token::reduce},

   /* parameters */
   {par::servicetime,   token::parameter},
   {par::latency,       token::parameter},
   {par::efficiency,    token::parameter},
   {par::compltime,     token::parameter},
   {par::pardegree,     token::parameter},
   {par::resources,     token::parameter},
   {par::datap,         token::parameter},
   {par::grain,         token::parameter},

   {par::emitter_time,  token::parameter},
   {par::collector_time,token::parameter},
   {par::scatter_time,  token::parameter},
   {par::gather_time,   token::parameter},
   {par::dimension,     token::parameter},
   {par::inputsize,     token::parameter},

   {par::typein,  token::parameter},
   {par::typeout, token::parameter},

   /* parameter flags */
   {par::unranked,      token::parameter},
   {par::noann,         token::parameter},
   {par::full,          token::parameter},
   {par::static_grain,  token::parameter},
   {par::dynamic_grain, token::parameter},

   /* future stuff? */
   {par::arch,          token::parameter}
};

map<token::type, string> token::ts_map =
{
   {token::open,    "("},
   {token::close,   ")"},
   {token::comma,   ","},
   {token::seq,     "seq"},
   {token::comp,    "comp"},
   {token::pipe,    "pipe"},
   {token::farm,    "farm"},
   {token::map,     "map"},
   {token::reduce,  "reduce"}
};
