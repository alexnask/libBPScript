#ifndef BULLET__H
#define BULLET__H
#include <string>

namespace BPS {

class Bullet {
    protected:
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
        
        virtual std::string getType() const;
        virtual void setType(const std::string&);
        virtual double getSpeed() const;
        virtual void setSpeed(double);
        virtual double getAcceleration() const;
        virtual void setAcceleration(double);
        virtual double getDirection() const;
        virtual void setDirection(double);
        virtual double getLifetime() const;
        virtual void setLifetime(double);
        virtual int getTicks() const;
        virtual int getLifeStart() const;
        virtual double getX() const;
        virtual double getY() const;
        virtual void setX(double);
        virtual void setY(double);
        virtual void setPosition(double,double);
        virtual bool isActive() const;
};

}
#endif//BULLET__H
