#ifndef __VARS_H__
#define __VARS_H__
#include <map>
#include <string>
#include <utility>
#include <vector>
enum ValueType
{
    VT_NUM,
    VT_CHR,
    VT_LOG,
    VT_NULL,
};

enum VariableDim
{
    VD_VECTOR,
    VD_MATRIX,
};

class Numeric;
class Logical;
class Character;
class NULL_;

class Value
{
protected:
    bool NULLness;
public:
    virtual Value* toType(ValueType) const = 0;
    bool isNULL() const;
    virtual ~Value() = default;
};

class Numeric : public Value
{
private:
    float value;
public:
    float getValue() const;
    Value* toType(ValueType) const;
    Numeric() {NULLness = true;}
    Numeric(const Numeric& other) :
        value(other.value) 
        {NULLness = other.NULLness;}
    Numeric(float n) : value(n) {NULLness = false;}
};

class Logical : public Value
{
private:
    bool value;
public:
    bool getValue() const;
    Value* toType(ValueType) const;
    Logical() {NULLness = true;}
    Logical(const Logical& other) :
        value(other.value)
        {NULLness = other.NULLness;}
    Logical(bool v) : value(v) {NULLness = false;}
};

class Character : public Value
{
private:
    std::string value;
public:
    std::string getValue() const;
    Value* toType(ValueType) const;
    Character() {NULLness = true;}
    Character(const Character& other) :
        value(other.value) 
        {NULLness = other.NULLness;}
    Character(std::string s) : value(s) {NULLness = false;}
};
 
class NULL_ : public Value
{
public:
    Value* toType(ValueType) const;
    NULL_() {NULLness = true;}
    NULL_(const Numeric& other) {NULLness = true;}
};

class Variable;
class Identifier;
class Subscriptor;

class Variable
{
private:
    ValueType           type;
    VariableDim         dim;         //matrix or vector? Perhaps there is a beter way to keep track of this
    int                 subscripted;
    bool                tobedel;
    std::vector<Value*>   vals;
    int rowlen;
    bool deletable;
    static std::vector<Variable*> delayed;
    void onMemberChange(int);
public:
    static void clearHeap();
    std::string str() const;
    Variable(const std::vector<Value*>& varr, ValueType vt) : //the values have to be converted to the according type
        type(vt),
        dim(VD_VECTOR),
        subscripted(0),
        tobedel(false),
        vals(varr),
        deletable(true) {}
    Variable(Numeric* v, bool del = true) : 
        type(VT_NUM), 
        dim(VD_VECTOR), 
        subscripted(0),
        tobedel(false),
        vals(std::vector<Value*>(1, v)),
        deletable(del) {}
    Variable(Character* v, bool del = true) : 
        type(VT_CHR), 
        dim(VD_VECTOR), 
        subscripted(0),
        tobedel(false),
        vals(std::vector<Value*>(1, v)),
        deletable(del) {}
    Variable(Logical* v, bool del = true) : 
        type(VT_LOG), 
        dim(VD_VECTOR), 
        subscripted(0),
        tobedel(false),
        vals(std::vector<Value*>(1, v)),
        deletable(del) {}
    Variable(NULL_* v, bool del = true) : 
        type(VT_NULL), 
        dim(VD_VECTOR), 
        subscripted(0),
        tobedel(false),
        vals(std::vector<Value*>(1, v)),
        deletable(del) {}
     Variable(ValueType v) : 
        type(v), 
        dim(VD_VECTOR), 
        subscripted(0),
        tobedel(false),
        vals(std::vector<Value*>()),
        deletable(true) {}
    Variable(const Subscriptor& sub);
    Variable(const Variable& other) 
        {
            type = other.type;
            dim = other.dim; 
            subscripted = 0;
            tobedel = false;
            rowlen = other.rowlen;
            vals = std::vector<Value*>();
            for(std::vector<Value*>::const_iterator it = other.vals.begin(); it != other.vals.end(); it++) {
                vals.push_back((*it)->toType(type));
            }
            deletable = true;
        }
    Variable(const Identifier& id);
    ~Variable()// when a variable does get deleted it also deletes it's membership
    {
        for(std::vector<Value*>::iterator it = vals.begin(); it != vals.end(); it++) {
            delete *it;
        }
    }
    int getSize() const;
    bool isTrue() const;
    float getNum() const;
    ValueType getType() const;
    VariableDim getDimensions() const;
    bool isExpandable() const;
    bool isAssignable() const;
    bool isTemporary() const;
    void onNewSubscriptor();
    void onDeadSubscriptor();
    void killMePlease();
    std::vector<Value*>::iterator begin();
    std::vector<Value*>::iterator end();
    Variable* toType(ValueType) const;
    void toMatrix(int rl); //we need a "flatten" method aswell
    Value *operator[](int i) const;
    Value*& mtxElem(int i, int j);
    int getNumRows() const;
    int getNumCols() const;
    friend Variable* operator&(const Variable&, const Variable&);
    friend Variable* operator|(const Variable&, const Variable&);
    friend Variable* operator!(const Variable&);
    friend Variable* operator>(const Variable&, const Variable&);
    friend Variable* operator<(const Variable&, const Variable&);
    friend Variable* operator>=(const Variable&, const Variable&);
    friend Variable* operator<=(const Variable&, const Variable&);
    friend Variable* operator==(const Variable&, const Variable&);
    friend Variable* operator!=(const Variable&, const Variable&);
    friend Variable* operator+(const Variable&, const Variable&);
    friend Variable* operator+(const Variable&);
    friend Variable* operator-(const Variable&, const Variable&);
    friend Variable* operator-(const Variable&);
    friend Variable* operator*(const Variable&, const Variable&);
    friend Variable* operator/(const Variable&, const Variable&);
    friend Variable* operator^(const Variable&, const Variable&);
    friend Variable* operator&&(const Variable& lhs, const Variable& rhs);
    friend Variable* toNumIndexing(const Variable *indecies, int sz);
    friend bool containsNULL(const Variable* v);
    template<ValueType X, class Y, class Z, ValueType RET, typename F>
    friend Variable *zip_operator(const Variable& lhs, const Variable& rhs, F f);
    template<ValueType X, class Y, class Z, ValueType RET, typename F>
    friend Variable *apply_operator(const Variable& rhs, F f);
    friend class Identifier;
    friend class Subscriptor;
};

class Identifier
{
private:
    std::string name;        //not sure if we even need this
    bool        init;        //true if identifier is initialised
    Variable   *var;         //variables referenced by an identifier are assignable and expandable
public:
    std::string getName() const;
    Variable *getVariable();
    static std::map<std::string, Identifier*> scope;
    Identifier(std::string); //adds itself to the scope when cosntructed
    void operator=(const Variable&);
    void operator=(const Identifier&);
    friend class Subscriptor;
};

class Subscriptor
{
private:
    Variable   *indecies;
    Variable   *sstb;
    Identifier *sstbi;
    bool assignable;
public:
    Subscriptor(const Variable *indecies, Identifier *sstb);
    Subscriptor(const Variable *indecies, Variable *sstb);
    Subscriptor(const Variable *indecies, Subscriptor& other);
    friend class Variable;
    friend class Identifier;
    void operator=(const Variable&);
    ~Subscriptor();
};

#endif