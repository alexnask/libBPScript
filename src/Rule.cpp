#include "Rule.h"
using namespace BPS;


Rule::Rule(Symbol* direction,Symbol* speed,Symbol* acceleration,Symbol* lifetime,Symbol* spawnedNumber,Value* _condition,List* _effect) : directionSym(direction), speedSym(speed), accelerationSym(acceleration), lifetimeSym(lifetime), spawnedNumberSym(spawnedNumberSym),consition(_condition), effect(_effect) {}
