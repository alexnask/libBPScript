#include "Bullet.h"
#include <cmath>
using namespace BPS;
#define pi 3.14159265

inline double degreeToRadian(double degree) {
	return degree * (pi/180);
}


Bullet::Bullet(double _x, double _y, int _lifestart) : ticks(0), active(true), x(_x), y(_y), speed(0.f), acceleration(0.f), direction(0.f), lifetime(0), type("none"), lifeStart(_lifestart) {}

Bullet::~Bullet() {
}

void Bullet::update() {
    if(active) {
        if(ticks > lifetime) {
            active = false;
        }
        
        double speedx = speed * cos(degreeToRadian(direction));
        double speedy = speed * sin(degreeToRadian(direction));
        x += speedx;
        y += speedy;
        double accelx = acceleration * cos(degreeToRadian(direction));
        double accely = acceleration * sin(degreeToRadian(direction));
        speedx += accelx;
        speedy += accely;
        speed = sqrt(speedx*speedx+speedy*speedy);
        ticks++;
    }
}

std::string Bullet::getType() {
    return type;
}

void Bullet::setType(const std::string& _type) {
    type = _type;
}

double Bullet::getSpeed() {
    return speed;
}

void Bullet::setSpeed(double _speed) {
    speed = _speed;
}

double Bullet::getAcceleration() {
    return acceleration;
}

void Bullet::setAcceleration(double _acceleration) {
    acceleration = _acceleration;
}

double Bullet::getDirection() {
    return direction;
}

void Bullet::setDirection(double _direction) {
    direction = _direction;
}

double Bullet::getLifetime() {
    return lifetime;
}

void Bullet::setLifetime(double _lifetime) {
    lifetime = _lifetime;
}

double Bullet::getX() {
    return x;
}

double Bullet::getY() {
    return y;
}

void Bullet::setX(double _x) {
    x = _x;
}

void Bullet::setY(double _y) {
    y = _y;
}

void Bullet::setPosition(double _x, double _y) {
    setX(_x); setY(_y);
}

int Bullet::getTicks() {
    return ticks;
}

bool Bullet::isActive() {
    return active;
}

int Bullet::getLifeStart() {
    return lifeStart;
}

