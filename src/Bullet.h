#ifndef BULLET__H
#define BULLET__H
#include <string>

namespace BPS {

class Bullet {
    private:
        double speed, acceleration, direction, lifetime;
        int ticks;
        double x,y;
        bool active;
        std::string type;
    public:
        Bullet(double,double);
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
        double getX();
        double getY();
        bool isActive();
};

}
#endif//BULLET__H
