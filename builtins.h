#ifndef __BUILTINS_H__
#define __BUILTINS_H__
#include <map>
#include <string>
#include <vector>
#include <lexan.h>
enum
{
    LIMITLESS_ARGS = -1, 
};

class Function
{
private:
    static std::map<std::string, Function> init_builtins();
    int argcount;
    std::map<std::string, int> nparams;
    Lexarr defaults;
    Lexeme (*f)(Lexarr);
public:
    Lexeme operator()(Lexarr::iterator it, int len);
    Function(Lexeme (*f0)(Lexarr), 
             int ac = LIMITLESS_ARGS, 
             std::map<std::string, int> np0 = std::map<std::string, int>(),
             Lexarr defs = Lexarr()) :
        argcount(ac), 
        nparams(np0),
        defaults(defs),
        f(f0) {}
    Function() {} //delete this shit when you're done
    static std::map<std::string, Function> builtins;
};

void deltemp(Lexeme& var);

Variable *getvar(Lexeme& obj);


#endif