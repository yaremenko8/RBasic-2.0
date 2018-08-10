#ifndef __LEXAN_H__
#define __LEXAN_H__
#include <string>
#include <vector>
#include <iostream>
#include <vars.h>
enum LexType
{
    LT_NUM, //==VT_NUM
    LT_CHR, //==VT_CHR
    LT_LOG, //==VT_LOG
    LT_NULL,//==VT_NULL
    LT_ID,
    LT_INFIX, //unary operators are a subset of infixes. Don't ask.
    LT_TOKEN,
    LT_MTXPROD,
    LT_FUNC,
    LT_ASN,
    LT_SCL,
    LT_NL,
    LT_AND,
    LT_OR,
    LT_NOT,
    LT_LS,
    LT_GT,
    LT_LSE,
    LT_GTE,
    LT_EQ,
    LT_NEQ,
    LT_PLS,
    LT_MNS,
    LT_MUL,
    LT_DIV,
    LT_CL,
    LT_LPR,
    LT_RPR,
    LT_LBK,
    LT_RBK,
    LT_LBC,
    LT_RBC,
    LT_COM,
    LT_EOF,
    LT_FOR,
    LT_IN,
    LT_AASN,//argument assignment is a special infix operator that produces an argument assignment object. 
    LT_UNSPECIFIED,
    RPN_ASNO,
    RPN_AASNO,//argument assignment  object. 'var' points to rhs value, 'id' merely points to a namesake identifier
    RPN_FUNC,
    RPN_VAR,
    RPN_OP,
    RPN_NOTHING, //eats up the argument
    RPN_LOOP,
    RPN_EOF,
    RPN_GO,
    RPN_FGO,
    RPN_LABEL,
    RPN_VERBOSE,
    RPN_BLOCK,
    RPN_SUBSCRIPTOR,
};

int precedence(LexType);

class Function;
class Lexeme;

typedef std::vector<Lexeme> Lexarr;

class Lexeme
{
private:
    LexType type;
    LexType subtype;
    Function    *f;   //optional fields
    Identifier  *id;
    Variable    *var;
    Subscriptor *sub;
    int argc;
    std::string name;
    Lexarr::iterator label;
public:
    LexType getType() const;
    LexType getSubType() const;
    int getArgCount() const;
    void setArgCount(int);
    void incArgCount();
    Lexarr::iterator getLabel() const;
    void setLabel(Lexarr::iterator);
    void setType(LexType);
    void setSubType(LexType);
    Variable    *getVariable() const;
    void         setVariable(Variable *);
    Identifier  *getIdentifier() const;
    Function    *getFunction() const;
    Subscriptor *getSubscriptor() const;
    Lexeme toRPN();
    Lexeme(LexType t = LT_UNSPECIFIED, LexType st = LT_UNSPECIFIED, int argcount = -1) : 
        type(t), 
        subtype(st),
        argc(argcount) {}
    Lexeme(Function *f0) : 
        type(LT_FUNC), 
        subtype(LT_UNSPECIFIED), 
        f(f0) {}
    Lexeme(Identifier *id0) : 
        type(LT_TOKEN), 
        subtype(LT_ID), 
        id(id0) {}
    Lexeme(Variable *v) : 
        type(LT_TOKEN),
        subtype((LexType) v->getType()), 
        var(v) {}
    Lexeme(Subscriptor *s) :
        type(RPN_VAR),
        subtype(RPN_SUBSCRIPTOR),
        var(NULL),
        sub(s) {}
    Lexeme(const Lexeme& other) : 
        type(other.type),
        subtype(other.subtype),
        f(other.f),
        id(other.id),
        var(other.var),
        sub(other.sub),
        argc(other.argc),
        name(other.name),
        label(other.label) {}
    Lexeme(Lexarr::iterator it) :
        type(RPN_LABEL),
        subtype(LT_UNSPECIFIED),
        label(it) {}
    void operator=(const Lexeme& other);
};

std::istream& operator>>(std::istream&, Lexarr&);


#endif
