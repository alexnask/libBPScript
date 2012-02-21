#ifndef BULLET_MANAGER__H
#define BULLET_MANAGER__H
#include "Bullet.h"
#include "Value.h"
#include "Exception.h"
#include <map>
#include <cmath>
#include <algorithm>
#include <functional>

namespace BPS {

class BulletManager {
    private:
        std::vector<Bullet*> bullets;
        std::map<std::string,List*> bulletCode;
        int ticks;
        double rank;
        
        void checkSymbols(std::vector<std::string>&,List*) throw(Exception);
        Value* calculate(List* list) throw(Exception);
        Value* evaluate(List* list,Bullet*,const std::string&,unsigned int index = 0,std::map<std::string,double> additionalValue = std::map<std::string,double>()) throw(Exception);
        Number* getNumber(Value* val,Bullet*,const std::string&) throw(Exception);
        
        Value* getVariable(const std::string&,Bullet*,unsigned int);
        void setVariable(const std::string&,double,Bullet*);
    public:
        BulletManager(List*) throw(Exception);
        void spawn(const std::string&, double, double, double direction = 0.f, double speed = 0.f, double acceleration = 0.f, double lifetime = 10.f);
        void update() throw(Exception);
        double getRank();
        void setRank(double);
        void map(std::function<void(Bullet*)>);
        void destroy(Bullet*);
        ~BulletManager();
};

}

#endif//BULLET_MANAGER__H
