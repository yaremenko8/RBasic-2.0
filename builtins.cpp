#include <map>
#include <string>
#include <rbexcept.h>
#include <lexan.h>
#include <builtins.h>
#include <vars.h>
#include <iostream>

void deltemp(Lexeme& var) //deletes temporary stuff
{
    LexType t = var.getType(), st = var.getSubType();
    if(st == LT_ID  ||
       t == RPN_LABEL  ||
       (t == RPN_VAR  &&  st == LT_UNSPECIFIED)) {
        return;
    }
    if(t != RPN_VAR) {
        throw RBExcept(RBE_INTERNAL_ERROR);
    }  
    if(st == RPN_SUBSCRIPTOR) {
        delete var.getSubscriptor();
        if(var.getVariable() != NULL) {
            delete var.getVariable();
        }
    } else {
        var.getVariable()->killMePlease();
    }
}

Variable *getvar(Lexeme& obj)
{
    LexType t = obj.getType(), st = obj.getSubType();
    if(t != RPN_VAR) {
        throw RBExcept(RBE_EXECUTION_FAILURE);
    }
    if(st == LT_ID) {
        return obj.getIdentifier()->getVariable();
    } else if(st == RPN_SUBSCRIPTOR) {
        Variable *temp = new Variable(*obj.getSubscriptor());
        obj.setVariable(temp);
        return temp;
    } else {
        return obj.getVariable();
    }
}

Lexeme Function::operator()(Lexarr::iterator it, int len)
{
    if(argcount == LIMITLESS_ARGS)
    {
        Lexarr temp;
        for(int i = 0; i < len; i++)
        {
            temp.push_back(*it);
            it++;
        }
        return f(temp);
    }
    if(argcount < len) {
        throw RBExcept(RBE_INVALID_FUNCTION_ARGUMENTS);
    }
    Lexarr temp(argcount, Lexeme());
    int j = 0;
    int i;
    for(int i0 = 0; i0 < len; i0++) {
        i = i0 + j;
        while(i < argcount  &&  temp[i].getType() != LT_UNSPECIFIED) {
            j++;
            i = i0 + j;
        }
        if(i >= argcount) {
            throw RBExcept(RBE_INVALID_FUNCTION_ARGUMENTS);
        }
        if(it->getSubType() == RPN_AASNO) {
            if(nparams.find(it->getIdentifier()->getName()) == nparams.end()) {
                throw RBExcept(RBE_ARGUMENT_ASSIGNMENT_ERROR);
            }
            temp[nparams[it->getIdentifier()->getName()]] = Lexeme(it->getVariable()).toRPN();
        } else {
            temp[i] = *it;
        }
        it++;
    }
    if(defaults.size() != 0) {
        for(int k = 0; k < argcount; k++) {
            if(temp[k].getType() == LT_UNSPECIFIED) {
                temp[k] = Lexeme(new Variable(*defaults[k].getVariable())).toRPN(); //we'll have to generalise that
            }
        }
    }
    return f(temp);
}

#define TYPE_PRIOR(A)  ((A) == VT_NULL ? 0 : \
                        (A) == VT_LOG  ? 1 : \
                        (A) == VT_NUM  ? 2 : \
                        (A) == VT_CHR  ? 3 : -1)
                        
#define NTYPE(A, B) TYPE_PRIOR(A) > TYPE_PRIOR(B) ? (A) : (B)

Lexeme c(Lexarr vars)
{
    ValueType type = VT_NULL;
    std::vector<Value*> temp;
    Variable *current; 
    for(Lexarr::iterator it = vars.begin(); it != vars.end(); it++) {
        current = getvar(*it);
        temp.insert(temp.end(), current->begin(), current->end());
        type = NTYPE(type, current->getType());
        if(it->getSubType() == RPN_SUBSCRIPTOR) {
            delete current;
        }
    } //yeah a totally nescessary copy
    for(std::vector<Value*>::iterator it = temp.begin(); it != temp.end(); it++) {
        *it = (*it)->toType(type);
    }
    return Lexeme(new Variable(temp, type)).toRPN(); //hope this works
}

Lexeme length(Lexarr vars)
{
    if(vars[0].getType() == LT_UNSPECIFIED) {
        throw RBExcept(RBE_INVALID_FUNCTION_ARGUMENTS);
    }
    Variable *temp = getvar(vars[0]);
    int len = temp->getSize();
    if(vars[0].getSubType() == RPN_SUBSCRIPTOR) {
        delete temp;
    }
    return Lexeme(new Variable(new Numeric(len))).toRPN();
}

Lexeme mode(Lexarr vars)
{
    if(vars[0].getType() == LT_UNSPECIFIED) {
        throw RBExcept(RBE_INVALID_FUNCTION_ARGUMENTS);
    }
    Variable *temp = getvar(vars[0]);
    ValueType type = temp->getType();
    if(vars[0].getSubType() == RPN_SUBSCRIPTOR) {
        delete temp;
    }
    switch(type) {
    case VT_CHR:
        return Lexeme(new Variable(new Character("character"))).toRPN();
    case VT_NUM:
        return Lexeme(new Variable(new Character("numeric"))).toRPN();
    case VT_LOG:
        return Lexeme(new Variable(new Character("logical"))).toRPN();
    case VT_NULL:
        return Lexeme(new Variable(new Character("NULL"))).toRPN();
    default:
        throw RBExcept(RBE_INTERNAL_ERROR);
    }
}

Lexeme is_matrix(Lexarr vars)
{
    if(vars[0].getType() == LT_UNSPECIFIED) {
        throw RBExcept(RBE_INVALID_FUNCTION_ARGUMENTS);
    }
    Variable *temp = getvar(vars[0]);
    VariableDim dim = temp->getDimensions();
    if(vars[0].getSubType() == RPN_SUBSCRIPTOR) {
        delete temp;
    }
    return Lexeme(new Variable(new Logical(dim == VD_MATRIX))).toRPN();
}

Lexeme is_vector(Lexarr vars)
{
    if(vars[0].getType() == LT_UNSPECIFIED) {
        throw RBExcept(RBE_INVALID_FUNCTION_ARGUMENTS);
    }
    Variable *temp = getvar(vars[0]);
    VariableDim dim = temp->getDimensions();
    if(vars[0].getSubType() == RPN_SUBSCRIPTOR) {
        delete temp;
    }
    return Lexeme(new Variable(new Logical(dim == VD_VECTOR))).toRPN();
}

Lexeme matrix(Lexarr vars)
{
    Variable *tmp_data = getvar(vars[0]);
    Variable *tmp_nrow = getvar(vars[1]);
    Variable *tmp_ncol = getvar(vars[2]);
    Variable *tmp_byrow = getvar(vars[3]);
    int  nrow  = tmp_nrow->getNum();
    int  ncol  = tmp_ncol->getNum();
    bool byrow = tmp_byrow->isTrue();
    int  len = tmp_data->getSize();
    if(len == 0) {
        throw RBExcept(RBE_UNEXPECTED_EMPTY_VECTOR);
    }
    std::vector<Value*> mtx;
    if(byrow) {
        for(int i = 0; i < nrow * ncol; i++) {
            mtx.push_back((*tmp_data)[i % len]->toType(tmp_data->getType()));
        }
    } else {
        for(int i = 0; i < nrow * ncol; i++) {
            mtx.push_back((*tmp_data)[((i / ncol) + nrow * (i % ncol)) % len]->toType(tmp_data->getType()));
        }
    }
    Variable *temp = new Variable(mtx, tmp_data->getType());
    temp->toMatrix(ncol);
    if(vars[0].getSubType() == RPN_SUBSCRIPTOR) {
        delete tmp_data;
    }
    if(vars[1].getSubType() == RPN_SUBSCRIPTOR) {
        delete tmp_nrow;
    }
    if(vars[2].getSubType() == RPN_SUBSCRIPTOR) {
        delete tmp_ncol;
    }
    if(vars[3].getSubType() == RPN_SUBSCRIPTOR) {
        delete tmp_byrow;
    }
    return Lexeme(temp).toRPN();
}

Lexeme construct_vector(Lexarr vars) //we need to make sure this valid on 1xn matrices
{
    if(vars[0].getType() == LT_UNSPECIFIED  ||
       vars[1].getType() == LT_UNSPECIFIED) {
        throw RBExcept(RBE_INTERNAL_ERROR);
    }
    Variable *temp;
    Lexeme    res;
    if(vars[1].getSubType() != LT_UNSPECIFIED) {
        temp = getvar(vars[1]);
    } else {
        temp = new Variable(new Logical(true));
    }
    if(vars[0].getSubType() == LT_ID) {
        res = Lexeme(new Subscriptor(temp, vars[0].getIdentifier()));
    } else if(vars[0].getSubType() == RPN_SUBSCRIPTOR) {
        res = Lexeme(new Subscriptor(temp, *vars[0].getSubscriptor()));
    } else {
        res = Lexeme(new Subscriptor(temp, vars[0].getVariable()));
    }
    if(vars[1].getSubType() == RPN_SUBSCRIPTOR  ||
       vars[1].getSubType() == LT_UNSPECIFIED) {
        delete temp;
    }
    return res;
}

Lexeme construct_matrix(Lexarr vars) 
{
    if(vars[0].getType() == LT_UNSPECIFIED  ||
       vars[1].getType() == LT_UNSPECIFIED  ||
       vars[2].getType() == LT_UNSPECIFIED) {
        throw RBExcept(RBE_INTERNAL_ERROR);
    }
    Variable *tmp_mtx = getvar(vars[0]);
    if(tmp_mtx->getDimensions() != VD_MATRIX) {
        throw RBExcept(RBE_ILLEGAL_DOUBLE_INDEXING);
    }
    Variable *tmp_row;
    Variable *tmp_col;
    Variable *temp;
    std::vector<Value*> indecies;
    if(vars[1].getSubType() != LT_UNSPECIFIED) {
        temp = getvar(vars[1]);
        tmp_row = toNumIndexing(temp, tmp_mtx->getNumRows());
        if(vars[1].getSubType() == RPN_SUBSCRIPTOR) {
            delete temp;
        }
    } else {
        temp = new Variable(new Logical(true));
        tmp_row = toNumIndexing(temp, tmp_mtx->getNumRows());
        delete temp;
    }
    if(vars[2].getSubType() != LT_UNSPECIFIED) {
        temp = getvar(vars[2]);
        tmp_col = toNumIndexing(temp, tmp_mtx->getNumCols());
        if(vars[2].getSubType() == RPN_SUBSCRIPTOR) {
            delete temp;
        }
    } else {
        temp = new Variable(new Logical(true));
        tmp_col = toNumIndexing(temp, tmp_mtx->getNumCols());
        delete temp;
    }
    int rows = tmp_row->getSize();
    int cols = tmp_col->getSize();
    int mtxrowlen = tmp_mtx->getNumCols();
    Numeric *currow, *curcol;
    temp = new Variable(std::vector<Value*>(rows * cols, NULL), VT_NUM);
    temp->toMatrix(cols);
    for(int i = 0; i < rows; i++) {
        for(int j = 0; j < cols; j++) {
            currow = dynamic_cast<Numeric*>((*tmp_row)[i]);
            curcol = dynamic_cast<Numeric*>((*tmp_col)[j]);
            if(currow->isNULL()  ||  curcol->isNULL()) {
                temp->mtxElem(i, j) = new Numeric();
            }
            temp->mtxElem(i, j) = new Numeric((currow->getValue() - 1) * mtxrowlen + (curcol->getValue() - 1) + 1); 
        }
    }
    Lexeme ret;
    if(vars[0].getSubType() == LT_ID) {
        ret = Lexeme(new Subscriptor(temp, vars[0].getIdentifier()));
    } else if(vars[0].getSubType() == RPN_SUBSCRIPTOR) {
        ret = Lexeme(new Subscriptor(temp, *vars[0].getSubscriptor()));
    } else {
        ret = Lexeme(new Subscriptor(temp, vars[0].getVariable()));
    }
    if(vars[0].getSubType() == RPN_SUBSCRIPTOR) {
        delete tmp_mtx;
    }
    return ret;
}

Lexeme t(Lexarr vars) 
{
    if(vars[0].getType() == LT_UNSPECIFIED) {
        throw RBExcept(RBE_INTERNAL_ERROR);
    }
    Variable *tmp_mtx = getvar(vars[0]);
    Variable *temp;
    if(tmp_mtx->getDimensions() != VD_MATRIX) {
        throw RBExcept(RBE_ILLEGAL_DOUBLE_INDEXING);
    }
    int rows = tmp_mtx->getNumRows();
    int cols = tmp_mtx->getNumCols();
    temp = new Variable(std::vector<Value*>(rows * cols, NULL), tmp_mtx->getType());
    temp->toMatrix(rows);
    for(int i = 0; i < rows; i++) {
        for(int j = 0; j < cols; j++) {
            temp->mtxElem(j, i) = tmp_mtx->mtxElem(i, j)->toType(tmp_mtx->getType());
        }
    }
    if(vars[0].getSubType() == RPN_SUBSCRIPTOR) {
        delete tmp_mtx;
    }
    return Lexeme(temp).toRPN();
}

std::map<std::string, Function> Function::init_builtins()
{
    std::map<std::string, Function> temp;
    
    temp["c"]                 = Function(c);
    
    temp["length"]            = Function(length, 1);
    
    temp["mode"]              = Function(mode, 1);
    
    Lexarr defaults_matrix = {Lexeme(new Variable(new NULL_())).toRPN(), 
                              Lexeme(new Variable(new Numeric(1))).toRPN(),
                              Lexeme(new Variable(new Numeric(1))).toRPN(),
                              Lexeme(new Variable(new Logical(false))).toRPN()};
    std::map<std::string, int> nparams_matrix = 
                             {std::pair<std::string, int>("data",  0),
                              std::pair<std::string, int>("nrow",  1),
                              std::pair<std::string, int>("ncol",  2),
                              std::pair<std::string, int>("byrow", 3)};
    temp["matrix"]            = Function(matrix, 4, nparams_matrix, defaults_matrix);
    
    temp["t"]                 = Function(t, 1);
    
    temp["is.matrix"]         = Function(is_matrix, 1);
    
    temp["is.vector"]         = Function(is_vector, 1);
    
    temp["0construct.vector"] = Function(construct_vector, 2);
    
    temp["0construct.matrix"] = Function(construct_matrix, 3);
    return temp;
}

std::map<std::string, Function> Function::builtins = init_builtins();
