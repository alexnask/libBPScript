#ifndef EXCEPTION__H
#define EXCEPTION__H
#include <string>
#include <sstream>
#include <exception>

namespace BPS {

class Exception : public std::exception {
    private:
        std::string s;
    public:
        Exception(std::string,unsigned int line);
        ~Exception() throw();
        const char* what() const throw();
};


}
#endif//EXCEPTION__H
