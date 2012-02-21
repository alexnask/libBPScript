#include "Exception.h"
using namespace BPS;

Exception::Exception(std::string _s, unsigned int line)  {
    std::stringstream out;
    out << line;
    s = "[ERROR](Line " + out.str() + "): " + _s;
}
Exception::~Exception() throw() {}

const char* Exception::what() const throw() {
    return s.c_str();
}
