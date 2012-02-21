#ifndef VALUE__H
#define VALUE__H
#include <vector>
#include <map>
#include <string>
#include <cctype>
#include <cstdlib>
#include <sstream>

namespace BPS {

enum ValueType {
    NUMBER,
    SYMBOL,
    LIST
};

class Value {
    public:
        Value(unsigned int,ValueType);
        virtual std::string toString() = 0;
        virtual Value* clone() const = 0;
        ValueType getType() {
            return type;
        }
        unsigned int getLine() const {
            return line;
        }
    protected:
        ValueType type;
        unsigned int line;
};

class Number : public Value {
    public:
        double data;
        
        Number(double,unsigned int);
        virtual std::string toString();
        virtual Number* clone() const {return(new Number(*this));};
};

class Symbol : public Value {
    public:
        static std::vector<std::string> reserved;
        std::string data;
        
        Symbol(std::string,unsigned int);
        virtual std::string toString();
        virtual Symbol* clone() const {return(new Symbol(*this));};
};

class List : public Value {
    public:
        std::vector<Value*> data;
        
        List(std::vector<Value*>,unsigned int);
        List(const List&);
        List(unsigned int);
        
        virtual List* clone() const {return(new List(*this));};
        ~List();
        virtual std::string toString();
};

}
#endif//VALUE__H
