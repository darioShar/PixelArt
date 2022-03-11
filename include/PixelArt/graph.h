#pragma once

#include <array>
#include <SFML/Graphics.hpp>
#include "hash.h"

/* Basic types for graph manipulation*/

namespace pa {

	// First, introduce default types
	using IntPoint = sf::Vector2i;
	using Point = sf::Vector2f;

	// lexical order
	static bool operator<(const Point& pa, const Point& pb) {
		return (pa.x < pb.x) || (pa.y < pb.y);
	}

	// Default structure for Edge
	struct Edge {
		Point p1;
		Point p2;

		Edge(const Point& pa, const Point& pb) {
			if (pa < pb) {
				p1 = pa;
				p2 = pb;
			}
			else {
				p1 = pb;
				p2 = pa;
			}
		}
		bool operator==(const Edge& e) const {
			return p1 == e.p1 && p2 == e.p2;
		}
	};

	// Edge visibility
	enum Visibility {
		None,
		Shading,
		Contour
	};

	// Default hasher
	// using EdgeHasher = PairHasher<Point, Point>; // no need

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
}

// Hash functions

namespace std {
	template <> struct hash<pa::Point>
	{
		size_t operator()(const pa::Point& p) const
		{
			return ((hash<float>()(p.x)
				^ (hash<float>()(p.y) << 1)) >> 1);
		}
	};

	template <> struct hash<pa::Edge>
	{
		size_t operator()(const pa::Edge& e) const
		{
			return pa::PairHasher<pa::Point, pa::Point>()(std::pair(e.p1, e.p2));
		}
	};
}