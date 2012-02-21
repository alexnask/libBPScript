#include "BulletManager.h"
#include <SFML/Graphics.hpp>
#include <iostream>

using namespace BPS;

int main() {
    BulletManager* manager = BulletManager::fromFile("example.script");
    manager->spawn("test",384,284);

	sf::RenderWindow win(sf::VideoMode(800,600,32), "BPS test");
	win.SetFramerateLimit(20);
    sf::RectangleShape bulletShape(sf::Vector2f(32,32));
	while(win.IsOpen()) {
	    win.Clear();
	    sf::Event event;
	    while(win.PollEvent(event)) {
	        if(event.Type == sf::Event::Closed) {
                win.Close();
            }
	    }
	    try {
	        manager->update();
	        manager->map([&] (Bullet* bullet) {
	            if(bullet) {
	                if(bullet->getY() + 16 < 0 || bullet->getY() - 16 > 600 || bullet->getX() + 16 < 0 || bullet->getX() - 16 > 800 || !bullet->isActive()) {
	                    manager->destroy(bullet);
	                } else {
	                    bulletShape.SetPosition(bullet->getX(),bullet->getY());
	                    win.Draw(bulletShape);
	                }
	            }
	        });
	    } catch(Exception& exception) {
	        std::cout << exception.what() << std::endl;
	    }
	    win.Display();
	}
	delete manager;
    return 0;
}
