#include <lexan.h>
#include <builtins.h>
#include <vars.h>
#include <vector>
#include <iostream>
#include <rbexcept.h>
#include <exec.h>

void getargs(Lexarr& stack, Lexeme* rarg, Lexeme* larg = NULL)
{
    *rarg = stack.back();  //did we even implement lexeme assignment?
    if(rarg->getType() == RPN_EOF) {
        throw RBExcept(RBE_EXECUTION_FAILURE);
    }
    stack.pop_back();
    if(larg != NULL) {
        *larg = stack.back();
        if(larg->getType() == RPN_EOF) {
            throw RBExcept(RBE_EXECUTION_FAILURE);
        }
        stack.pop_back();
    }
}


#define BINOP(OP) stack.push_back(Lexeme((*getvar(larg)) OP (*getvar(rarg))).toRPN())
#define UNOP(OP) stack.push_back(Lexeme(OP (*getvar(rarg))).toRPN())

void exec(Lexarr& rpn)
{
    LexType type, stype;
    Lexeme larg, rarg, ret;
    Lexarr stack(1, Lexeme(RPN_EOF));
    Variable *temp;
    int argc;
    for(Lexarr::iterator it = rpn.begin(); it != rpn.end(); it++) {
        type  = it->getType();
        stype = it->getSubType();
        argc  = it->getArgCount();
        if(type == RPN_FUNC) {
            ret = (*it->getFunction())(stack.end() - argc, argc);
            for(int i = 0; i < argc; i++) {
                deltemp(stack.back());
                stack.pop_back();
            }
            stack.push_back(ret);
        } else if(type == RPN_OP) {
            if(argc == 2) {
                getargs(stack, &rarg, &larg);
                switch(stype) {
                case LT_ASN:
                    stack.push_back(Lexeme(RPN_VAR, RPN_ASNO));
                    if(larg.getSubType() == LT_ID) {
                        *larg.getIdentifier() = *getvar(rarg);
                        stack.back().setVariable(new Variable(*getvar(larg)));
                    } else if(larg.getSubType() == RPN_SUBSCRIPTOR) {
                        *larg.getSubscriptor() = *getvar(rarg);
                        stack.back().setVariable(new Variable(*larg.getSubscriptor()));
                    } else {
                        throw RBExcept(RBE_ILLEGAL_ASSIGNMENT);
                    }
                    break;
                case LT_AASN:
                    if(larg.getSubType() != LT_ID) {
                        throw RBExcept(RBE_ARGUMENT_ASSIGNMENT_ERROR);
                    }
                    stack.push_back(Lexeme(larg.getIdentifier()).toRPN()); //RPN conversions are crucial.
                    stack.back().setVariable(rarg.getSubType() == RPN_SUBSCRIPTOR ?
                                             new Variable(*rarg.getSubscriptor())  :
                                             new Variable(*getvar(rarg)));
                    stack.back().setSubType(RPN_AASNO);
                    break;
                case RPN_FGO:
                    temp = getvar(larg);
                    if(!temp->isTrue()) {
                        it = rarg.getLabel();
                    }
                    break;
                case LT_AND:
                    BINOP(&);
                    break;
                case LT_OR:
                    BINOP(|);
                    break;
                case LT_LS:
                    BINOP(<);
                    break;
                case LT_LSE:
                    BINOP(<=);
                    break;
                case LT_GT:
                    BINOP(>);
                    break;
                case LT_GTE:
                    BINOP(>=);
                    break;
                case LT_EQ:
                    BINOP(==);
                    break;
                case LT_NEQ:
                    BINOP(!=);
                    break;
                case LT_PLS:
                    BINOP(+);
                    break;
                case LT_MNS:
                    BINOP(-);
                    break;
                case LT_MUL:
                    BINOP(*);
                    break;
                case LT_DIV:
                    BINOP(/);
                    break;
                case LT_CL:
                    BINOP(^);
                    break;
                case LT_MTXPROD:
                    BINOP(&&);
                    break;
                default:
                    throw RBExcept(RBE_UNDEFINED_OP_OVERLOAD);
                }
                deltemp(larg);
                deltemp(rarg);
            } else if(argc == 1) {
                getargs(stack, &rarg);
                switch(stype) {
                case LT_PLS:
                    UNOP(+);
                    break;
                case LT_MNS:
                    UNOP(-);
                    break;
                case LT_NOT:
                    UNOP(!);
                    break;
                case RPN_GO:
                    it = rarg.getLabel();
                    break;
                case RPN_NOTHING:
                    break;
                case RPN_VERBOSE:
                    if(rarg.getSubType() != RPN_ASNO && (rarg.getType() != RPN_VAR  ||  rarg.getSubType() != LT_UNSPECIFIED)) {
                        std::cout << getvar(rarg)->str() << std::endl;
                    }
                    Variable::clearHeap();
                    break;
                default:
                    throw RBExcept(RBE_UNDEFINED_OP_OVERLOAD);
                }
                deltemp(rarg);
            } else {
                throw RBExcept(RBE_INTERNAL_ERROR);
            }
        } else if(type == RPN_VAR  ||
                  type == RPN_LABEL) {
            stack.push_back(*it);
        } else {
            throw RBExcept(RBE_INTERNAL_ERROR);
        }
    }
}