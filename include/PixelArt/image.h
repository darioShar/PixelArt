#pragma once

#include <SFML/Graphics.hpp>
#include <array>
#include <vector>
#include <ctype.h>

using IntPoint = sf::Vector2i;
enum Direction : int {
	TOP_LEFT = 0,
	TOP,
	TOP_RIGHT,
	LEFT,
	RIGHT,
	BOTTOM_LEFT,
	BOTTOM,
	BOTTOM_RIGHT,
	NUM_DIR
};

const std::array<sf::Vector2i, NUM_DIR> VecDir
{
	sf::Vector2i(-1,-1),
	sf::Vector2i(0,-1),
	sf::Vector2i(1,-1),
	sf::Vector2i(-1,0),
	sf::Vector2i(1,0),
	sf::Vector2i(-1,1),
	sf::Vector2i(0,1),
	sf::Vector2i(1,1)
};

struct ColorYUV {
	double Y, U, V;
};

static ColorYUV RGBtoYUV(const sf::Color& c) {
	double y = 0.299 * c.r + 0.587 * c.g + 0.114 * c.b;
	double u = 0.492 * (c.b - y);
	double v = 0.877 * (c.r - y);
	return ColorYUV{ y, u, v };
}

class ColorPixel {
	const sf::Image& m_image;
	int x, y;

public:
	explicit ColorPixel(const sf::Image& im, int x, int y) :
		m_image(im),
		x(x), y(y)
	{}

	bool similar(const ColorPixel& cp) const {
		ColorYUV c1 = RGBtoYUV(m_image.getPixel(x, y));
		ColorYUV c2 = RGBtoYUV(cp.m_image.getPixel(cp.x, cp.y));
		return std::abs(c1.Y - c2.Y) < 48.0
			&& std::abs(c1.U - c2.U) < 7.0
			&& std::abs(c1.V - c2.V) < 6.0;
	}
};

class ColorImageOp {
	const sf::Image& m_image;

public :
	ColorImageOp(const sf::Image& im) : m_image(im) {}

	const ColorPixel operator[](const IntPoint& p) const {
		return ColorPixel(m_image, p.x, p.y);
	}

};
