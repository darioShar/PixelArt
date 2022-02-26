#include <algorithm>
#include <cmath>
#include <iostream>
#include <utility>
#include <vector>
#include "interpolation.hpp"
#include "interpolation_impl.hpp"
#include <SFML/Window.hpp>
#include <iostream>

int main(int argc, char* argv[])
{
    std::cout << "Testing SFML program" << std::endl;
    sf::Window window(sf::VideoMode(800, 600), "My window");

    sf::Clock clock;
    while (window.isOpen() && clock.getElapsedTime().asSeconds() < 1.0)
    {
        // check all the window's events that were triggered since the last iteration of the loop
        sf::Event event;
        while (window.pollEvent(event))
        {
            // "close requested" event: we close the window
            if (event.type == sf::Event::Closed)
                window.close();
        }
    }

    std::cout << "SFML program ended successfully" << std::endl;
    return 0;
}
