#include "BulletManager.h"
using namespace BPS;

// TODO: Make sure there are no memory leaks and fix them if there are.
// TODO: Make sure this shithole works.

BulletManager* BulletManager::fromFile(const std::string& path) {
    std::string str,buff;
    std::ifstream infile;
    infile.open(path);
    while(!infile.eof()) {
        getline(infile,buff);
        str += buff + '\n';
    }
    infile.close();
    Parser* parser = new Parser(str);
    List* root = parser->run();
    BulletManager* manager = new BulletManager(root);
    delete parser;
    delete root;
    return manager;
}

BulletManager::BulletManager(List* root) throw(Exception) : ticks(0), rank(0.f) {
    if(Symbol::reserved.size() <= 0) {
        //Keywords
        Symbol::reserved.push_back("begin");
        Symbol::reserved.push_back("when");
        Symbol::reserved.push_back("speed");
        Symbol::reserved.push_back("rank");
        Symbol::reserved.push_back("acceleration");
        Symbol::reserved.push_back("direction");
        Symbol::reserved.push_back("lifetime");
        Symbol::reserved.push_back("spawn");
        Symbol::reserved.push_back("set");
        Symbol::reserved.push_back("random");
        Symbol::reserved.push_back("true");
        Symbol::reserved.push_back("false");
        //Operators
        Symbol::reserved.push_back("and");
        Symbol::reserved.push_back("or");
        Symbol::reserved.push_back("==");
        Symbol::reserved.push_back(">=");
        Symbol::reserved.push_back("<=");
        Symbol::reserved.push_back("<>");
        Symbol::reserved.push_back(">");
        Symbol::reserved.push_back("<");
        Symbol::reserved.push_back("+");
        Symbol::reserved.push_back("-");
        Symbol::reserved.push_back("/");
        Symbol::reserved.push_back("*");
        Symbol::reserved.push_back("%");
    }
    if(root->data.size() > 0) {
        for(unsigned int bullet = 0; bullet < root->data.size(); bullet++) {
            if(root->data[bullet]->getType() != LIST) {
                throw(Exception("List expected as a root element.",root->data[bullet]->getLine()));
            }
            List* bulletList = (List*)root->data[bullet];
            // Check the validity of the bullet construction clause
            if(bulletList->data.size() != 3) {
                throw(Exception("A bullet construction list should have exactly three atoms.",bulletList->getLine()));
            } else if(bulletList->data[0]->getType() != SYMBOL || ((Symbol*)bulletList->data[0])->data != "begin") {
                throw(Exception("A bullet constructior list should start with \"begin\"",bulletList->data[0]->getLine()));
            } else if(bulletList->data[1]->getType() != SYMBOL) {
                throw(Exception("The second argument to a bullet constructor list should be its name.",bulletList->data[1]->getLine()));
            } else if(std::find(Symbol::reserved.begin(), Symbol::reserved.end(), ((Symbol*)bulletList->data[1])->data) != Symbol::reserved.end()) {
                throw(Exception("Name " + ((Symbol*)bulletList->data[1])->data + " is reserved.",bulletList->data[1]->getLine()));
            } else if(bulletList->data[2]->getType() != LIST) {
                throw(Exception("The third argument to a bullet constructor list should be the bullet's code.",bulletList->data[2]->getLine()));
            }
            // Check the validity of the bullet code
            List* code = (List*)bulletList->data[2];
            if(code->data.size() <= 0) {
                throw(Exception("The bullet's code cannot be empty.",code->getLine()));
            } else if(code->data[0]->getType() != SYMBOL) {
                throw(Exception("The bullet's code first argument should be the name of the variable to bind the frame number to.",code->data[0]->getLine()));
            } else if(std::find(Symbol::reserved.begin(), Symbol::reserved.end(), ((Symbol*)code->data[0])->data) != Symbol::reserved.end()) {
                throw(Exception("Name " + ((Symbol*)code->data[0])->data + " is reserved.",code->data[0]->getLine()));
            }
            
            for(unsigned int rule = 1; rule < code->data.size(); rule++) {
                if(code->data[rule]->getType() != LIST) {
                    throw(Exception("All elements of a bullet's code except for the first should be rules",code->data[rule]->getLine()));
                } else if(((List*)code->data[rule])->data.size() < 3) {
                    throw(Exception("A rule should have more than or exactly three parts.",code->data[rule]->getLine()));
                } else if(((List*)code->data[rule])->data[0]->getType() != SYMBOL || ((Symbol*)((List*)code->data[rule])->data[0])->data != "when") {
                    throw(Exception("The first element to a rule should be \"when\"",((List*)code->data[rule])->data[0]->getLine()));
                }
            }
            
            Symbol::reserved.push_back(((Symbol*)bulletList->data[1])->data);
            // We copy the list so the root list can be destroyed after BulletManager is constructed
            if(bulletCode[((Symbol*)bulletList->data[1])->data]) delete bulletCode[((Symbol*)bulletList->data[1])->data];
            bulletCode[((Symbol*)bulletList->data[1])->data] = new List(*code);
        }
        // All bullets now loaded, can look for symbols pointing to nothing.
        for(std::map<std::string,List*>::iterator bullet = bulletCode.begin(); bullet != bulletCode.end(); ++bullet) {
            for(unsigned int rule = 1; rule < bullet->second->data.size(); ++rule) {
                try {
                    std::vector<std::string> ignored;
                    ignored.push_back(((Symbol*)bullet->second->data[0])->data);
                    checkSymbols(ignored,(List*)bullet->second->data[rule]);
                } catch(Exception& exception) {
                    throw(exception);
                }
            }
        }
    }
}

void BulletManager::checkSymbols(std::vector<std::string>& ignore, List* list) throw(Exception) {
    for(unsigned int elem = 0; elem < list->data.size(); ++elem) {
        if(list->data[elem]->getType() == LIST) {
            List* sublist = (List*)list->data[elem];
            bool ignored = false;
            if(sublist->data.size() > 2) {
                if(sublist->data[0]->getType() == SYMBOL) {
                    if(((Symbol*)sublist->data[0])->data == "spawn" && sublist->data[2]->getType() == SYMBOL) {
                        if(std::find(Symbol::reserved.begin(), Symbol::reserved.end(), ((Symbol*)sublist->data[2])->data) != Symbol::reserved.end() || std::find(ignore.begin(), ignore.end(), ((Symbol*)sublist->data[2])->data) != ignore.end()) {
                            throw(Exception("Redifinition of symbol " + ((Symbol*)sublist->data[2])->data,sublist->data[2]->getLine()));
                        } else {
                            ignored = true;
                            ignore.push_back(((Symbol*)sublist->data[2])->data);
                        }
                    } 
                }
            }

            try {
                checkSymbols(ignore, sublist);
            } catch(Exception& exception) {
                throw(exception);
            }
            if(ignored) {
                ignore.pop_back();
            }
        } else if(list->data[elem]->getType() == SYMBOL) {
            if(std::find(Symbol::reserved.begin(), Symbol::reserved.end(), ((Symbol*)list->data[elem])->data) == Symbol::reserved.end() && std::find(ignore.begin(), ignore.end(), ((Symbol*)list->data[elem])->data) == ignore.end()) {
                throw(Exception("Undefined symbol " + ((Symbol*)list->data[elem])->data, list->data[elem]->getLine()));
            }
        }
    }
}

Number* BulletManager::getNumber(Value* val, Bullet* bullet, const std::string& frameNum) throw(Exception) {
    if(val->getType() == NUMBER) {
        return new Number(*(Number*)val);
    } else if(val->getType() == SYMBOL) {
        if(((Symbol*)val)->data == frameNum) {
            return new Number((double)(ticks - bullet->getLifeStart()),val->getLine());
        }
        Value* foo = getVariable(((Symbol*)val)->data,bullet,val->getLine());
        if(foo->getType() == NUMBER) return new Number(*(Number*)foo);
        delete foo;
    } else {
        try {
            return getNumber(evaluate((List*)val,bullet,frameNum),bullet,frameNum);
        } catch(Exception& exception) {
            throw(exception);
        }
    }
    throw(Exception("Could not evaluate value to number.",val->getLine()));
}

Value* BulletManager::evaluate(List* _list, Bullet* bullet, const std::string& frameNum, unsigned int index, std::map<std::string,double> additional) throw(Exception) {
    // The list's symbols are replaced then the list is calculated
    List* list = new List(*_list);
    for(unsigned int ielem = index; ielem < list->data.size(); ielem++) {
        Value* elem = list->data[ielem];
        if(elem->getType() == SYMBOL) {
            if(((Symbol*)elem)->data == frameNum) {
                list->data[ielem] = new Number((double)(ticks - bullet->getLifeStart()),elem->getLine());
            } else if(additional.find(((Symbol*)elem)->data) != additional.end()) {
                list->data[ielem] = new Number(additional[((Symbol*)elem)->data],elem->getLine());
            } else {
                list->data[ielem] = getVariable(((Symbol*)elem)->data,bullet,elem->getLine());
            }
        } else if(elem->getType() == LIST) {
            list->data[ielem] = evaluate((List*)elem,bullet,frameNum,0,additional);
        }
    }
    Value* val = calculate(list);
    delete list;
    return val;
}

Value* BulletManager::calculate(List* list) throw(Exception) {
    // This list should be clear of variable symbols and be left with only operators
    if(list->data.size() > 0) {
        if(list->data[0]->getType() != SYMBOL && list->data.size() != 1) {
            throw(Exception("Invalid list: is neither comprised of a single number and does not start with an operator.",list->data[0]->getLine()));
        } else if(list->data[0]->getType() == NUMBER && list->data.size() == 1) {
            return new Number(*(Number*)list->data[0]);
        } else {
            Symbol* op = (Symbol*)list->data[0];
            if(op->data == "and") {
                if(list->data.size() < 3) {
                    throw(Exception("and expects at elast two arguments.",list->getLine()));
                }
                bool isTrue = true;
                for(unsigned int i = 1; i < list->data.size(); i++) {
                    if(list->data[i]->getType() == SYMBOL) {
                        if(((Symbol*)list->data[i])->data == "false") {
                            isTrue = false;
                        } else if(((Symbol*)list->data[i])->data != "true") {
                            throw(Exception("Invalid symbol " + ((Symbol*)list->data[i])->data + " passed as an argument to &&.",list->data[i]->getLine()));
                        }
                    } else if(list->data[i]->getType() == LIST) {
                        try {
                            Value* ret = calculate((List*)list->data[i]);
                            if(ret->getType() != SYMBOL) {
                                throw(Exception("Invalid argument of type argument passed to &&.",ret->getLine()));
                            } else {
                                if(((Symbol*)ret)->data == "false") {
                                    isTrue = false;
                                } else if(((Symbol*)ret)->data != "true") {
                                    throw(Exception("Invalid symbol " + ((Symbol*)ret)->data + " passed as an argument to &&.",ret->getLine()));
                                }
                            }
                            delete ret;
                        } catch(Exception& exception) {
                            throw(exception);
                        }
                    } else {
                        throw(Exception("Invalid argument of type argument passed to &&.",list->getLine()));
                    }
                }
                return new Symbol(((isTrue) ? "true" : "false"), list->getLine());
            } else if(op->data == "or") {
                if(list->data.size() < 3) {
                    throw(Exception("or expects at elast two arguments.",list->getLine()));
                }
                bool isTrue = false;
                for(unsigned int i = 1; i < list->data.size(); i++) {
                    if(list->data[i]->getType() == SYMBOL) {
                        if(((Symbol*)list->data[i])->data == "true") {
                            isTrue = true;
                        } else if(((Symbol*)list->data[i])->data != "false") {
                            throw(Exception("Invalid symbol " + ((Symbol*)list->data[i])->data + " passed as an argument to ||.",list->data[i]->getLine()));
                        }
                    } else if(list->data[i]->getType() == LIST) {
                        try {
                            Value* ret = calculate((List*)list->data[i]);
                            if(ret->getType() != SYMBOL) {
                                throw(Exception("Invalid argument of type argument passed to ||.",ret->getLine()));
                            } else {
                                if(((Symbol*)ret)->data == "true") {
                                    isTrue = true;
                                } else if(((Symbol*)ret)->data != "false") {
                                    throw(Exception("Invalid symbol " + ((Symbol*)ret)->data + " passed as an argument to ||.",ret->getLine()));
                                }
                            }
                            delete ret;
                        } catch(Exception& exception) {
                            throw(exception);
                        }
                    } else {
                        throw(Exception("Invalid argument of type argument passed to ||.",list->getLine()));
                    }
                }
                return new Symbol(((isTrue) ? "true" : "false"), list->getLine());
            } else if(op->data == "==") {
                if(list->data.size() != 3) {
                    throw(Exception("== expects exactly two arguments.",list->getLine()));
                }
                Value* val1 = list->data[1];
                Value* val2 = list->data[2];
                if(val1->getType() == LIST) try { Value* temp = val1; val1 = calculate((List*)val1); delete temp; } catch(Exception& exception) { throw(exception); }
                if(val2->getType() == LIST) try { Value* temp = val2; val2 = calculate((List*)val2); delete temp; } catch(Exception& exception) { throw(exception); }
                if(val1->getType() != val2->getType()) {
                    return new Symbol("false", list->getLine());
                }
                Symbol* sym;
                switch(val1->getType()) {
                    case SYMBOL:
                        sym = new Symbol(((((Symbol*)val1)->data == ((Symbol*)val2)->data) ? "true" : "false"), list->getLine());
                        break;
                    case NUMBER:
                        sym = new Symbol(((((Number*)val1)->data == ((Number*)val2)->data) ? "true" : "false"), list->getLine());
                        break;
                    default:
                        // Something has gone horribly wrong but we can't tell the user we failed D:
                        break;
                }
                //delete val1,val2;
                return sym;
            } else if(op->data == "<>") {
                if(list->data.size() != 3) {
                    throw(Exception("<> expects exactly two arguments.",list->getLine()));
                }
                Value* val1 = list->data[1];
                Value* val2 = list->data[2];
                if(val1->getType() == LIST) try { val1 = calculate((List*)val1); } catch(Exception& exception) { throw(exception); }
                if(val2->getType() == LIST) try { val2 = calculate((List*)val2); } catch(Exception& exception) { throw(exception); }
                if(val1->getType() != val2->getType()) {
                    return new Symbol("true", list->getLine());
                }
                switch(val1->getType()) {
                    case SYMBOL:
                        return new Symbol(((((Symbol*)val1)->data != ((Symbol*)val2)->data) ? "true" : "false"), list->getLine());
                        break;
                    case NUMBER:
                        return new Symbol(((((Number*)val1)->data != ((Number*)val2)->data) ? "true" : "false"), list->getLine());
                        break;
                    default:
                        // Something has gone horribly wrong but we can't tell the user we failed D:
                        break;
                }
            } else if(op->data == ">=") {
                if(list->data.size() != 3) {
                    throw(Exception(">= expects exactly two arguments.",list->getLine()));
                }
                Value* val1 = list->data[1];
                Value* val2 = list->data[2];
                if(val1->getType() == LIST) try { val1 = calculate((List*)val1); } catch(Exception& exception) { throw(exception); }
                if(val2->getType() == LIST) try { val2 = calculate((List*)val2); } catch(Exception& exception) { throw(exception); }
                if(val1->getType() != NUMBER || val2->getType() != NUMBER) {
                    throw(Exception("You can only compare numbers you silly :D",val1->getLine()));
                }
                return new Symbol(((((Number*)val1)->data >= ((Number*)val2)->data) ? "true" : "false"), list->getLine());
            } else if(op->data == "<=") {
                if(list->data.size() != 3) {
                    throw(Exception("<= expects exactly two arguments.",list->getLine()));
                }
                Value* val1 = list->data[1];
                Value* val2 = list->data[2];
                if(val1->getType() == LIST) try { val1 = calculate((List*)val1); } catch(Exception& exception) { throw(exception); }
                if(val2->getType() == LIST) try { val2 = calculate((List*)val2); } catch(Exception& exception) { throw(exception); }
                if(val1->getType() != NUMBER || val2->getType() != NUMBER) {
                    throw(Exception("You can only compare numbers you silly :D",val1->getLine()));
                }
                return new Symbol(((((Number*)val1)->data <= ((Number*)val2)->data) ? "true" : "false"), list->getLine());
            } else if(op->data == ">")  {
                if(list->data.size() != 3) {
                    throw(Exception("> expects exactly two arguments.",list->getLine()));
                }
                Value* val1 = list->data[1];
                Value* val2 = list->data[2];
                if(val1->getType() == LIST) try { val1 = calculate((List*)val1); } catch(Exception& exception) { throw(exception); }
                if(val2->getType() == LIST) try { val2 = calculate((List*)val2); } catch(Exception& exception) { throw(exception); }
                if(val1->getType() != NUMBER || val2->getType() != NUMBER) {
                    throw(Exception("You can only compare numbers you silly :D",val1->getLine()));
                }
                return new Symbol(((((Number*)val1)->data > ((Number*)val2)->data) ? "true" : "false"), list->getLine());
            } else if(op->data == "<")  {
                if(list->data.size() != 3) {
                    throw(Exception("< expects exactly two arguments.",list->getLine()));
                }
                Value* val1 = list->data[1];
                Value* val2 = list->data[2];
                if(val1->getType() == LIST) try { val1 = calculate((List*)val1); } catch(Exception& exception) { throw(exception); }
                if(val2->getType() == LIST) try { val2 = calculate((List*)val2); } catch(Exception& exception) { throw(exception); }
                if(val1->getType() != NUMBER || val2->getType() != NUMBER) {
                    throw(Exception("You can only compare numbers you silly :D",val1->getLine()));
                }
                return new Symbol(((((Number*)val1)->data < ((Number*)val2)->data) ? "true" : "false"), list->getLine());
            } else if(op->data == "+")  {
                if(list->data.size() < 3) {
                    throw(Exception("+ expects at least two arguments.",list->getLine()));
                }
                double ret = 0.f;
                for(unsigned int i = 1; i < list->data.size(); i++) {
                    Value* val = list->data[i];
                    if(val->getType() == LIST) try { val = calculate((List*)val); } catch(Exception& exception) { throw(exception); }
                    if(val->getType() != NUMBER) {
                        throw(Exception("+ only accepts numbers.",val->getLine()));
                    }
                    ret += ((Number*)val)->data;
                }
                return new Number(ret,list->getLine());
            } else if(op->data == "-")  {
                if(list->data.size() < 3) {
                    throw(Exception("- expects at least two arguments.",list->getLine()));
                }
                Value* val1 = list->data[1];
                if(val1->getType() == LIST) try { val1 = calculate((List*)val1); } catch(Exception& exception) { throw(exception); }
                if(val1->getType() != NUMBER) {
                    throw(Exception("- only accepts numbers.",val1->getLine()));
                }
                double ret = ((Number*)val1)->data;
                for(unsigned int i = 2; i < list->data.size(); i++) {
                    Value* val = list->data[i];
                    if(val->getType() == LIST) try { val = calculate((List*)val); } catch(Exception& exception) { throw(exception); }
                    if(val->getType() != NUMBER) {
                        throw(Exception("- only accepts numbers.",val->getLine()));
                    }
                    ret -= ((Number*)val)->data;
                }
                return new Number(ret,list->getLine());
            } else if(op->data == "*")  {
                if(list->data.size() < 3) {
                    throw(Exception("* expects at least two arguments.",list->getLine()));
                }
                Value* val1 = list->data[1];
                if(val1->getType() == LIST) try { val1 = calculate((List*)val1); } catch(Exception& exception) { throw(exception); }
                if(val1->getType() != NUMBER) {
                    throw(Exception("* only accepts numbers.",val1->getLine()));
                }
                double ret = ((Number*)val1)->data;
                for(unsigned int i = 2; i < list->data.size(); i++) {
                    Value* val = list->data[i];
                    if(val->getType() == LIST) try { val = calculate((List*)val); } catch(Exception& exception) { throw(exception); }
                    if(val->getType() != NUMBER) {
                        throw(Exception("* only accepts numbers.",val->getLine()));
                    }
                    ret *= ((Number*)val)->data;
                }
                return new Number(ret,list->getLine());
            } else if(op->data == "/")  {
                if(list->data.size() != 3) {
                    throw(Exception("/ expects exactly two arguments.",list->getLine()));
                }
                Value* val1 = list->data[1];
                Value* val2 = list->data[2];
                if(val1->getType() == LIST) try { val1 = calculate((List*)val1); } catch(Exception& exception) { throw(exception); }
                if(val2->getType() == LIST) try { val2 = calculate((List*)val2); } catch(Exception& exception) { throw(exception); }
                if(val1->getType() != NUMBER || val2->getType() != NUMBER) {
                    throw(Exception("/ only accepts numbers.",val1->getLine()));
                }
                return new Number(((Number*)val1)->data / ((Number*)val2)->data,list->getLine());
            } else if(op->data == "%")  {
                if(list->data.size() != 3) {
                    throw(Exception("% expects exactly two arguments.",list->getLine()));
                }
                Value* val1 = list->data[1];
                Value* val2 = list->data[2];
                if(val1->getType() == LIST) try { val1 = calculate((List*)val1); } catch(Exception& exception) { throw(exception); }
                if(val2->getType() == LIST) try { val2 = calculate((List*)val2); } catch(Exception& exception) { throw(exception); }
                if(val1->getType() != NUMBER || val2->getType() != NUMBER) {
                    throw(Exception("% only accepts numbers.",val1->getLine()));
                }
                return new Number(fmod(((Number*)val1)->data, ((Number*)val2)->data),list->getLine());
            } else {
                throw(Exception(op->data + " is not a valid operator.",op->getLine()));
            }
        }
    }
    return new Number(0.f,list->getLine());
}

Value* BulletManager::getVariable(const std::string& name, Bullet* bullet, unsigned int line) {
    if(name == "direction") {
        return new Number(bullet->getDirection(),line);
    } else if(name == "speed") {
        return new Number(bullet->getSpeed(),line);
    } else if(name == "acceleration") {
        return new Number(bullet->getAcceleration(),line);
    } else if(name == "lifetime") {
        return new Number(bullet->getLifetime(),line);
    } else if(name == "rank") {
        return new Number(rank,line);
    } else if(name == "and" || name == "or" || name == "==" || name == ">=" || name == "<=" || name == "<>" || name == ">" || name == "<" || name == "+" || name == "-" || name == "*" || name == "/" || name == "*" || name == "%") {
        return new Symbol(name,line);
    }
    return NULL;
}

void BulletManager::setVariable(const std::string& name, double value, Bullet* bullet) {
    if(name == "direction") {
        bullet->setDirection(value);
    } else if(name == "speed") {
        bullet->setSpeed(value);
    } else if(name == "acceleration") {
        bullet->setAcceleration(value);
    } else if(name == "lifetime") {
        bullet->setLifetime(value);
    }
}

void BulletManager::update() throw(Exception) {
    // Here, we will find all bullets of the same type and execute the bullet code on them. MUHAHAHAHAHA (wootlol)
    unsigned int count = bullets.size();
    for(unsigned int counter = 0; counter < count; counter++) {
        Bullet* bullet = bullets[counter];
        if(bulletCode.count(bullet->getType()) > 0) {
            // First, we must replace known variables with their values in the code
            // To do that, we copy the code so we can change everything in our new copy
            List* code = new List(*bulletCode[bullet->getType()]);
            if(code->data.size() > 1) {
                std::string& frameNum = ((Symbol*)code->data[0])->data;
                for(std::vector<Value*>::iterator val = code->data.begin()+1; val != code->data.end(); ++val) {
                    List* rule = new List(*(List*)*val);
                    Value* condition;
                    if(rule->data[1]->getType() == LIST) {
                        try {
                            condition = evaluate((List*)rule->data[1],bullet,frameNum);
                        } catch(Exception& exception) {
                            throw(exception);
                        }
                    } else if(rule->data[1]->getType() == SYMBOL) {
                        if(((Symbol*)rule->data[1])->data == "true" || ((Symbol*)rule->data[1])->data == "false") {
                            condition = (Value*)(new Symbol(*(Symbol*)rule->data[1]));
                        } else {
                            throw(Exception("A rule's condition can only be a list that calculates to true or false or them values themselves :)",rule->data[1]->getLine()));
                        }
                    } else {
                        throw(Exception("A rule's condition can only be a list that calculates to true or false or them values themselves :)",rule->data[1]->getLine()));
                    }
                    if(condition->getType() != SYMBOL) {
                        throw(Exception("A rule's condition can only be a list that calculates to true or false or them values themselves :)",condition->getLine()));
                    } else if(((Symbol*)condition)->data == "true") {
                        // The condition was calculated to true, it is time to execute the body now
                        for(unsigned int bit = 2; bit < rule->data.size(); ++bit) {
                            if(rule->data[bit]->getType() != LIST) {
                                throw(Exception("The only actions allowed in a rule's code are setting and spawning.",rule->data[bit]->getLine()));
                            }
                            List* statement = (List*)rule->data[bit];
                            if(statement->data.size() > 0) {
                                if(statement->data[0]->getType() != SYMBOL) {
                                    throw(Exception("The only actions allowed in a rule's code are setting and spawning.",statement->data[0]->getLine()));
                                } else if(((Symbol*)statement->data[0])->data != "set" && ((Symbol*)statement->data[0])->data != "spawn") {
                                    throw(Exception("The only actions allowed in a rule's code are setting and spawning.",statement->data[0]->getLine()));
                                } else if(((Symbol*)statement->data[0])->data == "set") {
                                    if(statement->data.size() != 3) {
                                        throw(Exception("set expects exactly two arguments.",statement->getLine()));
                                    }
                                    if(statement->data[1]->getType() != SYMBOL) {
                                        throw(Exception("set's first argument should be a variable name.",statement->data[1]->getLine()));
                                    } else if(((Symbol*)statement->data[1])->data == frameNum || (std::find(Symbol::reserved.begin(), Symbol::reserved.end(), ((Symbol*)statement->data[1])->data) != Symbol::reserved.end() && ((Symbol*)statement->data[1])->data != "direction" && ((Symbol*)statement->data[1])->data != "speed" && ((Symbol*)statement->data[1])->data != "acceleration" && ((Symbol*)statement->data[1])->data != "lifetime")) {
                                        // TODO: check this code
                                        throw(Exception("Variable " + ((Symbol*)statement->data[1])->data  + " is immutable, you cannot change it.", statement->data[1]->getLine()));
                                    }
                                    try {
                                        Number* val = getNumber(statement->data[2],bullet,frameNum);
                                        setVariable(((Symbol*)statement->data[1])->data,val->data,bullet);
                                        delete val;
                                    } catch(Exception& exception) {
                                        throw(exception);
                                    }
                                } else if(((Symbol*)statement->data[0])->data == "spawn") {
                                    int spawns = 0;
                                    if(statement->data.size() != 4) {
                                        throw(Exception("spawn expects exactly three arguments.",statement->getLine()));
                                    }
                                    try {
                                        Number* val = getNumber(statement->data[1],bullet,frameNum);
                                        spawns = (int)round(val->data);
                                        delete val;
                                    } catch(Exception& exception) {
                                        throw(exception);
                                    }
                                    if(statement->data[2]->getType() != SYMBOL) {
                                        throw(Exception("spawn's second argument should be the name of the spawned number variable.",statement->data[2]->getLine()));
                                    } else if(std::find(Symbol::reserved.begin(), Symbol::reserved.end(), ((Symbol*)statement->data[2])->data) != Symbol::reserved.end()) {
                                        throw(Exception("Symbol " + ((Symbol*)statement->data[2])->data + " is reserved.",statement->data[2]->getLine()));
                                    } else if(statement->data[3]->getType() != LIST) {
                                        throw(Exception("spawn's third argument should be a bullet construction list.",statement->data[3]->getLine()));
                                    } else if(((List*)statement->data[3])->data.size() != 5) {
                                        throw(Exception("A bullet construction list should contain exactly five atoms.",statement->data[3]->getLine()));
                                    }
                                    
                                    for(int i = 1; i <= spawns; ++i) {
                                        std::map<std::string,double> additional;
                                        additional[((Symbol*)statement->data[2])->data] = (double)i;
                                        double arguments[4] = { 0.f };
                                        for(unsigned int j = 1; j < 5; j++) {
                                            try {
                                                if(((List*)statement->data[3])->data[j]->getType() != LIST) {
                                                    Number* val = getNumber(((List*)statement->data[3])->data[j],bullet,frameNum);
                                                    arguments[j-1] = val->data;
                                                    delete val;
                                                } else {
                                                    // The problem appears to be that the list is modified while it should not xO
                                                    Value* val = evaluate((List*)((List*)statement->data[3])->data[j],bullet,frameNum,0,additional);
                                                    if(val->getType() != NUMBER) {
                                                        throw(Exception("Could not evaluate argument of the bullet construction lsit to a number.",val->getLine()));
                                                    }
                                                    arguments[j-1] = ((Number*)val)->data;
                                                    delete val;
                                                }
                                            } catch(Exception& exception) {
                                                throw(exception);
                                            }
                                        }
                                        spawn(((Symbol*)(((List*)statement->data[3])->data[0]))->data,bullet->getX(),bullet->getY(),arguments[0],arguments[1],arguments[2],arguments[3]);
                                    }
                                }
                            }
                        }
                    }
                    delete condition;
                    delete rule;
                }
            }
            // Finally, we destroy our copy of the code
            delete code;
        }
        // update the bullet's position, speed, acceleration, ... 
        bullet->update();
    }
    ticks ++;
}

void BulletManager::spawn(const std::string& name, double x, double y, double direction, double speed, double acceleration, double lifetime) {
    Bullet* bullet = new Bullet(x,y,ticks);
    bullet->setType(name);
    bullet->setDirection(direction);
    bullet->setSpeed(speed);
    bullet->setAcceleration(acceleration);
    bullet->setLifetime(lifetime);
    bullets.push_back(bullet);
}

double BulletManager::getRank() {
    return rank;
}

void BulletManager::setRank(double _rank) {
    rank = _rank;
}

void BulletManager::map(std::function<void(Bullet*)> apply) {
    std::for_each(bullets.begin(), bullets.end(), apply);
}

void BulletManager::destroy(Bullet* bullet) {
    std::vector<Bullet*>::iterator iter = std::find(bullets.begin(), bullets.end(), bullet);
    if(iter != bullets.end()) {
       if(bullet) {
           delete bullet;
           bullet = NULL;
       }
       bullets.erase(iter);
    }
}

BulletManager::~BulletManager() {
    for(auto bullet: bullets) {
        delete bullet;
        bullet = NULL;
    }
    bullets.clear();
    for(auto code: bulletCode) {
        delete code.second;
        code.second = NULL;
    }
    bulletCode.clear();
}

