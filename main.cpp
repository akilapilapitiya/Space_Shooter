#include <iostream>
#include <SFML/Graphics.hpp>

//GLOBAL VARIABLES INITIALIZATION

//Window Initialization
sf::RenderWindow window(sf::VideoMode(600, 500), "<Game Name>"); //To Update


//Main Program
int main()
{
    sf::CircleShape shape(100.f);
    shape.setFillColor(sf::Color::Blue);

    while (window.isOpen())
    {
        sf::Event event;
        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed)
                window.close();
        }
        window.clear();
        window.draw(shape);
        window.display();
    }

    return 0;
    int num;
    number  = 10;
}
