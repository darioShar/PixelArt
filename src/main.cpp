#include <algorithm>
#include <cmath>
#include <iostream>
#include <utility>
#include <vector>
#include <PixelArt/voronoi_diagram.h>
#include <PixelArt/image_op.h>
#include <argparse.hpp>
#include <filesystem>

enum Mode : int {
    DISPLAY_GRAPH,
    DISPLAY_VORONOI,
    DISPLAY_ACTIVE_EDGES,
    NUM_MODES
}mode;

struct PixelArtArgs : public argparse::Args {
    std::string& src_path = kwarg("s", "source file/folder");
    float& default_scale = kwarg("z", "Default scale value of images").set_default(8.0);
    std::vector<float>& yuv_similarity  = 
        kwarg("yuv", "max Y, U, V difference for initial graph construction (similarity determination)")
        .set_default(std::vector<float>({42.0, 7.0, 6.0}));
    std::vector<float>& yuv_edges =
        kwarg("dissimilarity", "YUV L^2 distances specifying active edges types (shading edge, contour edge)")
        .set_default(std::vector<float>({ 3.0/255.0, 100.0/255.0 }));
    bool& verbose = flag("v,verbose", "A flag to toggle verbose");
};


int main(int argc, char* argv[])
{
    // Program running with command line
    // Set up program according to argument list
    PixelArtArgs args = argparse::parse<PixelArtArgs>(argc, argv);
    if (args.verbose)
        args.print();


    std::cout << "Starting exemple program" << std::endl;
    std::cout << "Searching for file(s)..." << std::endl;

    std::vector<std::string> files;
    int file_number = 0;
    //Image contains Pixel Data
    sf::Image inputImage;
    
    if (std::filesystem::is_directory(args.src_path)) {
        for (auto& entry : std::filesystem::directory_iterator(args.src_path)) {
            if (entry.is_regular_file()) {
                if (inputImage.loadFromFile((entry.path().string()))) {
                    files.push_back(std::filesystem::absolute(entry.path()).string());
                }
            }
        }
    }
    else if (std::filesystem::is_regular_file(args.src_path)) {
        if (inputImage.loadFromFile(args.src_path)) {
            files.push_back(args.src_path);
        }
    }

    if (files.empty()) {
        std::cerr << "No image file found, exiting" << std::endl;
        return -1;
    }
    std::cout << "Loaded files" << std::endl;
    for (auto& file : files) std::cout << file << std::endl;
    // loading file

    if (!inputImage.loadFromFile(files[file_number])) {
        std::cerr << " Error reading first file" << std::endl;
        return -1;
    }

    sf::Vector2u dim = inputImage.getSize();

    /***************** RENDERING *************************/
    // Let's setup a window
    sf::RenderWindow window(sf::VideoMode(500, 500), "Pixel Art Exemple Program");
    sf::Texture texture;
    texture.loadFromImage(inputImage);
    sf::Sprite background(texture);
    sf::Vector2f oldPos;
    bool moving = false;

    float zoom = 1.0f;

    bool disp_background = true;
    mode = DISPLAY_GRAPH;

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
                if (event.key.code == sf::Keyboard::M) {
                    mode = static_cast<Mode>((static_cast<int>(mode) + 1) % static_cast<int>(Mode::NUM_MODES));
                    std::cout << "Switched to mode " << mode << std::endl;
                }
                if (event.key.code == sf::Keyboard::N) {
                    file_number = (file_number + 1) % files.size();
                    if (!inputImage.loadFromFile(files[file_number])) {
                        std::cerr << "A file has been deleted, exiting" << std::endl;
                        return -1;
                    }
                    texture.loadFromImage(inputImage);
                    background.setTexture(texture);
                }
                if (event.key.code == sf::Keyboard::B) {
                    disp_background = !disp_background;
                }
                if (event.key.code == sf::Keyboard::C) {
                    disp_color = (disp_color + 1) % 2;
                }
                if (event.key.code == sf::Keyboard::Y) {
                    if (event.key.code == sf::Keyboard::Up) {
                        args.yuv_similarity[0] += 1.0;
                    }
                    else if (event.key.code == sf::Keyboard::Down) {
                        args.yuv_similarity[0] -= 1.0;
                    }
                }

                if (event.key.code == sf::Keyboard::U) {
                    if (event.key.code == sf::Keyboard::Up) {
                        args.yuv_similarity[1] += 1.0;
                    }
                    else if (event.key.code == sf::Keyboard::Down) {
                        args.yuv_similarity[1] -= 1.0;
                    }
                }

                if (event.key.code == sf::Keyboard::V) {
                    if (event.key.code == sf::Keyboard::Up) {
                        args.yuv_similarity[2] += 1.0;
                    }
                    else if (event.key.code == sf::Keyboard::Down) {
                        args.yuv_similarity[2] -= 1.0;
                    }
                }

                if (event.key.code == sf::Keyboard::S) {
                    if (event.key.code == sf::Keyboard::Up) {
                        args.yuv_edges[0] += 1.0;
                    }
                    else if (event.key.code == sf::Keyboard::Down) {
                        args.yuv_edges[0] -= 1.0;
                    }
                }

                if (event.key.code == sf::Keyboard::Q) {
                    if (event.key.code == sf::Keyboard::Up) {
                        args.yuv_edges[1] += 1.0;
                    }
                    else if (event.key.code == sf::Keyboard::Down) {
                        args.yuv_edges[1] -= 1.0;
                    }
                }
            }
        }

        // Calculations
        auto param_pixel = pa::PixelGraphParam(inputImage, pa::ColorYUV(args.yuv_similarity[0], args.yuv_similarity[1], args.yuv_similarity[2]));
        pa::PixelGraph similarity(param_pixel);
        //Planarize the graph
        similarity.compute();

        auto param_voronoi = pa::EdgeDissimilarityParam(args.yuv_edges[0], args.yuv_edges[1]);
        pa::VoronoiDiagram diagram(param_voronoi);
        diagram.setGraph(similarity);
        diagram.compute();

        // Draw our simple scene
        window.clear(sf::Color(150, 150, 150));
        float scale = args.default_scale;
        if (disp_background) {
            background.setScale(sf::Vector2f(scale, scale));
            window.draw(background);
        }

        sf::Vertex line[2];
        line[0].color = sf::Color::Red;
        line[1].color = sf::Color::Red;


        switch (mode) {
        case Mode::DISPLAY_GRAPH:
        {
            auto& graph_edges = similarity.getGraph();
            for (int i = 0; i < dim.x; i++) {
                for (int j = 0; j < dim.y; j++) {
                    for (int k = 0; k < pa::NUM_DIR; k++) {
                        if (graph_edges[i][j][k]) {
                            auto dir = pa::VecDir[k];
                            line[0].position = scale * sf::Vector2f(i + 0.5f, j + 0.5f);
                            line[1].position = scale * sf::Vector2f(i + 0.5f + dir.x, j + 0.5f + dir.y);
                            window.draw(line, 2, sf::Lines);
                        }
                    }
                }
            }
            break;
        }
        case Mode::DISPLAY_VORONOI:
        {
            const pa::diagram& d = diagram.getDiagram();
            for (auto& vec : d) {
                for (auto& p : vec.second) {
                    line[0].position = scale * vec.first;
                    line[1].position = scale * p;
                    window.draw(line, 2, sf::Lines);
                }
            }
            break;
        }
        case Mode::DISPLAY_ACTIVE_EDGES:
        {
            const pa::edge_list& active_edges = diagram.getActiveEdges();
            for (auto& edge_color : active_edges) {
                auto& edge = edge_color.first;
                /*if (&(*active_edges.find(pa::Edge(edge.p2, edge.p1))) != &edge_color) {
                    std::cout << "Problem on edges hash table" <<std::endl;
                }*/
                auto& colors = edge_color.second.colors;
                line[0].position = scale * edge.p1;
                line[1].position = scale * edge.p2;
                line[0].color = colors[disp_color];
                line[1].color = colors[disp_color];

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
