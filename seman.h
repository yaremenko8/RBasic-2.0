#ifndef __SEMAN_H__
#define __SEMAN_H__
#include <lexan.h>

struct LoopData
{
    Lexeme iterator;
    Lexeme subiter;
    Lexeme len;
    Lexeme iterable;
    Lexeme beglabel;
    Lexarr::iterator endlabel;
    LoopData() {}
};


struct Dump
{
public:
    bool complete;
    //Lexarr::iterator it;
    int argcount;
    std::string varname;
    std::vector<LoopData> loops;
    LexType last;
    Lexarr  stack;
    int  inargs;
    int  nstloop;
    int  nstblock;
    int  nstbrack;
    bool nonempty;
    bool exprbegin;
    Dump() {complete = true;}
    static Dump dump;
};


#define DUMP    Dump::dump.complete = false;      \
                Dump::dump.argcount = argcount;   \
                Dump::dump.varname = varname;     \
                Dump::dump.loops = loops;         \
                Dump::dump.last = last;           \
                Dump::dump.stack = stack;         \
                Dump::dump.inargs = inargs;       \
                Dump::dump.nstloop = nstloop;     \
                Dump::dump.nstblock = nstblock;   \
                Dump::dump.nstbrack = nstbrack;   \
                Dump::dump.nonempty = nonempty;   \
                Dump::dump.exprbegin = exprbegin;

#define UNDUMP  Dump::dump.complete = true;       \
                argcount = Dump::dump.argcount;   \
                varname = Dump::dump.varname;     \
                loops = Dump::dump.loops;         \
                last = Dump::dump.last;           \
                stack = Dump::dump.stack;         \
                inargs = Dump::dump.inargs;       \
                nstloop = Dump::dump.nstloop;     \
                nstblock = Dump::dump.nstblock;   \
                nstbrack = Dump::dump.nstbrack;   \
                nonempty = Dump::dump.nonempty;   \
                exprbegin = Dump::dump.exprbegin;

Lexarr& operator>>(Lexarr&, Lexarr&);

#endif