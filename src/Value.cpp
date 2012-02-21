#include "Value.h"
#include <iostream>
using namespace BPS;

Value::Value(unsigned int _line, ValueType _type) : type(_type), line(_line) {}

Number::Number(double _data, unsigned int _line) : Value(_line,NUMBER), data(_data) {}
std::string Number::toString() {
    std::ostringstream strs;
    strs << data;
    return strs.str();
}

std::vector<std::string> Symbol::reserved;

Symbol::Symbol(std::string _data, unsigned int _line) : Value(_line,SYMBOL), data(_data) {}
std::string Symbol::toString() {
    return data;
}

List::List(std::vector<Value*> _data, unsigned int _line) : Value(_line,LIST), data(_data) {}
List::List(unsigned int _line) : Value(_line,LIST), data() {}
List::List(const List& original) : Value(original), data(original.data) {
    std::vector<Value*>::const_iterator o = original.data.begin();
    for(std::vector<Value*>::iterator i = data.begin(); i != data.end(); ++i,++o) {
        (*i)= (*o)->clone();
    }
    line = original.getLine();
}
List::~List() {
    for(unsigned int i = 0; i < data.size(); i++) {
        delete data[i];
        data[i] = NULL;
    }
}
std::string List::toString() {
    std::string str = "(";
    for(unsigned int i = 0; i < data.size(); i++) {
        str += data[i]->toString();
        if(i != data.size()-1) str += " ";
    }
    str += ")";
    return str;
}

