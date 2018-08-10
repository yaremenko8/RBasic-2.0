#ifndef __RBEXCEPT_H__
#define __RBEXCEPT_H__
#include <string>
enum RBasicExceptionType
{
    RBE_INVALID_NUMERIC_TOKEN,
    RBE_INVALID_CHARACTER_TOKEN,
    RBE_INVALID_INDEX,
    RBE_UNRECOGNISED_ESCAPE_SEQUENCE,
    RBE_UNEXPECTED_INPUT,
    RBE_UNDEFINED_OP_OVERLOAD,
    RBE_PREMATURE_EOF,
    RBE_SYNTAX_ERROR,
    RBE_INCOMPLETE_EXPRESSION,
    RBE_MISMATCHED_PARANTHESES,
    RBE_INTERNAL_ERROR,
    RBE_EXECUTION_FAILURE,
    RBE_ILLEGAL_ASSIGNMENT,
    RBE_ILLEGAL_CONVERSION,
    RBE_ILLEGAL_SUBSCRIPTION,
    RBE_ILLEGAL_DOUBLE_INDEXING,
    RBE_UNEXPECTED_EMPTY_VECTOR,
    RBE_ARGUMENT_ASSIGNMENT_ERROR,
    RBE_USE_OF_UNINITIALISED_IDENTIFIER,
    RBE_UNEXPECTED_NULL,
    RBE_INVALID_FUNCTION_ARGUMENTS,
    RBE_INVALID_FLAT_MATRIX,
    RBE_UNEXPECTED_TYPE_OR_SIZE,
    RBE_ILLEGAL_MATRIX_PRODUCT,
};

class RBExcept
{
private:
    RBasicExceptionType type;
    std::string context;
public:
    RBExcept(RBasicExceptionType t, std::string cont = "") : type(t), context(cont) {}
    RBExcept(const RBExcept& other) : type(other.type), context(other.context) {}
    RBasicExceptionType getType() const;
    std::string getContext() const;
};

void rbperror(const RBExcept&);

#endif