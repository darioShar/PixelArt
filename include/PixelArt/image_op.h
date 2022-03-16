#pragma once
#include "graph.h"
#include <SFML/Graphics.hpp>

/* To do operation on graph / images */
namespace pa {

	// Default type for YUV manipulation.
	struct ColorYUV {
		float Y, U, V;

		ColorYUV() : Y(0.0), U(0.0), V(0.0) {}
		ColorYUV(float a, float b, float c) : Y(a), U(b), V(c) {}
		// Converts RGB to YUV
		void convertRGB(const sf::Color& c) {
			Y= (0.299 * c.r + 0.587 * c.g + 0.114 * c.b);
			U = 0.492 * (static_cast<float>(c.b) - Y);
			V = 0.877 * (static_cast<float>(c.r) - Y);
		}
	};

	// Pixel Graph parameters for similarity graph calculation
	// image on which to render, and YUV distance parameters.
	struct PixelGraphParam {
		const sf::Image& image;
		ColorYUV color;
		PixelGraphParam(const sf::Image& im, ColorYUV c_yuv = ColorYUV({ 42.0, 7.0, 6.0 }))
			: image(im), color(c_yuv) {}
	};

	// Functor to do the similarity test on adjacent pixel
	struct TestYUVSimilarity {
		using param_type = PixelGraphParam;
		using arg_type = IntPoint;
		using return_type = bool;

		param_type param; // parameters. Here it will be PixelGraphParam

		explicit TestYUVSimilarity(param_type param) :
			param(param)
		{}

		return_type operator()(const arg_type& p1, const arg_type& p2) const {
			ColorYUV c1; c1.convertRGB(param.image.getPixel(p1.x, p1.y));
			ColorYUV c2; c2.convertRGB(param.image.getPixel(p2.x, p2.y));
			return std::abs(c1.Y - c2.Y) < param.color.Y
				&& std::abs(c1.U - c2.U) < param.color.U
				&& std::abs(c1.V - c2.V) < param.color.V;
		}
	};

	// Parameters to determine edge type. Depends on YUV distance between
	// adjcent colors.
	struct EdgeDissimilarityParam {
		float shadingYUVDistance;
		float contourYUVDistance;
		EdgeDissimilarityParam(float a = 4.0/255.0, float b = 100.0/255.0) : shadingYUVDistance(a), contourYUVDistance(b) {}
	};

	// Functor to do edge type calculation.
	struct TestEdgeVisibility {
		using param_type = EdgeDissimilarityParam;
		using arg_type = sf::Color;
		using return_type = Visibility;

		param_type param;

		explicit TestEdgeVisibility(param_type param) :
			param(param)
		{}

		return_type operator()(const arg_type& rgb1, const arg_type& rgb2) const {
			ColorYUV c1; c1.convertRGB(rgb1);
			ColorYUV c2; c2.convertRGB(rgb2);
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
	// Op is the functor parametrized by param.
	template<class Op>
	class ImageOp {
		using param_type = typename Op::param_type;
		using arg_type = typename Op::arg_type;
		using return_type = typename Op::return_type;
		Op func;

	public:
		ImageOp(param_type param) : func(Op(param)) {}

		return_type operator()(const arg_type& a1, const arg_type& a2) const {
			return func(a1, a2);
		}

		param_type& getParam() {
			return func.param;
		}

		const param_type& getParam() const {
			return func.param;
		}

	};
}