#pragma once

#include <SFML/Graphics.hpp>
#include <array>
#include <vector>
#include <ctype.h>

namespace depix {
	using IntPoint = sf::Vector2i;
	using Point = sf::Vector2f;

	using edge_color = std::pair<sf::Color, sf::Color>;
	using edge = std::pair<Point, Point>;

	/*struct PointHasher {
		size_t operator()(const Point& p) const
		{
			return ((std::hash<float>()(p.x)
				^ (std::hash<float>()(p.y) << 1)) >> 1);
		}
	};*/

	template<typename T1, typename T2>
	struct PairHasher {
		size_t operator()(const std::pair<T1, T2>& p) const
		{
			return ((std::hash<T1>()(p.first)
				^ (std::hash<T2>()(p.second) << 1)) >> 1);
		}
	};

	using EdgeHasher = PairHasher<Point, Point>;

	struct EdgeCompare {
		bool operator()(const edge& e1, const edge& e2) const {
			return (e1 == e2) || ((e1.first == e2.second) && (e1.second == e2.first));
		}
	};


	/*template <class T1, class T2>
	struct PairHasher
	{
		std::size_t operator() (const std::pair<T1, T2>& pair) const {
			return std::hash<T1>()(pair.first) ^ std::hash<T2>()(pair.second);
		}
	};*/

	// in clockwise order (important for later, in voronoi !!!)
	enum Direction : int {
		TOP_LEFT = 0,
		LEFT,
		BOTTOM_LEFT,
		BOTTOM,
		BOTTOM_RIGHT,
		RIGHT,
		TOP_RIGHT,
		TOP,
		CENTER
	};
#define NUM_DIR Direction::CENTER

	const std::array<IntPoint, NUM_DIR + 1> VecDir
	{
		sf::Vector2i(-1,-1),
		sf::Vector2i(-1,0),
		sf::Vector2i(-1,1),
		sf::Vector2i(0,1),
		sf::Vector2i(1,1),
		sf::Vector2i(1,0),
		sf::Vector2i(1,-1),
		sf::Vector2i(0,-1),
		sf::Vector2i(0,0),
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

	public:
		ColorImageOp(const sf::Image& im) : m_image(im) {}

		const ColorPixel operator[](const IntPoint& p) const {
			return ColorPixel(m_image, p.x, p.y);
		}

	};
}

namespace std {
	template <> struct hash<depix::Point>
	{
		size_t operator()(const depix::Point& p) const
		{
			return ((hash<float>()(p.x)
				^ (hash<float>()(p.y) << 1)) >> 1);
		}
	};
	
}