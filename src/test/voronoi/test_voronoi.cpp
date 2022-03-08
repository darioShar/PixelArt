#include <algorithm>
#include <cmath>
#include <iostream>
#include <utility>
#include <vector>
#include <SFML/Graphics.hpp>
#include <PixelArt/graph.h>
#include <PixelArt/voronoi.h>

enum Mode : int{
    DISPLAY_GRAPH,
    DISPLAY_VORONOI,
    DISPLAY_ACTIVE_EDGES,
    NUM_MODES
}mode;

int main(int argc, char* argv[])
{
	std::cout << "Starting test program on voronoi diagram" << std::endl;
    //Image contains Pixel Data
    sf::Image inputImage;
    if (!inputImage.loadFromFile(("../../../../../img/smw_yoshi_input.png"))) {
        std::cout << "Failed to open image file for processing" << std::endl;
        return -1;
    }

    sf::Vector2u dim = inputImage.getSize();

    //Create Similarity Graph
    depix::PixelGraph similarity(inputImage);
    //Planarize the graph
    similarity.planarize();

    depix::VoronoiDiagram diagram;
    diagram.setGraph(similarity);
    diagram.createDiagram();


    /***************** RENDERING *************************/
    // Let's setup a window
    sf::RenderWindow window(sf::VideoMode(500, 500), "SFML View Transformation");
    // Create something simple to draw
    sf::Texture texture;
    texture.loadFromImage(inputImage);
    sf::Sprite background(texture);
    sf::Vector2f oldPos;
    bool moving = false;

    float zoom = 1.0f;

    bool disp_background = true;

    int disp_color = 0;

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
                    mode = static_cast<Mode>((static_cast<int>(mode) + 1) % static_cast<int>(Mode::NUM_MODES));
                    std::cout << "Switched to mode " << mode << std::endl;
                }
                if (event.key.code == sf::Keyboard::B) {
                    disp_background = !disp_background;
                }
                if (event.key.code == sf::Keyboard::C) {
                    disp_color = (disp_color + 1) % 2;
                }
            }
        }

        // Draw our simple scene
        window.clear(sf::Color::White);
        float scale = 8.0f;
        if (disp_background) {
            background.setScale(sf::Vector2f(scale, scale));
            window.draw(background);
        }

        sf::Vertex line[2];
        line[0].color = sf::Color::Red;
        line[1].color = sf::Color::Red;


        switch (mode) {
        case Mode::DISPLAY_GRAPH :
        {
            auto& graph_edges = similarity.getEdges();
            for (int i = 0; i < dim.x; i++) {
                for (int j = 0; j < dim.y; j++) {
                    for (int k = 0; k < depix::NUM_DIR; k++) {
                        if (graph_edges[i][j][k]) {
                            auto dir = depix::VecDir[k];
                            line[0].position = scale * sf::Vector2f(i + 0.5f, j + 0.5f);
                            line[1].position = scale * sf::Vector2f(i + 0.5f + dir.x, j + 0.5f + dir.y);
                            window.draw(line, 2, sf::Lines);
                        }
                    }
                }
            }
            break;
        }
        case Mode::DISPLAY_VORONOI :
        {
            depix::diagram& d = diagram.getDiagram();
            for (auto& vec : d) {
                for (auto& p : vec.second) {
                    line[0].position = scale * vec.first;
                    line[1].position = scale * p;
                    window.draw(line, 2, sf::Lines);
                }
            }
            break;
        }
        case Mode::DISPLAY_ACTIVE_EDGES :
        {
            depix::edge_list& active_edges = diagram.getActiveEdges();
            for (auto& edge_color : active_edges) {
                auto& edge = edge_color.first;
                auto& colors = edge_color.second;
                line[0].position = scale * edge.first;
                line[1].position = scale * edge.second;
                if (disp_color == 0) {
                    line[0].color = colors.first;
                    line[1].color = colors.first;

                }
                else {
                    line[0].color = colors.second;
                    line[1].color = colors.second;
                }
                window.draw(line, 2, sf::Lines);
            }
            break;
        }
        }
       

        // ok checked.

        window.display();
    }

    /***************** RENDERING *************************/

	std::cout << "Test program on graph ended successfully" << std::endl;

    return 0;
}