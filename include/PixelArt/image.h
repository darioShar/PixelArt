#pragma once

#include <SFML/Graphics.hpp>
#include <array>
#include <vector>
#include <ctype.h>

namespace depix {
	using IntPoint = sf::Vector2i;
	using Point = sf::Vector2f;

	static bool operator<(const Point& pa, const Point& pb) {
		return (pa.x < pb.x) || (pa.y < pb.y);
	}

	enum Visibility {
		None,
		Shading,
		Contour
	};

	struct Edge {
		Point p1;
		Point p2;
		Visibility v;

		Edge(const Point& pa, const Point& pb) : v(None) {
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
		float Y, U, V;
	};

	static ColorYUV RGBtoYUV(const sf::Color& c) {
		float y = 0.299 * c.r + 0.587 * c.g + 0.114 * c.b;
		float u = 0.492 * (c.b - y);
		float v = 0.877 * (c.r - y);
		return ColorYUV{ y, u, v };
	}

	// Graph similarity
	struct YUVGraphParam {
		const sf::Image& image;
		float Y = 42.0;
		float U = 7.0;
		float V = 6.0;
		YUVGraphParam(const sf::Image& im) : image(im) {}
	};

	struct TestYUVGraph {
		using param_type = YUVGraphParam;
		using arg_type = IntPoint;
		using return_type = bool;

		param_type param;

		explicit TestYUVGraph(const param_type& param) :
			param(param)
		{}

		return_type operator()(const arg_type& p1, const arg_type& p2) const {
			ColorYUV c1 = RGBtoYUV(param.image.getPixel(p1.x, p1.y));
			ColorYUV c2 = RGBtoYUV(param.image.getPixel(p2.x, p2.y));
			return std::abs(c1.Y - c2.Y) < param.Y
				&& std::abs(c1.U - c2.U) < param.U
				&& std::abs(c1.V - c2.V) < param.V;
		}
	};

	// Test curve visibility

	struct EdgeDissimilarityParam {
		float shadingYUVDistance = 4.0 / 255.0;
		float contourYUVDistance = 100.0 / 255.0;
	};

	struct TestEdgeVisibility {
		using param_type = EdgeDissimilarityParam;
		using arg_type = sf::Color;
		using return_type = Visibility;

		param_type param;

		explicit TestEdgeVisibility(const param_type& param) :
			param(param)
		{}

		return_type operator()(const arg_type& rgb1, const arg_type& rgb2) const {
			ColorYUV c1 = RGBtoYUV(rgb1);
			ColorYUV c2 = RGBtoYUV(rgb2);
			float val = (c1.Y - c2.Y) * (c1.Y - c2.Y)
				+ (c1.U - c2.U) * (c1.U - c2.U)
				+ (c1.V - c2.V) * (c1.V - c2.V);
			if (val < param.shadingYUVDistance)
				return None;
			if (val < param.contourYUVDistance)
				return Shading;
			return Contour;
		}

	};


	// For operations on images, similarity/dissimilarity etc;
	// Later we'll let user tune parameters
	// Op = Operation Functor class acting on Args = argument types
	// Op funcotr parameter given by parameters.
	template<class Op>
	class ColorImageOp {
		using param_type = typename Op::param_type;
		using arg_type = typename Op::arg_type;
		using return_type = typename Op::return_type;
		Op func;

	public:
		ColorImageOp(param_type param) : func(Op(param)) {}

		return_type operator()(const arg_type& a1, const arg_type& a2) const {
			return func(a1, a2);
		}

		param_type getParam() const {
			return func.param;
		}

	};

}

// HASH

namespace depix {
	/*struct PointHasher {
		size_t operator()(const Point& p) const
		{
			return ((std::hash<float>()(p.x)
				^ (std::hash<float>()(p.y) << 1)) >> 1);
		}
	};*/

	template<typename T1, typename T2, template <class, class> class PairType = std::pair>
	struct PairHasher {
		size_t operator()(PairType<T1, T2>&& p) const
		{
			return ((std::hash<T1>()(p.first)
				^ (std::hash<T2>()(p.second) << 1)) >> 1);
		}
	};


	/*template <class T1, class T2>
	struct PairHasher
	{
		std::size_t operator() (const std::pair<T1, T2>& pair) const {
			return std::hash<T1>()(pair.first) ^ std::hash<T2>()(pair.second);
		}
	};*/
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

	template <> struct hash<depix::Edge>
	{
		size_t operator()(const depix::Edge& e) const
		{
			return depix::PairHasher<depix::Point, depix::Point>()(std::pair(e.p1, e.p2));
		}
	};	
}