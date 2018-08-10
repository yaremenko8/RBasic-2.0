#include <lexan.h>
#include <seman.h>
#include <builtins.h>
#include <rbexcept.h>


bool parcl(Lexarr& stack)  //checks if all parantheses are closed
{ //!
    LexType temp; //not exactly sure about that stop condition
    for(Lexarr::reverse_iterator it = stack.rbegin(); (it->getType() != RPN_LOOP) && (it->getType() != RPN_EOF); it++) {
        temp = it->getType();
        if(temp == LT_LPR  ||  temp == LT_LBK) {
            return false;
        }
    }
    return true;
}

Lexarr& operator>>(Lexarr& ext, Lexarr& rpn) 
{
    if(Dump::dump.complete) {
        rpn.reserve(ext.size() + 1000);
    }
    int argcount = 0;
    LexType temp;
    std::string varname;
    std::vector<LoopData> loops;
    LexType last = RPN_EOF; //EOF = begining of expression, FUNC = function, OP = infix, VAR = end of value
    Lexarr  stack(1, Lexeme(RPN_EOF)); //we should probably use a list here
    int  inargs   = 0;
    int  nstloop  = 0;
    int  nstblock = 0;
    int  nstbrack = 0;
    bool nonempty  = false;
    bool exprbegin = true;
    if(!Dump::dump.complete) {
        UNDUMP
    }
    for(Lexarr::iterator it = ext.begin(); it != ext.end(); it++) {
        switch(it->getType()) {
        case LT_TOKEN:
            if(last == RPN_VAR  ||  last == RPN_FUNC) {
                throw RBExcept(RBE_SYNTAX_ERROR, "token preceded by wrong lexeme");
            }
            rpn.push_back(it->toRPN());
            last = RPN_VAR;
            exprbegin = false;
            break;
        case LT_INFIX:
            if(last == RPN_FUNC) {
                throw RBExcept(RBE_SYNTAX_ERROR, "infix preceded by wrong lexeme");
            }
            temp = it->getSubType();
            if(temp == LT_AASN  &&  inargs == 0) {
                throw RBExcept(RBE_SYNTAX_ERROR, "argument assignment outside of arguments");
            }
            if(temp == LT_ASN) { //have another look at that
                while(stack.back().getType() == RPN_OP && 
                    precedence(temp) < precedence(stack.back().getSubType())) {
                  rpn.push_back(stack.back());
                  stack.pop_back();
                }
            } else {
                while(stack.back().getType() == RPN_OP && 
                    precedence(temp) <= precedence(stack.back().getSubType())) {
                  rpn.push_back(stack.back());
                  stack.pop_back();
                }
            }
            stack.push_back(it->toRPN());
            if(last == RPN_EOF  ||  last == RPN_OP) {
                stack.back().setArgCount(1); //unary op
            } else {
                stack.back().setArgCount(2); //binary op
            }
            last = RPN_OP;
            exprbegin = false;
            break;
        case LT_LPR:
            if(last == RPN_VAR) {
                throw RBExcept(RBE_SYNTAX_ERROR, "left paranthesis preceded by wrong lexeme");
            }
            stack.push_back(*it);
            stack.back().setArgCount(0);
            last = RPN_EOF;
            exprbegin = false;
            break;
        case LT_FUNC:
            if(last == RPN_VAR  ||  last == RPN_FUNC) {
                throw RBExcept(RBE_SYNTAX_ERROR, "function preceded by wrong lexeme");
            }
            stack.push_back(it->toRPN());
            inargs++;
            last = RPN_FUNC;
            exprbegin = false;
            break;
        case LT_COM:
            if(inargs == 0  &&  nstbrack == 0) {
                throw RBExcept(RBE_SYNTAX_ERROR, "comma outside of arguments");
            }
            while(stack.back().getType() != LT_LPR  &&
                  stack.back().getType() != LT_LBK) {
                if(stack.back().getType() == RPN_EOF) {
                    throw RBExcept(RBE_SYNTAX_ERROR, "eof occured while scanning for parantheses");
                }
                rpn.push_back(stack.back());
                stack.pop_back();
            }
            stack.back().incArgCount();
            if(stack.back().getType() == LT_LPR  &&  
               last != RPN_VAR) {
                throw RBExcept(RBE_SYNTAX_ERROR, "empty function argument");
            }
            if(stack.back().getType() == LT_LBK  &&  
               last == RPN_EOF) {
                rpn.push_back(Lexeme(RPN_VAR));  //placeholder
            }
            last = RPN_EOF;
            exprbegin = false;
            break;
        case LT_RPR: //have another look at that
            if(last == RPN_EOF  &&  
               stack.back().getType() == LT_LPR &&
               stack.back().getArgCount() == 0) {
                argcount = -1;
                stack.pop_back();
                if(stack.back().getType() != RPN_FUNC) {
                    throw RBExcept(RBE_SYNTAX_ERROR, "empty parantheses");
                }
            } else if(last == RPN_VAR) {
                while(stack.back().getType() != LT_LPR) {
                    if(stack.back().getType() == RPN_EOF  ||
                       stack.back().getType() == LT_LBK) {
                        throw RBExcept(RBE_SYNTAX_ERROR, "mismatched parantheses or brackets");
                    }
                    rpn.push_back(stack.back());
                    stack.pop_back();
                }
                argcount = stack.back().getArgCount();
                stack.pop_back();
            } else {
                throw RBExcept(RBE_SYNTAX_ERROR, "right paranthesis preceded by wrong lexeme");
            }
            argcount++;
            if(stack.back().getType() == RPN_FUNC) {
                rpn.push_back(stack.back());
                stack.pop_back();
                rpn.back().setArgCount(argcount);
                inargs--;
            } else if(stack.back().getType() == RPN_LOOP) {
                if(argcount != 1) {
                    throw RBExcept(RBE_SYNTAX_ERROR, "commas in iterable expression");
                }
                rpn.push_back(Lexeme(RPN_OP, LT_ASN, 2));
                rpn.push_back(Lexeme(RPN_OP, RPN_NOTHING, 1));
                rpn.push_back(loops.back().len);
                rpn.push_back(loops.back().iterable);
                rpn.push_back(Lexeme(&Function::builtins["length"]).toRPN());
                rpn.back().setArgCount(1);
                rpn.push_back(Lexeme(RPN_OP, LT_ASN, 2));
                rpn.push_back(Lexeme(RPN_OP, RPN_NOTHING, 1));
                rpn.push_back(loops.back().subiter);
                rpn.push_back(Lexeme(new Variable(new Numeric(1), false)).toRPN());
                rpn.push_back(Lexeme(RPN_OP, LT_ASN, 2));
                rpn.push_back(Lexeme(RPN_OP, RPN_NOTHING, 1));
                rpn.push_back(loops.back().subiter);
                loops.back().beglabel = Lexeme(rpn.end() - 2);
                rpn.push_back(loops.back().len);
                rpn.push_back(Lexeme(RPN_OP, LT_LSE, 2));
                rpn.push_back(Lexeme(rpn.begin()));
                loops.back().endlabel = rpn.end() - 1;
                rpn.push_back(Lexeme(RPN_OP, RPN_FGO, 2));
                rpn.push_back(loops.back().iterator);
                rpn.push_back(loops.back().iterable);
                rpn.push_back(loops.back().subiter);
                rpn.push_back(Lexeme(&Function::builtins["0construct.vector"]).toRPN());
                rpn.back().setArgCount(2);
                rpn.push_back(Lexeme(RPN_OP, LT_ASN, 2));
                rpn.push_back(Lexeme(RPN_OP, RPN_NOTHING, 1));
                exprbegin = true;
                last = RPN_EOF;
                nstloop++;
                break;
            } else if(argcount != 1) { //!
                throw RBExcept(RBE_SYNTAX_ERROR, 
                      "comma outside of arguments or mismatched parathesis in arguments");
            }
            exprbegin = false;
            last = RPN_VAR;
            break;
        case LT_NL:
            if(exprbegin) {
                if(stack.back().getType() == RPN_LOOP) {
                    throw RBExcept(RBE_SYNTAX_ERROR, "empty loop");
                } else { //last != RPN_EOF should not occur
                    break;
                }
            }
            if(!parcl(stack) || last == RPN_OP  || last == LT_FUNC) { //last == RPN_EOF should not occur
                break;
            }
        case LT_RBC:
            if(it->getType() == LT_RBC  &&  exprbegin) {
                nstblock--;
                if(nstblock < 0) {
                    throw RBExcept(RBE_SYNTAX_ERROR, "mismatched braces");
                }
                if(nonempty) {
                    rpn.pop_back();
                } else {
                    rpn.push_back(Lexeme(new Variable(new NULL_(), false)).toRPN());
                    nonempty = true;
                    last     = RPN_EOF;
                }
                stack.pop_back();
                last = RPN_VAR;
                exprbegin = false;
                break;
            }
        case LT_SCL:
        case LT_EOF:
            if(exprbegin) {
                if(stack.back().getType() == RPN_LOOP) {
                    throw RBExcept(RBE_SYNTAX_ERROR, "empty loop");
                } else { //last != RPN_EOF should not occur
                    break;
                }
            }
            if(!parcl(stack) || last != RPN_VAR) { //last == RPN_EOF should not occur
                if(it->getType() != LT_EOF) {
                    throw RBExcept(RBE_SYNTAX_ERROR, "unexpected semicolon");
                } else {
                    DUMP
                    throw RBExcept(RBE_INCOMPLETE_EXPRESSION);
                }
            }
            while(stack.back().getType() != RPN_EOF  &&
                  stack.back().getType() != RPN_LOOP) {
                rpn.push_back(stack.back());
                stack.pop_back();
            }
            if(stack.back().getType() == RPN_LOOP) {
                rpn.push_back(Lexeme(RPN_OP, RPN_NOTHING, 1));
                rpn.push_back(loops.back().subiter);
                rpn.push_back(loops.back().subiter);
                rpn.push_back(Lexeme(new Variable(new Numeric(1), false)).toRPN());
                rpn.push_back(Lexeme(RPN_OP, LT_PLS, 2));
                rpn.push_back(Lexeme(RPN_OP, LT_ASN, 2));
                rpn.push_back(Lexeme(RPN_OP, RPN_NOTHING, 1));
                rpn.push_back(loops.back().beglabel);
                rpn.push_back(Lexeme(RPN_OP, RPN_GO, 1));
                rpn.push_back(Lexeme(RPN_VAR));  //used to be RPN_LOOP
                loops.back().endlabel->setLabel(rpn.end() - 2);
                stack.pop_back();
                loops.pop_back();
                nstloop--;
                exprbegin = false;
                last      = RPN_VAR;
                it--;
            } else if(it->getType() == LT_RBC) {
                nstblock--;
                if(nstblock < 0) {
                    throw RBExcept(RBE_SYNTAX_ERROR, "mismatched braces");
                }
                stack.pop_back();
                last = RPN_VAR;
                exprbegin = false;
            } else {
                if(nstblock == 0) {
                    rpn.push_back(Lexeme(RPN_OP, RPN_VERBOSE, 1));
                } else {
                    rpn.push_back(Lexeme(RPN_OP, RPN_NOTHING, 1));
                }
                nonempty  = true;
                last      = RPN_EOF;
                exprbegin = true;
            }
            break;
        case LT_FOR:
            if(!exprbegin) {
                throw RBExcept(RBE_SYNTAX_ERROR, "attempted loop evaluation");
            }
            it++;
            while(it->getType() == LT_NL) {
                it++;
            }
            if(it->getType() != LT_LPR) {
                throw RBExcept(RBE_SYNTAX_ERROR, "no left paranthesesis after \'for\'");
            }
            it++;
            if(it->getSubType() != LT_ID) {
                throw RBExcept(RBE_SYNTAX_ERROR, "invalid or no identifier provided for loop");
            }
            loops.push_back(LoopData());
            loops.back().iterator = (*it).toRPN();
            it++;
            while(it->getType() == LT_NL) {
                it++;
            }
            if(it->getType() != LT_IN) {
                throw RBExcept(RBE_SYNTAX_ERROR, "\'for\' without an \'in\'");
            }
            varname = std::string("0cnt") + std::to_string(nstloop);
            if(Identifier::scope.find(varname) != Identifier::scope.end()) {
                loops.back().subiter = Lexeme(Identifier::scope[varname]).toRPN();
            } else {
                loops.back().subiter = Lexeme(new Identifier(varname)).toRPN();
            }
            varname = std::string("0len") + std::to_string(nstloop);
            if(Identifier::scope.find(varname) != Identifier::scope.end()) {
                loops.back().len = Lexeme(Identifier::scope[varname]).toRPN();
            } else {
                loops.back().len = Lexeme(new Identifier(varname)).toRPN();
            }
            varname = std::string("0temp") + std::to_string(nstloop);
            if(Identifier::scope.find(varname) != Identifier::scope.end()) {
                loops.back().iterable = Lexeme(Identifier::scope[varname]).toRPN();
            } else {
                loops.back().iterable = Lexeme(new Identifier(varname)).toRPN();
            }
            stack.push_back(Lexeme(RPN_LOOP));
            stack.push_back(Lexeme(LT_LPR));
            stack.back().setArgCount(0);
            rpn.push_back(loops.back().iterable);
            last = RPN_EOF;
            exprbegin = false;
            break;
        case LT_LBK:
            if(last != RPN_VAR) {
                throw RBExcept(RBE_SYNTAX_ERROR, "left bracket preceded by wrong lexeme");
            }
            stack.push_back(*it);
            stack.back().setArgCount(0);
            nstbrack++;
            last = RPN_EOF;
            exprbegin = false;
            break;
        case LT_RBK:
            if(last == RPN_EOF  &&  
               stack.back().getType() == LT_LBK &&
               stack.back().getArgCount() == 0) {
                argcount = -1;
                stack.pop_back();
                rpn.push_back(Lexeme(RPN_VAR));
            } else if(last == RPN_VAR) {
                while(stack.back().getType() != LT_LBK) {
                    if(stack.back().getType() == RPN_EOF  ||
                       stack.back().getType() == LT_LPR) {
                        throw RBExcept(RBE_SYNTAX_ERROR, "mismatched brackets or parantheses");
                    }
                    rpn.push_back(stack.back());
                    stack.pop_back();
                }
                argcount = stack.back().getArgCount();
                stack.pop_back();
            } else if(last == RPN_EOF  &&  
                      stack.back().getType() == LT_LBK &&
                      stack.back().getArgCount() == 1) {
                rpn.push_back(Lexeme(RPN_VAR));  //placeholder
                argcount = 1;
                stack.pop_back();
            } else {
                throw RBExcept(RBE_SYNTAX_ERROR, "right bracket preceded by wrong lexeme");
            }
            argcount++;
            if(argcount == 2) {
                rpn.push_back(Lexeme(&Function::builtins["0construct.matrix"]).toRPN());
                rpn.back().setArgCount(argcount + 1);
            } else if(argcount == 1) {
                rpn.push_back(Lexeme(&Function::builtins["0construct.vector"]).toRPN());
                rpn.back().setArgCount(argcount + 1);
            } else if(argcount == 0) {
                rpn.push_back(Lexeme(&Function::builtins["0construct.vector"]).toRPN());
                rpn.back().setArgCount(2);
            } else {
                throw RBExcept(RBE_SYNTAX_ERROR, "excessive arguments in brackets");
            }
            nstbrack--;
            exprbegin = false;
            last = RPN_VAR;
            break;
        case LT_LBC:
            nonempty = false;
            nstblock++;
            stack.push_back(Lexeme(RPN_EOF));
            exprbegin = true;
            last = RPN_EOF;
            break;
        case LT_IN:
            throw RBExcept(RBE_SYNTAX_ERROR, "\'in\' without a \'for\'");
        default:
            throw RBExcept(RBE_INTERNAL_ERROR);
        }
    }
    return ext;
}
