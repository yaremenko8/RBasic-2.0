#include <string>
#include <stdexcept>
#include <rbexcept.h>
#include <vector>
#include <map>
#include <iostream>
#include <cctype>
#include <lexan.h>
#include <builtins.h>
#include <vars.h>

LexType Lexeme::getType() const
{
    return type;
}

LexType Lexeme::getSubType() const
{
    return subtype;
}

Lexeme Lexeme::toRPN()
{
    Lexeme temp = *this;
    switch(type) {
        case LT_TOKEN:
            temp.type = RPN_VAR;
            break;
        case LT_INFIX:
            temp.type = RPN_OP;
            break;
        case LT_FUNC:
            temp.type = RPN_FUNC;
            break;
        default:
        throw RBExcept(RBE_INTERNAL_ERROR);
    }
    return temp;
}

void Lexeme::operator=(const Lexeme& other)
{
    type = other.type;
    subtype = other.subtype;
    f = other.f;
    id = other.id;
    var = other.var;
    sub = other.sub;
    argc = other.argc;
    name = other.name;
    label = other.label;
}

int Lexeme::getArgCount() const
{
    return argc;
}

void Lexeme::setArgCount(int argcount)
{
    argc = argcount;
}

void Lexeme::incArgCount()
{
    argc++;
}

Lexarr::iterator Lexeme::getLabel() const
{
    return label;
}

void Lexeme::setLabel(Lexarr::iterator lbl) 
{
    label = lbl;
}

Variable* Lexeme::getVariable() const 
{
    return var;
} 

Identifier* Lexeme::getIdentifier() const
{
    return id;
}

Function* Lexeme::getFunction() const
{
    return f;
}

Subscriptor* Lexeme::getSubscriptor() const
{
    return sub;
}

void Lexeme::setVariable(Variable *v)
{
    var = v;
}

void Lexeme::setType(LexType t)
{
    type = t;
}

void Lexeme::setSubType(LexType st)
{
    subtype = st;
}

int precedence(LexType op)
{
    switch(op) {
    case LT_AASN:
        return -1;
    case LT_ASN:
        return 0;
    case LT_AND:
    case LT_OR:
        return 1;
    case LT_NOT:
        return 2;
    case LT_LS:
    case LT_GT:
    case LT_LSE:
    case LT_GTE:
    case LT_EQ:
    case LT_NEQ:
        return 3;
    case LT_PLS:
    case LT_MNS:
        return 4;
    case LT_MUL:
    case LT_DIV:
        return 5;
    case LT_CL:
        return 6;
    default:
        throw RBExcept(RBE_INTERNAL_ERROR);
    }
}

std::istream& operator>>(std::istream& istr, Lexarr& larr) 
{
    istr >> std::noskipws;
    std::string temp;
    static char c;
    static bool leftover = false;
    if(!leftover && !(istr >> c)) {
        larr.push_back(Lexeme(LT_EOF));
        return istr;
    }
    do {
        if(isspace(c)) {
            if(c == '\n') {
                larr.push_back(Lexeme(LT_NL));
                leftover = false;
                break;
            } else {
                continue;
            }
        } else if(isalpha(c) || c == '_' || c == '.') {
            temp = std::string(1, c);
            while(istr >> c  &&  (isalnum(c) || c == '_' || c == '.')) {
                temp += c;
            }
            if(temp == "TRUE") {
                larr.push_back(Lexeme(new Variable(new Logical(true), false)));
            } else if(temp == "FALSE") {
                larr.push_back(Lexeme(new Variable(new Logical(false), false)));   
            } else if(temp == "NULL") {
                larr.push_back(Lexeme(new Variable(new NULL_(), false))); 
            } else if(temp == "for") {
                larr.push_back(Lexeme(LT_FOR));
            } else if(temp == "in") {
                larr.push_back(Lexeme(LT_IN));
            } else if(Function::builtins.find(temp) != Function::builtins.end()) {
                larr.push_back(Lexeme(&Function::builtins[temp]));
            } else if(Identifier::scope.find(temp) != Identifier::scope.end()) {
                larr.push_back(Lexeme(Identifier::scope[temp]));
            } else {
                larr.push_back(Lexeme(new Identifier(temp)));
            }
            leftover = true;
            break;
        } else if(isdigit(c)) {
            temp = std::string(1, c);
            while(istr >> c  &&  (isdigit(c) || c == '.')) {
                temp += c;
            }
            float num;
            leftover = true;
            try {
                num = std::stof(temp);
            } catch(std::invalid_argument) {
                if(istr.eof()) {
                    leftover = false;
                }
                throw RBExcept(RBE_INVALID_NUMERIC_TOKEN, temp);
            }
            larr.push_back(Lexeme(new Variable(new Numeric(num), false)));
            break;
        } else if(c == '\"') {
            temp = std::string("");
            bool escape = false;
            leftover = false;    //In R you can have a character token split between input instances
            while(istr >> c  &&  c != '\"' && !escape) {  //But since I'm in charge here, we're not gonna have that.
                if(escape) {
                    if(c == 'n') {
                        temp += '\n';
                    } else if(c == 't') {
                        temp += '\t';
                    } else if(c == '\"') {
                        temp += '\"';
                    } else if(c == '\\') {
                        temp += '\\';
                    } else {
                        throw RBExcept(RBE_UNRECOGNISED_ESCAPE_SEQUENCE, "\\" + std::string(1, c));
                    }
                    escape = false;
                } else if (c == '\\') {
                    escape = true;
                } else {
                    temp += c;
                }
            }
            if(istr.eof()) {
                throw RBExcept(RBE_INVALID_CHARACTER_TOKEN);
            }
            larr.push_back(Lexeme(new Variable(new Character(temp), false)));
            break;
        } else {
            switch(c) {
            case '>':
                if(istr >> c  &&  c == '=') {
                    leftover = false;
                    larr.push_back(Lexeme(LT_INFIX, LT_GTE));
                } else {
                    leftover = true;
                    larr.push_back(Lexeme(LT_INFIX, LT_GT));
                }
                break;
            case '<':
                if(istr >> c  &&  (c == '=' || c == '-')) {
                    leftover = false;
                    if(c == '=') {
                        larr.push_back(Lexeme(LT_INFIX, LT_LSE));
                    } else {
                        larr.push_back(Lexeme(LT_INFIX, LT_ASN));
                    }
                } else {
                    leftover = true;
                    larr.push_back(Lexeme(LT_INFIX, LT_LS));
                }
                break;
            case '=':
                if(istr >> c  &&  c == '=') {
                    leftover = false;
                    larr.push_back(Lexeme(LT_INFIX, LT_EQ));
                } else {
                    leftover = true;
                    larr.push_back(Lexeme(LT_INFIX, LT_AASN));
                }
                break;
            case '!':
                if(istr >> c  &&  c == '=') {
                    leftover = false;
                    larr.push_back(Lexeme(LT_INFIX, LT_NEQ));
                } else {
                    leftover = true;
                    larr.push_back(Lexeme(LT_INFIX, LT_NOT));
                }
                break;
            case '+':
                larr.push_back(Lexeme(LT_INFIX, LT_PLS));
                leftover = false;
                break;
            case '-':
                larr.push_back(Lexeme(LT_INFIX, LT_MNS));
                leftover = false;
                break;
            case '*':
                larr.push_back(Lexeme(LT_INFIX, LT_MUL));
                leftover = false;
                break;
            case '/':
                larr.push_back(Lexeme(LT_INFIX, LT_DIV));
                leftover = false;
                break;
            case '&':
                larr.push_back(Lexeme(LT_INFIX, LT_AND));
                leftover = false;
                break;
            case '|':
                larr.push_back(Lexeme(LT_INFIX, LT_OR));
                leftover = false;
                break;
            case ':':
                larr.push_back(Lexeme(LT_INFIX, LT_CL));
                leftover = false;
                break;
            case '(':
                larr.push_back(Lexeme(LT_LPR));
                leftover = false;
                break;
            case ')':
                larr.push_back(Lexeme(LT_RPR));
                leftover = false;
                break;
            case '[':
                larr.push_back(Lexeme(LT_LBK));
                leftover = false;
                break;
            case ']':
                larr.push_back(Lexeme(LT_RBK));
                leftover = false;
                break;
            case '{':
                larr.push_back(Lexeme(LT_LBC));
                leftover = false;
                break;
            case '}':
                larr.push_back(Lexeme(LT_RBC));
                leftover = false;
                break;
            case ',':
                larr.push_back(Lexeme(LT_COM));
                leftover = false;
                break;
            case ';':
                larr.push_back(Lexeme(LT_SCL));
                leftover = false;
                break;
            case '%':
                if(istr >> c && c == '*' && istr >> c && c == '%') {
                    leftover = false;
                    larr.push_back(Lexeme(LT_INFIX, LT_MTXPROD));
                } else {
                    throw RBExcept(RBE_UNEXPECTED_INPUT, std::string(1, c));
                }
                break;
            case '#':
                while(istr >> c && c != '\n');
                leftover = true;
                break;
            default:
                leftover = false;
                throw RBExcept(RBE_UNEXPECTED_INPUT, std::string(1, c));
            }
            break;
        }
    } while(istr >> c);
    if(istr.eof()) {
        leftover = false;
    }
    return istr;
}

