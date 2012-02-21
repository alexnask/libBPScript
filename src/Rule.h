#ifndef RULE__H
#define RULE__H
#include "Bullet.h"

namespace BPS {

class Rule {
    private:
        Symbol* directionSym, speedSym, accelerationSym, lifetimeSym, spawnedNumberSym;
        Value* condition;
        List* effect;
    public:
        Rule(Symbol*,Symbol*,Symbol*,Symbol*,Symbol*,Value*,List*);
        bool matches(int);
        void affect(Bullet*);
};

}
#endif//RULE__H
