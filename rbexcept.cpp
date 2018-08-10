#include <rbexcept.h>
#include <iostream>

RBasicExceptionType RBExcept::getType() const
{
    return type;
}

std::string RBExcept::getContext() const
{
    return context;
}

void rbperror(const RBExcept& e)
{
    switch(e.getType()) {
    case RBE_INVALID_NUMERIC_TOKEN:
        std::cerr << "Error: invalid numeric token in \'" << e.getContext() << "\'\n";
        break;
    case RBE_INVALID_CHARACTER_TOKEN:
        std::cerr << "Error: invalid character token\n";
        break;
    case RBE_UNRECOGNISED_ESCAPE_SEQUENCE:
        std::cerr << "Error: unrecognised escape sequence in \'" << e.getContext() << "\'\n";
        break;
    case RBE_UNEXPECTED_INPUT:
        std::cerr << "Error: unexpected input in \'" << e.getContext() << "\'\n";
        break;
    case RBE_PREMATURE_EOF:
        std::cerr << "Error: premature end of input\n";
        break;
    case RBE_SYNTAX_ERROR:
        std::cerr << "Syntax error";
        if(e.getContext() == "") {
            std::cerr << ".\n";
        } else {
            std::cerr << ": " << e.getContext() << std::endl;
        }
        break;
    case RBE_INCOMPLETE_EXPRESSION:
        std::cerr << "Error: incomplete expression\n";
        break;
    case RBE_MISMATCHED_PARANTHESES:
        std::cerr << "Error: missmatched parantheses\n";
        break;
    case RBE_INTERNAL_ERROR:
        std::cerr << "Error: internal error\n";
        break;
    case RBE_INVALID_INDEX:
        std::cerr << "Error: non-positive indecies are not allowed\n";
        break;
    case RBE_UNDEFINED_OP_OVERLOAD:
        std::cerr << "Error: incorrect number of parameters provided for an operator\n";
        break;
    case RBE_EXECUTION_FAILURE:
        std::cerr << "Error: stack anomaly occured at runtime\n";
        break;
    case RBE_ILLEGAL_ASSIGNMENT:
        std::cerr << "Error: illegal assignment\n";
        break;
    case RBE_ILLEGAL_CONVERSION:
        std::cerr << "Error: could not perform implicit type conversion\n";
        break;
    case RBE_ILLEGAL_SUBSCRIPTION:
        std::cerr << "Error: invalid subscriptor provided\n";
        break;
    case RBE_UNEXPECTED_EMPTY_VECTOR:
        std::cerr << "Error: unexpected empty vector\n";
        break;
    case RBE_ARGUMENT_ASSIGNMENT_ERROR:
        std::cerr << "Error: argument assignment failed\n";
        break;
    case RBE_USE_OF_UNINITIALISED_IDENTIFIER:
        std::cerr << "Error: \'" + e.getContext() + "\' is not initialised\n";
        break;
    case RBE_UNEXPECTED_NULL:
        std::cerr << "Error: unexpected NULL\n";
        break;
    case RBE_INVALID_FUNCTION_ARGUMENTS:
        std::cerr << "Error: invalid function arguments\n";
        break;
    case RBE_INVALID_FLAT_MATRIX:
        std::cerr << "Error: failed to transform vector to matrix\n";
        break;
    case RBE_UNEXPECTED_TYPE_OR_SIZE:
        std::cerr << "Error: unsuitable vector provided\n";
        break;
    case RBE_ILLEGAL_DOUBLE_INDEXING:
        std::cerr << "Error: subscripted object is not a matrix\n";
        break;
    case RBE_ILLEGAL_MATRIX_PRODUCT:
        std::cerr << "Error: illegal matrix product\n";
        break;
    default:
        std::cerr << "Error: unknown exception\n";
    }
}
