#pragma once

#include "image_op.h"
#include "graph.h"

namespace pa {
	// weight is 0 if no edge, > 0 otherwise
	using weight_per_dir = std::array<int, NUM_DIR>;
	using pixel_graph_edges = std::vector<std::vector<weight_per_dir>>;
	using square = std::array<IntPoint, 4>;


	//Checks if the requested pixel is in range of the image
	template<typename T, typename U>
	bool isValid(const T& p, const U& dim)
	{
		return (p.x >= 0 && p.x < dim.x&& p.y >= 0 && p.y < dim.y);
	}


	// Checks if pixel is on bounds defined by dim vector
	template<typename T, typename U>
	bool isOnBounds(const T& p, const U& dim)
	{
		return (p.x == 0 || p.x == (dim.x - 1) || p.y == 0 || p.y == (dim.y - 1));
	}


	// PixelGraph class.  Does similarity calculations.
	class PixelGraph
	{
		// Dimensions of the image we are working on
		sf::Vector2u dim;

		// Functor testing similarity between two points
		ImageOp<TestYUVSimilarity> m_test_similarity;

		// graph[i][j][k] -> denotes whether there is a an edge from (i,j) in kth direction in the graph
		pixel_graph_edges m_graph;

		// Just helper function to initialize m_graph
		void init_graph();

		// get square pixels position form top_left position
		// in this order : top_left, bottom_right, bottom_left, top_right
		square&& get_square(const IntPoint& top_left);

		// checks for cross from top_left pixel
		bool cross(const IntPoint& top_left) const;

		//For removing trivial cross.
		//Returns true if there is at least one more connection than diagonal
		// assumes diagonal is already present
		bool check_additional_connection_and_remove_trivial_cross(const IntPoint& p);

		//How many edges for the pixel (x,y)
		int valence(const IntPoint& p);

		// are directions opposite ?
		bool are_dir_opposite(Direction d1, Direction d2) const;

		// parcours 2-valence curve from 2 adjacent points. Will stop on end of 
		// curve or beginning of cycle. returns curve length (number of edges)
		int count_curve_edges(IntPoint a, const Direction d);

		// DFS on 8*8 limited grid. num_label is number of the labels to put on start point
		// connex component.
		void DFS_grid_limited(const IntPoint& top_left, const IntPoint& start, int grid[8][8], int num_label);

		//Heuristics for features
		void curves_heuristic(const IntPoint& top_left);
		void sparse_pixels_heuristic(const IntPoint& top_left);
		void islands_heuristic(const IntPoint& top_left);

	public:
		PixelGraph(const PixelGraphParam& p);
		PixelGraph(PixelGraphParam&& p);
		PixelGraph(const PixelGraph& g);

		// Compute the similarity graph as defined in the paper.
		// Will look through all pixels and determine who is connected to who
		// by similarity parameters defined in YUVGraphParam.
		// The resulting graph will be stored in m_graph
		void compute();

		//Accessors, return const reference so no unnecessary copies are made
		const sf::Image& getImage() const { return m_test_similarity.getParam().image; }
		const pixel_graph_edges& getGraph() const { return m_graph; }

		// Returns true if there is an edge from (x,y) in kth direction
		bool edge(int x, int y, Direction k) const;
		bool edge(const IntPoint& p, Direction k) const;
		// Deletes edge
		void delete_edge(int x, int y, Direction k);
		void delete_edge(const IntPoint& p, Direction k);
	};
}