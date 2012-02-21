#ifndef PARSER__H
#define PARSER__H
#include "Value.h"

namespace BPS {

class Parser {
    private:
        std::string data;
        unsigned int currIndex;
        
        Value* read();
        void skipWhitespace();
        void skipLine();
        std::string readTillWhitespace();
        unsigned int line();
        
        Number* readNumber();
        List* readList();
        Symbol* readSymbol();
    public:
        Parser(std::string);
        List* run();
};

}
#endif//PARSER__H
