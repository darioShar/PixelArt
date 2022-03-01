#include <algorithm>
#include <cmath>
#include <iostream>
#include <utility>
#include <vector>
#include <SFML/Graphics.hpp>
#include "../../graph.h"





int main(int argc, char* argv[])
{
	std::cout << "Starting test program on graph" << std::endl;
	//Image contains Pixel Data
	sf::Image inputImage;
	if (!inputImage.loadFromFile(("D:\\Desktop\\Dario\\3A\\MAP586\\projet\\PixelArt\\src\\img\\smw2_yoshi_02_input.png"))) {
		std::cout << "Failed to open image file for processing" << std::endl;
		return -1;
	}

    sf::Vector2u dim = inputImage.getSize();

	//Create Similarity Graph
	Graph similarity(inputImage);
	//Planarize the graph
	similarity.planarize();


    /***************** RENDERING *************************/
    // Let's setup a window
    sf::RenderWindow window(sf::VideoMode(200, 200), "SFML View Transformation");

    // Create something simple to draw
    sf::Texture texture;
    texture.loadFromImage(inputImage);
    sf::Sprite background(texture);

    sf::Vector2f oldPos;
    bool moving = false;

    float zoom = 1;

    // Retrieve the window's default view
    sf::View view = window.getDefaultView();

    double accumZoom = 1;

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
                    zoom = std::min(2.f, zoom + .1f);
                else if (event.mouseWheelScroll.delta >= 1)
                    zoom = std::max(.5f, zoom - .1f);

                // Update our view
                view.setSize(window.getDefaultView().getSize()); // Reset the size
                view.zoom(zoom); // Apply the zoom level (this transforms the view)
                window.setView(view);
                break;
            }
        }

        // Draw our simple scene
        window.clear(sf::Color::White);
        float scale = 8.0f;
        background.setScale(sf::Vector2f(scale, scale));
        window.draw(background);

        sf::Vertex line[2];
        line[0].color = sf::Color::Red;
        line[1].color = sf::Color::Red;

        auto& edges = similarity.getEdges();
        for (int i = 0; i < dim.x; i++) {
            for (int j = 0; j < dim.y; j++) {
                for (int k = 0; k < NUM_DIR; k++) {
                    if (edges[i][j][k]) {
                        auto dir = VecDir[k];
                        line[0].position = scale * sf::Vector2f(i + 0.5f, j + 0.5f);
                        line[1].position = scale * sf::Vector2f(i + 0.5f + dir.x, j + 0.5f + dir.y);
                        window.draw(line, 2, sf::Lines);
                    }
                }
            }
        }

        window.display();
    }

    /***************** RENDERING *************************/

	std::cout << "Test program on graph ended successfully" << std::endl;

    return 0;
}