#ifndef BULLET__H
#define BULLET__H
#include <string>

namespace BPS {

class Bullet {
    private:
        double speed, acceleration, direction, lifetime;
        int ticks;
        int lifeStart; // Tick of the BulletManager the bullet's life started
        double x,y;
        bool active;
        std::string type;
    public:
        Bullet(double,double,int);
        virtual ~Bullet();
        virtual void update();
        
        virtual std::string getType();
        virtual void setType(const std::string&);
        virtual double getSpeed();
        virtual void setSpeed(double);
        virtual double getAcceleration();
        virtual void setAcceleration(double);
        virtual double getDirection();
        virtual void setDirection(double);
        virtual double getLifetime();
        virtual void setLifetime(double);
        virtual int getTicks();
        virtual int getLifeStart();
        virtual double getX();
        virtual double getY();
        virtual void setX(double);
        virtual void setY(double);
        virtual void setPosition(double,double);
        virtual bool isActive();
};

}
#endif//BULLET__H
