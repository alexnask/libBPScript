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
        void update();
        
        std::string getType();
        void setType(const std::string&);
        double getSpeed();
        void setSpeed(double);
        double getAcceleration();
        void setAcceleration(double);
        double getDirection();
        void setDirection(double);
        double getLifetime();
        void setLifetime(double);
        int getTicks();
        int getLifeStart();
        double getX();
        double getY();
        void setX(double);
        void setY(double);
        void setPosition(double,double);
        bool isActive();
};

}
#endif//BULLET__H
