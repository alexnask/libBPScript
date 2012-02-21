#include "Parser.h"
using namespace BPS;

Parser::Parser(std::string _data) : data(_data), currIndex(0) {}

void Parser::skipWhitespace() {
    while(currIndex < data.size() && isspace(data[currIndex])) {
        currIndex ++;
    }
}

void Parser::skipLine() {
    while(currIndex < data.size() && data[currIndex] != '\n' && data[currIndex] != '\r') {
        currIndex++;
    }
    currIndex++;
}

std::string Parser::readTillWhitespace() {
    std::string ret;
    while(currIndex < data.size() && !isspace(data[currIndex]) && data[currIndex] != ')') {
        ret += data[currIndex];
        currIndex++;
    } 
    return ret;
}

unsigned int Parser::line() {
    unsigned int lines = 1;
    for(unsigned int i = 0; i < currIndex; i++) {
        if(data[i] == '\n' || data[i] == '\r') lines ++;
    }
    return lines;
}

List* Parser::run() {
    std::vector<Value*> values;
    while(currIndex < data.size()) {
        Value* value = read();
        if(value != NULL) {
            values.push_back(value);
        }
    }
    return new List(values,0);
}

Value* Parser::read() {
    skipWhitespace();
    
    if(currIndex >= data.size()) return NULL;
    char next = data[currIndex];
    if(next == '\0') {
        currIndex = data.size();
        return NULL;
    } else if(next >= '0' && next <= '9' || next == '.') {
        return readNumber();
    } else if(next == '-' && currIndex <= data.size()) {
        char anotherNext = data[currIndex+1];
        if(anotherNext >= '0' && anotherNext <= '9' || anotherNext == '.') {
            return readNumber();
        } else {
            return readSymbol();
        }
    } else {
        switch(next) {
            case ')':
                // TODO: Error
                return NULL;
                break;
            case '(':
                return readList();
                break;
            case ';':
                skipLine();
                return NULL;
                break;
            default:
                return readSymbol();
                break;
        }
    }
    return NULL;
}

Number* Parser::readNumber() {
    std::string str = readTillWhitespace();
    bool negative = false;
    if(str[0] == '-') {
        negative = true;
        str = str.substr(1);
    }
    
    bool dot = false;
    for(unsigned int i = 0; i < str.size(); i++) {
        if(!isdigit(str[i]) && str[i] != '.') {
            //TODO: Error
            return NULL;
        } else if(str[i] == '.' && !dot) {
            dot = true;
        } else if(str[i] == '.' && dot) {
            //TODO: Error
            return NULL;
        }
    }
    
    double n = ::atof(str.c_str());
    if(negative) n *= -1;
    return new Number(n,line());
}

Symbol* Parser::readSymbol() {
    return new Symbol(readTillWhitespace(),line());
}

List* Parser::readList() {
    currIndex ++;
    skipWhitespace();
    unsigned int nline = line();
    if(currIndex >= data.size()) return NULL; // TODO: Error
    if(data[currIndex] == ')') {
        currIndex++;
        return new List(nline);
    }
    Value* first = read();
    skipWhitespace();
    std::vector<Value*> list;
    list.push_back(first);
    while(true) {
        if(currIndex+1 >= data.size() && data[currIndex] != ')') {
            // TODO: Error
            return NULL;
        } else {
            skipWhitespace();
            if(currIndex >= data.size()) return NULL; // TODO: Error
        }
        if(data[currIndex] == ')') {
            currIndex ++;
            break;
        }
        Value* r = read();
        if(r != NULL) list.push_back(r);
    }
    return new List(list,nline);
}

