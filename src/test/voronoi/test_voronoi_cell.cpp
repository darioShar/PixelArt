#include <algorithm>
#include <cmath>
#include <iostream>
#include <utility>
#include <vector>
#include <SFML/Graphics.hpp>
#include <PixelArt/pixel_graph.h>
#include <PixelArt/voronoi_diagram.h>






int main(int argc, char* argv[])
{
	std::cout << "Starting test program on voronoi cell" << std::endl;
	

    pa::VoronoiDiagram diagram;
    const pa::possible_cells_list& cells = diagram.getPossibleVoronoiCells();
    pa::voronoiCellType type = 0;

    /***************** RENDERING *************************/
    // Let's setup a window
    sf::RenderWindow window(sf::VideoMode(500, 500), "SFML View Transformation");

    sf::Vector2f oldPos;
    bool moving = false;

    float zoom = 1.0f;

    // Retrieve the window's default view
    sf::View view = window.getDefaultView();

    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            switch (event.type) {
            case sf::Event::Closed:
                window.close();
                break;
            case sf::Event::MouseButtonPressed:
                // Mouse button is pressed, get the position and set moving as active
                if (event.mouseButton.button == 0) {
                    moving = true;
                    oldPos = window.mapPixelToCoords(sf::Vector2i(event.mouseButton.x, event.mouseButton.y));
                }
                break;
            case  sf::Event::MouseButtonReleased:
                // Mouse button is released, no longer move
                if (event.mouseButton.button == 0) {
                    moving = false;
                }
                break;
            case sf::Event::MouseMoved:
            {
                // Ignore mouse movement unless a button is pressed (see above)
                if (!moving)
                    break;
                // Determine the new position in world coordinates
                const sf::Vector2f newPos = window.mapPixelToCoords(sf::Vector2i(event.mouseMove.x, event.mouseMove.y));
                // Determine how the cursor has moved
                // Swap these to invert the movement direction
                const sf::Vector2f deltaPos = oldPos - newPos;

                // Move our view accordingly and update the window
                view.setCenter(view.getCenter() + deltaPos);
                window.setView(view);

                // Save the new position as the old one
                // We're recalculating this, since we've changed the view
                oldPos = window.mapPixelToCoords(sf::Vector2i(event.mouseMove.x, event.mouseMove.y));
                break;
            }
            case sf::Event::MouseWheelScrolled:
                // Ignore the mouse wheel unless we're not moving
                if (moving)
                    break;

                // Determine the scroll direction and adjust the zoom level
                // Again, you can swap these to invert the direction
                if (event.mouseWheelScroll.delta <= -1)
                    zoom *= 1.1f;
                else if (event.mouseWheelScroll.delta >= 1)
                    zoom /= 1.1f;

                // Update our view
                view.setSize(window.getDefaultView().getSize()); // Reset the size
                view.zoom(zoom); // Apply the zoom level (this transforms the view)
                window.setView(view);
                break;
            case sf::Event::KeyPressed:
                if (event.key.code == sf::Keyboard::N) {
                    do {
                        type++;
                    } while (((type & 0b11) == 0b11)
                        || (type & (0b11 << 2)) == (0b11 << 2)
                        || (type & (0b11 << 4)) == (0b11 << 4)
                        || (type & (0b11 << 6)) == (0b11 << 6));
                    std::cout << "configuration : ";
                    for (int i = 0; i < 8; i++) {
                        std::cout << !!(type & 1 << i) << " : ";
                    }
                    std::cout << std::endl;
                }

            }
        }

        // Draw our simple scene
        window.clear(sf::Color::White);
        float scale = 8.0f;

        sf::Vertex line[2];
        line[0].color = sf::Color::Red;
        line[1].color = sf::Color::Red;
        
        /*pa::diagram& d = diagram.getDiagram();
        for (auto& vec : d) {
            for (auto& p : vec.second) {
                line[0].position = scale * vec.first;
                line[1].position = scale * p;
                window.draw(line, 2, sf::Lines);
            }
        }*/
        auto& cell = cells[type];
        for (int i = 0; i < cell.size(); i++) {
            int j = (i + 1) % cell.size();
            line[0].position = scale * cell[i];
            line[1].position = scale * cell[j];
            window.draw(line, 2, sf::Lines);
        }

        // ok checked.

        window.display();
    }

    /***************** RENDERING *************************/

	std::cout << "Test program on graph ended successfully" << std::endl;

    return 0;
}