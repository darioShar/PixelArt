#include <PixelArt/pixel_graph.h>
#include <utility>
#include <algorithm>
#include <numeric>
#include <stack>

namespace pa {


#define DECLARE_SQUARE_VARS(top_left) square s = get_square(top_left); IntPoint& bottom_right = s[1]; IntPoint& bottom_left = s[2]; IntPoint& top_right = s[3];

	PixelGraph::PixelGraph(const PixelGraphParam& p) : dim(p.image.getSize()), m_test_similarity(p)
	{
		init_graph();
	}

	PixelGraph::PixelGraph(PixelGraphParam&& p) : dim(p.image.getSize()), m_test_similarity(p)
	{
		init_graph();
	}


	PixelGraph::PixelGraph(const PixelGraph& g) : m_test_similarity(g.m_test_similarity.getParam())
	{
		m_graph = g.getGraph();
	}


	void PixelGraph::init_graph() {
		//Preallocating structures, initialize to zero
		const weight_per_dir temp_wpd = {}; // initialize to zero
		const std::vector< weight_per_dir > temp_vec(dim.y, temp_wpd);
		m_graph.resize(dim.x, temp_vec);

		// Adding edge for pixels with similar colors
		for (int i = 0; i < dim.x; i++) {
			for (int j = 0; j < dim.y; j++) {
				for (int k = 0; k < NUM_DIR; k++) {
					IntPoint current_pixel(i, j);
					IntPoint adj_pixel = current_pixel + VecDir[k];
					// if both pixel are sufficiently similar, weight = 1
					if (isValid(adj_pixel, dim))
						m_graph[i][j][k] = m_test_similarity(current_pixel, adj_pixel);
				}
			}
		}
	}

	square&& PixelGraph::get_square(const IntPoint& top_left) {
		return square{
		top_left,
		IntPoint(top_left + VecDir[Direction::BOTTOM_RIGHT]),
		IntPoint(top_left + VecDir[Direction::BOTTOM]),
		IntPoint(top_left + VecDir[Direction::RIGHT])
		};
	}


	bool PixelGraph::cross(const IntPoint& top_left) const {
		IntPoint bottom_left(top_left + VecDir[Direction::BOTTOM]);
		if (edge(top_left, Direction::BOTTOM_RIGHT)
			&& edge(bottom_left, Direction::TOP_RIGHT))
			return true;
		return false;
	}

	bool PixelGraph::check_additional_connection_and_remove_trivial_cross(const IntPoint& top_left)
	{
		DECLARE_SQUARE_VARS(top_left)

		int test = edge(top_left, Direction::BOTTOM)
			| edge(top_left, Direction::RIGHT) << 1
			| edge(bottom_left, Direction::RIGHT) << 2
			| edge(bottom_right, Direction::TOP) << 3;

		if (test == 15) {
			//All colors are same in the square, remove diagonal edges
			delete_edge(top_left, BOTTOM_RIGHT);
			delete_edge(bottom_right, TOP_LEFT);
			delete_edge(top_right, BOTTOM_LEFT);
			delete_edge(bottom_left, TOP_RIGHT);
		}

		return test > 0;
	}


	int PixelGraph::valence(const IntPoint& p)
	{
		//Count the edges around the pixel
		int cnt = 0;
		for (int i = 0; i < NUM_DIR; i++)
			if (isValid(p + VecDir[i], dim) && edge(p, static_cast<Direction>(i)))
				cnt++;
		return cnt;
	}

	//Checks for Isolated pixels
	void PixelGraph::islands_heuristic(const IntPoint& top_left)
	{
		DECLARE_SQUARE_VARS(top_left)

		int left_diag = 5 * (valence(top_left) == 1 + valence(bottom_right) == 1);
		m_graph[top_left.x][top_left.y][Direction::BOTTOM_RIGHT] += left_diag;
		m_graph[bottom_right.x][bottom_right.y][Direction::TOP_LEFT] += left_diag;

		int right_diag = 5 * (valence(top_right) == 1 + valence(bottom_left) == 1);
		m_graph[top_right.x][top_right.y][Direction::BOTTOM_LEFT] += right_diag;
		m_graph[bottom_left.x][bottom_left.y][Direction::TOP_RIGHT] += right_diag;

	}

	bool PixelGraph::are_dir_opposite(Direction d1, Direction d2) const {
		return VecDir[d1] + VecDir[d2] == sf::Vector2i(0, 0);
	}

	int PixelGraph::count_curve_edges(IntPoint a, const Direction d) {
		int curve_length = 1;
		IntPoint b(a + VecDir[d]);

		Direction actual_dir;
		// we must invert direction so 'a' knows it comes from 'b' at first step
		for (int i = 0; i < NUM_DIR; i++) {
			if (are_dir_opposite(static_cast<Direction>(i), d)) {
				actual_dir = static_cast<Direction>(i);
				break;
			}
		}

		// first, parcouring from 'a'
		while (valence(a) == 2 && a != b) {
			curve_length++;
			// finding next direction. Cannot be opposite to old direction d !
			int new_dir = 0;
			std::find_if(m_graph[a.x][a.y].begin(), m_graph[a.x][a.y].end(),
				[this, &actual_dir, &new_dir](int& dir_weight) mutable
				{return !are_dir_opposite(actual_dir, static_cast<Direction>(new_dir++)) && dir_weight > 0; });
			actual_dir = static_cast<Direction>(--new_dir);
			a += VecDir[actual_dir];
		}

		actual_dir = d;
		// Now from 'b'
		while (valence(b) == 2 && b != a) {
			curve_length++;
			// finding next direction. Cannot be opposite to old direction d !
			int new_dir = 0;
			std::find_if(m_graph[b.x][b.y].begin(), m_graph[b.x][b.y].end(),
				[this, &actual_dir, &new_dir](int& dir_weight) mutable
				{return !are_dir_opposite(actual_dir, static_cast<Direction>(new_dir++)) && dir_weight > 0; });
			actual_dir = static_cast<Direction>(--new_dir);
			b += VecDir[actual_dir];
		}

		return curve_length;

	}

	//Curves Heuristic
	void PixelGraph::curves_heuristic(const IntPoint& top_left)
	{
		DECLARE_SQUARE_VARS(top_left)

		// top_left to bottom_right
		int left_curve_length = count_curve_edges(top_left, BOTTOM_RIGHT);
		m_graph[top_left.x][top_left.y][Direction::BOTTOM_RIGHT] += left_curve_length;
		m_graph[bottom_right.x][bottom_right.y][Direction::TOP_LEFT] += left_curve_length;

		// top_right to bottom_left
		int right_curve_length = count_curve_edges(top_right, BOTTOM_LEFT);
		m_graph[top_right.x][top_right.y][Direction::BOTTOM_LEFT] += right_curve_length;
		m_graph[bottom_left.x][bottom_left.y][Direction::TOP_RIGHT] += right_curve_length;

	}


	//Checks if (x,y) are inclusively inside the given cell
	bool insideBounds(const IntPoint& p, int row_st, int row_end, int col_st, int col_end)
	{
		return (p.x >= row_st && p.x <= row_end && p.y >= col_st && p.y <= col_end);
	}

	// DFS on 8*8 limited grid. num_label is number of the labels to put on start point
	// connex component. top_left point so we can know where to center the grid
	// grid labels must be 0 to consider that is not yet in any component
	void PixelGraph::DFS_grid_limited(const IntPoint& top_left, const IntPoint& start, int grid[8][8], int num_label) {
		std::vector<IntPoint> stack;
		stack.reserve(48);
		stack.push_back(start);
		while (!stack.empty())
		{
			IntPoint point = stack.back();
			stack.pop_back();
			for (int i = 0; i < NUM_DIR; i++)
			{
				//See in all directions, scan for points that are in the not yet visited, 
				// in the 8x8 box, and have an edge from the current point.
				if (!m_graph[point.x][point.y][i])
					continue;

				IntPoint nextPoint(point + VecDir[i]);
				if (!insideBounds(nextPoint, top_left.x - 3, top_left.x + 4, top_left.y - 3, top_left.y + 4))
					continue;

				IntPoint gridPoint(nextPoint - top_left + IntPoint(3, 3));
				if (grid[gridPoint.x][gridPoint.y] != 0) continue;
				grid[gridPoint.x][gridPoint.y] = num_label;
				stack.push_back(nextPoint);
			}
		}
	}

	void PixelGraph::sparse_pixels_heuristic(const IntPoint& top_left)
	{
		DECLARE_SQUARE_VARS(top_left)

		int grid[8][8] = { 0 };
		grid[3][3] = 1;
		DFS_grid_limited(top_left, top_left, grid, 1);
		grid[4][3] = 2;
		DFS_grid_limited(top_left, top_right, grid, 2);


		//Find the size of the components
		int componentA = 0;
		int componentB = 0;

		for (int i = 0; i < 8; i++)
			for (int j = 0; j < 8; j++)
			{
				if (grid[i][j] == 1) componentA++;
				else if (grid[i][j] == 2) componentB++;
			}

		// end

		m_graph[top_left.x][top_left.y][Direction::BOTTOM_RIGHT] += componentA;
		m_graph[bottom_right.x][bottom_right.y][Direction::TOP_LEFT] += componentA;

		m_graph[top_right.x][top_right.y][Direction::BOTTOM_LEFT] += componentB;
		m_graph[bottom_left.x][bottom_left.y][Direction::TOP_RIGHT] += componentB;
	}



	void PixelGraph::compute()
	{
		//For Internal Pixels, process via heuristic if edges are crossing
		//A Pixel is the topLeft of a 2x2 box
		// 
		//Take current pixel as top-left of a 4 pixel square and check whether the colors are same in all
		for (int i = 0; i < dim.x - 1; i++) {
			for (int j = 0; j < dim.y - 1; j++)
			{
				IntPoint top_left(i, j);
				DECLARE_SQUARE_VARS(top_left)

				if (cross(top_left)) {
					if (check_additional_connection_and_remove_trivial_cross(top_left))
						continue;

					//Run heuristics and update weights
					islands_heuristic(top_left);
					curves_heuristic(top_left);
					sparse_pixels_heuristic(top_left);

					//Remove lighter edge, or both if equality (no else if)
					if (m_graph[top_left.x][top_left.y][BOTTOM_RIGHT] <= m_graph[top_right.x][top_right.y][BOTTOM_LEFT])
					{
						delete_edge(top_left, BOTTOM_RIGHT);
						delete_edge(bottom_right, TOP_LEFT);
					}
					if (m_graph[top_left.x][top_left.y][BOTTOM_RIGHT] >= m_graph[top_right.x][top_right.y][BOTTOM_LEFT])
					{
						delete_edge(top_right, BOTTOM_LEFT);
						delete_edge(bottom_left, TOP_RIGHT);
					}

				}
			}
		}
	}


	// Returns true if there is an edge from (x,y) in kth direction
	bool PixelGraph::edge(int x, int y, Direction k) const
	{
		return m_graph[x][y][static_cast<int>(k)] != 0;
	}

	bool PixelGraph::edge(const IntPoint& p, Direction k) const
	{
		if (isValid(p, dim))
			return edge(p.x, p.y, k);
		return false;
	}

	// Deletes edge
	void PixelGraph::delete_edge(int x, int y, Direction k) {
		m_graph[x][y][static_cast<int>(k)] = 0;
	}

	void PixelGraph::delete_edge(const IntPoint& p, Direction k)
	{
		delete_edge(p.x, p.y, k);
	}
}