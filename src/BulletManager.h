#ifndef BULLET_MANAGER__H
#define BULLET_MANAGER__H
#include "Bullet.h"
#include "Value.h"
#include "Parser.h"
#include "Exception.h"
#include <map>
#include <cmath>
#include <algorithm>
#include <functional>
#include <fstream>

namespace BPS {

class BulletManager {
    protected:
        std::vector<Bullet*> bullets;
        std::vector<Bullet*>::iterator iter;
        std::map<std::string,List*> bulletCode;
        int ticks;
        double rank;
    private:
        void checkSymbols(std::vector<std::string>&,List*) throw(Exception);
        Value* calculate(List* list) throw(Exception);
        Value* evaluate(List* list,Bullet*,const std::string&,unsigned int index = 0,std::map<std::string,double> additionalValue = std::map<std::string,double>()) throw(Exception);
        Number* getNumber(Value* val,Bullet*,const std::string&) throw(Exception);
        
        Value* getVariable(const std::string&,Bullet*,unsigned int);
        void setVariable(const std::string&,double,Bullet*);
    public:
        BulletManager(List*) throw(Exception);
        static BulletManager* fromFile(const std::string&);
        virtual void spawn(const std::string&, double, double, double direction = 0.f, double speed = 0.f, double acceleration = 0.f, double lifetime = 10.f);
        virtual void update() throw(Exception);
        virtual double getRank();
        virtual void setRank(double);
        virtual void map(std::function<void(Bullet*)>);
        virtual void destroy(Bullet*);
        virtual ~BulletManager();
};

}

#endif//BULLET_MANAGER__H
