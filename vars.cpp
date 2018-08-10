#include <vars.h>
#include <rbexcept.h>
#include <string>
#include <vector>
#include <iostream>
#include <algorithm>

std::map<std::string, Identifier*> Identifier::scope = {}; //TD

std::vector<Value*>::iterator Variable::begin()
{
    return vals.begin();
}

std::vector<Value*>::iterator Variable::end()
{
    return vals.end();
}
    
Identifier::Identifier(std::string s) 
{
    init = false;
    name = s;
    scope[name] = this;
}

std::string Identifier::getName() const
{
    return name;
}

bool Variable::isTrue() const
{
    if(vals.size() != 1 ||  type != VT_LOG  ||  vals[0]->isNULL()) {
        throw RBExcept(RBE_UNEXPECTED_TYPE_OR_SIZE);
    }
    return dynamic_cast<Logical*>(vals[0])->getValue();
}

float Variable::getNum() const
{
    if(vals.size() != 1 ||  type != VT_NUM  ||  vals[0]->isNULL()) {
        throw RBExcept(RBE_UNEXPECTED_TYPE_OR_SIZE);
    }
    return dynamic_cast<Numeric*>(vals[0])->getValue();
}

ValueType Variable::getType() const 
{
    return type;
}

VariableDim Variable::getDimensions() const
{
    return dim;
}

Variable *Identifier::getVariable()
{
    if(!init) {
        throw RBExcept(RBE_USE_OF_UNINITIALISED_IDENTIFIER, name);
    }
    return var;
}

Value* Numeric::toType(ValueType vtype) const
{
    switch(vtype) {
    case VT_NUM:
        return new Numeric(*this);
    case VT_LOG:
        if(NULLness) {
            return new Logical();
        }
        return value == 0 ? new Logical(false) : new Logical(true);
    case VT_CHR:
        if(NULLness) {
            return new Character();
        }
        return new Character(value == ((float) (int) value) ? std::to_string((int) value) : std::to_string(value));
    case VT_NULL:
        return new NULL_();
    default:
        throw RBExcept(RBE_INTERNAL_ERROR);
    }
}

Value* Logical::toType(ValueType vtype) const
{
    switch(vtype) {
    case VT_NUM:
        if(NULLness) {
            return new Numeric();
        }
        return value ? new Numeric(1) : new Numeric(0);
    case VT_LOG:
        return new Logical(*this);
    case VT_CHR:
        if(NULLness) {
            return new Character();
        }
        return value ? new Character("TRUE") : new Character("FALSE");
    case VT_NULL:
        return new NULL_();
    default:
        throw RBExcept(RBE_INTERNAL_ERROR);
    }
}

Value* Character::toType(ValueType vtype) const
{
    switch(vtype) {
    case VT_NUM:
        throw RBExcept(RBE_ILLEGAL_CONVERSION);
    case VT_LOG:
        throw RBExcept(RBE_ILLEGAL_CONVERSION);
    case VT_CHR:
        return new Character(*this);
    case VT_NULL:
        return new NULL_();
    default:
        throw RBExcept(RBE_INTERNAL_ERROR);
    }
}

Value* NULL_::toType(ValueType vtype) const
{
    switch(vtype) {
    case VT_NUM:
        return new Numeric();
    case VT_LOG:
        return new Logical();
    case VT_CHR:
        return new Character();
    case VT_NULL:
        return new NULL_();
    default:
        throw RBExcept(RBE_INTERNAL_ERROR);
    }
}


Variable* Variable::toType(ValueType vtype) const
{
    if(vals.size() == 0) {
        return new Variable(vtype);
    }
    std::vector<Value*> temp;
    for(std::vector<Value*>::const_iterator it = vals.begin(); it != vals.end(); it++) {
        temp.push_back((*it)->toType(vtype));
    }
    Variable *ret = new Variable(temp, vtype);
    if(dim == VD_MATRIX) {
        ret->toMatrix(rowlen);
    }
    return ret;
}

void Variable::toMatrix(int rl)
{
    if(vals.size() % rl != 0) {
        throw RBExcept(RBE_INVALID_FLAT_MATRIX);
    }
    dim = VD_MATRIX;
    rowlen = rl;
}

int Variable::getNumRows() const
{
    if(dim != VD_MATRIX) {
        throw RBExcept(RBE_INTERNAL_ERROR);
    }
    return vals.size() / rowlen;
}

int Variable::getNumCols() const
{
    if(dim != VD_MATRIX) {
        throw RBExcept(RBE_INTERNAL_ERROR);
    }
    return rowlen;
}

Value *Variable::operator[](int i) const
{
    return vals[i];
}

Value*& Variable::mtxElem(int i, int j) //row, col
{
    if(dim != VD_MATRIX) {
        throw RBExcept(RBE_INTERNAL_ERROR);
    }
    return vals[rowlen * i + j];
}

bool Value::isNULL() const
{
    return NULLness;
}

int Variable::getSize() const 
{
    return vals.size();
}

float Numeric::getValue() const
{
    return value;
}

std::string Character::getValue() const
{
    return value;
}

bool Logical::getValue() const
{
    return value;
}
    
template<ValueType X, class Y, class Z, ValueType RET, typename F>
Variable *zip_operator(const Variable& lhs, const Variable& rhs, F f)
{
    Variable *clhs = lhs.toType(X);
    Variable *crhs = rhs.toType(X);
    int lsize = clhs->getSize();
    int rsize = crhs->getSize();
    if(lsize == 0 || rsize == 0) {
        delete clhs;
        delete crhs;
        return new Variable(RET);
    }
    int len = lsize > rsize ? lsize : rsize;
    std::vector<Value*>::iterator lit = clhs->vals.begin();
    std::vector<Value*>::iterator rit = crhs->vals.begin();
    std::vector<Value*> temp;
    Z *lval, *rval;
    bool lnull, rnull;
    Y l, r;
    for(int i = 0; i < len; i++) {
        lval = dynamic_cast<Z*>(*lit);
        rval = dynamic_cast<Z*>(*rit);
        l = lval->getValue();
        r = rval->getValue();
        lnull = lval->isNULL();
        rnull = rval->isNULL();
        temp.push_back(f(l, r, lnull, rnull));
        lit++;
        rit++;
        if(lit == clhs->vals.end()) {
            lit = clhs->vals.begin();
        }
        if(rit == crhs->vals.end()) {
            rit = crhs->vals.begin();
        }
    }
    delete clhs;
    delete crhs;
    Variable *ret = new Variable(temp, RET);
    if(len == lsize) {
        if(lhs.getDimensions() == VD_MATRIX) {
            ret->toMatrix(lhs.getNumCols());
        }
    } else {
        if(rhs.getDimensions() == VD_MATRIX) {
            ret->toMatrix(rhs.getNumCols());
        }
    }
    return ret;
}

template<ValueType X, class Y, class Z, ValueType RET, typename F>
Variable *apply_operator(const Variable& rhs, F f)
{
    Variable *crhs = rhs.toType(X);
    if(crhs->getSize() == 0) {
        delete crhs;
        return new Variable(RET);
    }
    std::vector<Value*> temp;
    Z *rval;
    bool rnull;
    Y r;
    for(std::vector<Value*>::iterator rit = crhs->vals.begin(); rit != crhs->vals.end(); rit++) {
        rval = dynamic_cast<Z*>(*rit);
        r = rval->getValue();
        rnull = rval->isNULL();
        temp.push_back(f(r, rnull));
    }
    delete crhs;
    Variable *ret = new Variable(temp, RET);
    if(rhs.getDimensions() == VD_MATRIX) {
        ret->toMatrix(rhs.getNumCols());
    }
    return ret;
}

Variable* operator&(const Variable& lhs, const Variable& rhs)
{
    return zip_operator<VT_LOG, bool, Logical, VT_LOG>
    (lhs, rhs, [] (bool l, bool r, bool lnull, bool rnull) 
    { 
        if(lnull) {
            if(rnull) {
                return new Logical();
            } else {
                return r ? new Logical() : new Logical(false);
            }
        } else if(rnull) {
            return l ? new Logical() : new Logical(false);
        } else {
            return new Logical(l && r);
        }
    });
}

Variable* operator|(const Variable& lhs, const Variable& rhs)
{
    return zip_operator<VT_LOG, bool, Logical, VT_LOG>
    (lhs, rhs, [] (bool l, bool r, bool lnull, bool rnull) 
    { 
        if(lnull) {
            if(rnull) {
                return new Logical();
            } else {
                return r ? new Logical(true) : new Logical();
            }
        } else if(rnull) {
            return l ? new Logical(true) : new Logical();
        } else {
            return new Logical(l || r);
        }
    });
}

Variable* operator!(const Variable& rhs)
{
    return apply_operator<VT_LOG, bool, Logical, VT_LOG>
    (rhs, [] (bool r, bool rnull) 
    { 
        return rnull ? new Logical() : new Logical(!r);
    });
}

#define COMPARE(OP)                                                             \
    if(lhs.type == VT_CHR  ||  rhs.type == VT_CHR)                              \
    {                                                                           \
        return zip_operator<VT_CHR, std::string, Character, VT_LOG>             \
        (lhs, rhs, [] (std::string& l, std::string& r, bool lnull, bool rnull)  \
        {                                                                       \
            return lnull || rnull ? new Logical() : new Logical(l OP r);        \
        });                                                                     \
    } else {                                                                    \
        return zip_operator<VT_NUM, float, Numeric, VT_LOG>                     \
        (lhs, rhs, [] (float l, float r, bool lnull, bool rnull)                \
        {                                                                       \
            return lnull || rnull ? new Logical() : new Logical(l OP r);        \
        });                                                                     \
    } 

Variable* operator>(const Variable& lhs, const Variable& rhs)
{
    COMPARE(>)
}

Variable* operator<(const Variable& lhs, const Variable& rhs)
{
    COMPARE(<)
}

Variable* operator>=(const Variable& lhs, const Variable& rhs)
{
    COMPARE(>=)
}

Variable* operator<=(const Variable& lhs, const Variable& rhs)
{
    COMPARE(<=)
}

Variable* operator==(const Variable& lhs, const Variable& rhs)
{
    COMPARE(==)
}

Variable* operator!=(const Variable& lhs, const Variable& rhs)
{
    COMPARE(!=)
}

#define ARITHMETIC(OP)                                                  \
    return zip_operator<VT_NUM, float, Numeric, VT_NUM>                 \
    (lhs, rhs, [] (float l, float r, bool lnull, bool rnull)            \
    {                                                                   \
        return lnull || rnull ? new Numeric() : new Numeric(l OP r);    \
    });

Variable* operator+(const Variable& lhs, const Variable& rhs)
{
    ARITHMETIC(+)
}

Variable* operator+(const Variable& rhs)
{
    return rhs.toType(VT_NUM);
}

Variable* operator-(const Variable& lhs, const Variable& rhs)
{
    ARITHMETIC(-)
}

Variable* operator-(const Variable& rhs)
{
    return apply_operator<VT_NUM, float, Numeric, VT_NUM>
    (rhs, [] (float r, bool rnull) 
    { 
        return rnull ? new Numeric() : new Numeric(-r);
    });
}

Variable* operator*(const Variable& lhs, const Variable& rhs)
{
    ARITHMETIC(*)
}

Variable* operator/(const Variable& lhs, const Variable& rhs)
{
    ARITHMETIC(/)
}

Variable* operator^(const Variable& lhs, const Variable& rhs) //colon operator
{
    Variable *clhs = lhs.toType(VT_NUM);
    Variable *crhs = rhs.toType(VT_NUM);
    if(clhs->getSize() == 0  ||  crhs->getSize() == 0) {
        delete clhs;
        delete crhs;
        throw RBExcept(RBE_UNEXPECTED_EMPTY_VECTOR);
    }
    Numeric *fclhs = dynamic_cast<Numeric*>(clhs->vals[0]);
    Numeric *fcrhs = dynamic_cast<Numeric*>(crhs->vals[0]);
    if(fclhs->isNULL()  ||  fcrhs->isNULL()) {
        delete clhs;
        delete crhs;
        throw RBExcept(RBE_UNEXPECTED_NULL);
    }
    float l = fclhs->getValue();
    float r = fcrhs->getValue();
    std::vector<Value*> temp;
    for(float i = l; i <= r; i = i + 1) {
        temp.push_back(new Numeric(i));
    }
    delete clhs;
    delete crhs;
    return new Variable(temp, VT_NUM);
}

Variable* operator&&(const Variable& lhs, const Variable& rhs)
{
    if(lhs.getDimensions() != VD_MATRIX) {
        throw RBExcept(RBE_ILLEGAL_MATRIX_PRODUCT);
    }
    if(rhs.getDimensions() != VD_MATRIX) {
        throw RBExcept(RBE_ILLEGAL_MATRIX_PRODUCT);
    }
    if(lhs.getNumCols() != rhs.getNumRows()) {
        throw RBExcept(RBE_ILLEGAL_MATRIX_PRODUCT);
    }
    Variable *clhs = lhs.toType(VT_NUM);
    Variable *crhs = rhs.toType(VT_NUM);
    int rows = lhs.getNumRows();
    int cols = rhs.getNumCols();
    int prod = lhs.getNumCols();
    Variable *temp;
    float current;
    temp = new Variable(std::vector<Value*>(rows * cols, NULL), VT_NUM);
    temp->toMatrix(cols);
    Value *a, *b;
    for(int i = 0; i < rows; i++) {
        for(int j = 0; j < cols; j++) {
            current = 0;
            for(int k = 0; k < prod; k++) {
                a = clhs->mtxElem(i, k);
                b = crhs->mtxElem(k, j);
                if(a->isNULL() || b->isNULL()) {
                    temp->mtxElem(i, j) = new Numeric();
                    break;
                }
                current += dynamic_cast<Numeric*>(a)->getValue() *
                           dynamic_cast<Numeric*>(b)->getValue();
            }
            if(temp->mtxElem(i, j) == NULL) {
                temp->mtxElem(i, j) = new Numeric(current);
            }
        }
    }
    delete clhs;
    delete crhs;
    return temp;
}

void Variable::onNewSubscriptor()
{
    subscripted++;
}

void Variable::onDeadSubscriptor()
{
    subscripted--;
    if(tobedel && subscripted <= 0) {
        delete this;
    }
}

std::vector<Variable*> Variable::delayed = {};

void Variable::killMePlease()
{
    if(!deletable) {
        if(std::find(delayed.begin(), delayed.end(), this) == delayed.end()) {
            delayed.push_back(this);
        }
        return;
    }
    tobedel = true;
    if(subscripted <= 0) {
        delete this;
    }
}

void Variable::clearHeap()
{
    while(delayed.size() != 0) {
        delete delayed.back();
        delayed.pop_back();
    }
}
    
Variable* toNumIndexing(const Variable *indecies, int sz) 
{
    ValueType t = indecies->getType(); 
    if(t == VT_NUM  ||  indecies->getSize() == 0) {
        return indecies->toType(VT_NUM);
    } else if(t == VT_NULL) {
        std::vector<Value*> temp;
        int len = sz > indecies->getSize() ? sz : indecies->getSize();
        for(int i = 0; i < len; i++) {
            temp.push_back(new Numeric());
        }
        return new Variable(temp, VT_NUM);
    } else if(t == VT_LOG) {
        std::vector<Value*> temp;
        int len = sz > indecies->getSize() ? sz : indecies->getSize();
        std::vector<Value*>::const_iterator it = indecies->vals.begin();
        for(int i = 0; i < len; i++) {
            if((*it)->isNULL()) {
                temp.push_back(new Numeric());
            } else if(dynamic_cast<Logical*>(*it)->getValue()) {
                temp.push_back(new Numeric(i + 1));
            }
            it++;
            if(it == indecies->vals.end()) {
                it = indecies->vals.begin();
            }
        }
        return new Variable(temp, VT_NUM);
    } else {
        throw RBExcept(RBE_ILLEGAL_SUBSCRIPTION);
    }
}

bool containsNULL(const Variable* v)
{
    for(std::vector<Value*>::const_iterator it = v->vals.begin(); it != v->vals.end(); it++)
    {
        if((*it)->isNULL()) {
            return true;
        } else if(dynamic_cast<Numeric*>(*it)->getValue() < 1) { //assuming v is a an index vector
            throw RBExcept(RBE_INVALID_INDEX);
        }
    }
    return false;
}

Subscriptor::Subscriptor(const Variable *ind, Identifier *id) //check indecies for null
{
    if(!id->init) {
        throw RBExcept(RBE_USE_OF_UNINITIALISED_IDENTIFIER, id->name);
    }
    indecies   = toNumIndexing(ind, id->var->getSize());
    if(ind->getDimensions() == VD_MATRIX) {
        indecies->toMatrix(ind->rowlen);
    }
    assignable = !containsNULL(indecies);
    sstbi      = id;
    sstb       = NULL;
}

Subscriptor::Subscriptor(const Variable *ind, Variable *var)
{
    indecies   = toNumIndexing(ind, var->getSize());
    if(ind->getDimensions() == VD_MATRIX) {
        indecies->toMatrix(ind->rowlen);
    }
    containsNULL(indecies); //check for invalid indecies
    assignable = false;
    sstbi      = NULL;
    sstb       = var;
    sstb->onNewSubscriptor();
}

Subscriptor::Subscriptor(const Variable *ind, Subscriptor& other)
{
    sstb  = other.sstb;
    sstbi = other.sstbi;
    Variable *cind = toNumIndexing(ind, other.indecies->getSize());
    std::vector<Value*> temp;
    int curindex;
    Numeric *current;
    for(int i = 0; i < cind->getSize(); i++) {
        current = dynamic_cast<Numeric*>(cind->vals[i]);
        if(current->isNULL()) {
            temp.push_back(new Numeric());
            continue;
        }
        curindex = current->getValue() - 1;
        if(curindex >= other.indecies->getSize()) {
            temp.push_back(new Numeric());
        } else {
            temp.push_back(new Numeric(*dynamic_cast<Numeric*>(other.indecies->vals[curindex])));
        }
    }
    indecies   = new Variable(temp, VT_NUM);
    if(ind->getDimensions() == VD_MATRIX) {
        indecies->toMatrix(ind->rowlen);
    }
    assignable = !containsNULL(indecies)  &&  other.assignable;
    if(sstbi == NULL) {
        sstb->onNewSubscriptor();
    }
}

void Identifier::operator=(const Variable& other)
{
    if(init) {
        delete var;
    }
    var  = other.toType(other.type);
    init = true;
}

void Identifier::operator=(const Identifier& other)
{
    if(other.init) {
        *this = *other.var;
    } else {
        throw RBExcept(RBE_USE_OF_UNINITIALISED_IDENTIFIER, name);
    }
}

void Subscriptor::operator=(const Variable& rhs)
{
    if(!assignable) {
        throw RBExcept(RBE_ILLEGAL_ASSIGNMENT);
    }
    if(indecies->getSize() < rhs.getSize()) {
        throw RBExcept(RBE_ILLEGAL_ASSIGNMENT);
    }
    const Variable *crhs;
    if(rhs.type == VT_CHR) {
        sstbi->var = sstbi->var->toType(VT_CHR);
        crhs = &rhs;
    } else {
        crhs = rhs.toType(sstbi->var->type);
    }
    int ind;
    int len = indecies->getSize();
    int rhslen = crhs->getSize();
    for(int i = 0; i < len; i++) {
        ind = dynamic_cast<Numeric*>(indecies->vals[i])->getValue() - 1;
        while(ind >= sstbi->var->getSize()) {
            sstbi->var->vals.push_back(NULL_().toType(crhs->getType()));
        }
        delete sstbi->var->vals[ind];
        sstbi->var->vals[ind] = crhs->vals[i % rhslen]->toType(crhs->getType());
    }
    if(crhs != &rhs) {
        delete crhs;
    }
}

Variable::Variable(const Subscriptor& sub)
{
    vals = std::vector<Value*>();
    int curindex;
    Numeric *temp;
    Variable *var = sub.sstbi == NULL ? sub.sstb : sub.sstbi->getVariable();
    for(int i = 0; i < sub.indecies->getSize(); i++) {
        temp = dynamic_cast<Numeric*>(sub.indecies->vals[i]);
        if(temp->isNULL()) {
            vals.push_back(NULL_().toType(var->type));
            continue;
        }
        curindex = temp->getValue() - 1;
        if(curindex >= var->getSize()) {
            vals.push_back(NULL_().toType(var->type));
        } else {
            vals.push_back(var->vals[curindex]->toType(var->type));
        }
    }
    type = var->type;
    subscripted = 0;
    dim = VD_VECTOR;
    tobedel = false;
    deletable = true;
    if(sub.indecies->getDimensions() == VD_MATRIX) {
        toMatrix(sub.indecies->rowlen);
    }
}

Subscriptor::~Subscriptor()
{
    delete indecies;
    if(sstbi == NULL) {
        sstb->onDeadSubscriptor();
    }
}

std::string Variable::str() const
{
    if(vals.size() == 0) {
        if(type == VT_NUM) {
            return "Numeric(0)";
        } else if(type == VT_LOG) {
            return "Logical(0)";
        } else if(type == VT_CHR) {
            return "Character(0)";
        } else {
            return "NULL(0)";
        }
    }
    std::string temp = "";
    Character *cel;
    int i = 0;
    for(std::vector<Value*>::const_iterator it = vals.begin(); it != vals.end(); it++)
    {
        if((*it)->isNULL()) {
            temp += "NULL ";
        } else {
            cel = dynamic_cast<Character*>((*it)->toType(VT_CHR));
            if(type != VT_CHR) {
                temp += cel->getValue() + " ";
            } else {
                temp += "\"" + cel->getValue() + "\" ";
            }
            delete cel;
        }
        i++;
        if(dim == VD_MATRIX  &&  i % rowlen == 0) {
            temp.pop_back();
            temp += "\n";
        }
    }
    temp.pop_back();
    return temp;
}
