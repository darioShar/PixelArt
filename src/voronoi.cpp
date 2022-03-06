#include <PixelArt/voronoi.h>
#include <iostream>
#include <utility>
#include <numeric>

namespace depix {

	// VORONOI DIAGRAM CALCULATION
		// Each corner can do three things:
		// 1. Gain space from other cells (via type 1 and 2 points) (if an edge exists in that direction)
		// 2. Lose space to other cells (via type 3) (if an external edge cuts that corner)
		// 3. Keep the same space (there are no edges) (original corner point)
		// All the edge midpoints are kept to complete the diagram
		// Edges through the edge midpoints don't affect this diagram
		// This addition of points is done in anti-clowise order
		// TOPLEFT -> LEFT -> BOTTOMLEFT -> BOTTOM -> BOTTOMRIGHT -> RIGHT -> TOPRIGHT -> TOP

		// Going in this fashion :
	// 0 1 2
	// 7 8 3
	// 6 5 4
	// checking diagonals of quadrant1 = 0178
	// checking diagonals of quadrant2 = 1283
	// checking diagonals of quadrant3 = 7865
	//...
	bool checkCellType(voronoiCellType type)
	{
		return !(((type & 0b11) == 0b11)
			|| (type & (0b11 << 2)) == (0b11 << 2)
			|| (type & (0b11 << 4)) == (0b11 << 4)
			|| (type & (0b11 << 6)) == (0b11 << 6));
	}


	voronoiCell CellsCalculation::generateCellByType(voronoiCellType type) {
		voronoiCell cell;

		#define CUSTOM_PARCOURS_EDGE(i) (type & 1<<i)

		//TOPLEFT
		if (CUSTOM_PARCOURS_EDGE(0))
		{
			cell.emplace_back(-0.25, -0.75); // 1
			cell.emplace_back(-0.75, -0.25); // 2
		}
		else if (CUSTOM_PARCOURS_EDGE(1))
			cell.emplace_back(-0.25, -0.25); // 3
		else cell.emplace_back(-0.5, -0.5); // 4

		//LEFT
		cell.emplace_back(-0.5, 0.0); // Mid-point

		//BOTTOMLEFT
		if (CUSTOM_PARCOURS_EDGE(7))
		{
			cell.emplace_back(-0.75, +0.25); // 1
			cell.emplace_back(-0.25, +0.75); // 2
		}
		else if (CUSTOM_PARCOURS_EDGE(6))
			cell.emplace_back(-0.25, +0.25); // 3
		else cell.emplace_back(-0.5, +0.5); // 4

		//BOTTOM
		cell.emplace_back(0.0, +0.5); // Mid-point

		//BOTTOMRIGHT
		if (CUSTOM_PARCOURS_EDGE(4))
		{
			cell.emplace_back(+0.25, +0.75); // 1
			cell.emplace_back(+0.75, +0.25); // 2
		}
		else if (CUSTOM_PARCOURS_EDGE(5))
			cell.emplace_back(+0.25, +0.25); // 3
		else cell.emplace_back(+0.5, +0.5); // 4

		//RIGHT
		cell.emplace_back(+0.5, 0.0); // Mid-point

		//TOPRIGHT
		if (CUSTOM_PARCOURS_EDGE(3))
		{
			cell.emplace_back(+0.75, -0.25); // 1
			cell.emplace_back(+0.25, -0.75); // 2
		}
		else if (CUSTOM_PARCOURS_EDGE(2))
			cell.emplace_back(+0.25, -0.25); // 3
		else cell.emplace_back(+0.5, -0.5); // 4

		//TOP
		cell.emplace_back(0.0, -0.5); // Mid-point

		return cell;
	}

	void CellsCalculation::generateAllCellTypes() {
		std::generate(possibleCells.begin(), possibleCells.end(),
			[this, i = 0]() mutable{return generateCellByType(i++); });
	}
	// declare
	CellsCalculation VoronoiDiagram::cellsCalculation;

	VoronoiDiagram::VoronoiDiagram() : m_graph(nullptr)
	{
		// instantiate calculations
		(void)cellsCalculation;
	
	}

	void VoronoiDiagram::setGraph(const PixelGraph& graph) {
		m_graph = &graph;
		sf::Vector2u dim = graph.getImage().getSize();

		for (int i = 0; i < dim.x; i++)
		{
			std::vector<voronoiCell> v2(0);
			m_voronoiPoints.push_back(v2);
		}
	}

	const possible_cells_list VoronoiDiagram::getPossibleVoronoiCells() const {
		return cellsCalculation.possibleCells;
	}


	voronoiCellType VoronoiDiagram::extractType(const IntPoint& p) const {
		// warning : for_each takes a copy of the functor... so better with lambda function and reference
		voronoiCellType result = 0;
		std::for_each(node_parcours.begin(), node_parcours.end(), [this, &p, &result, edges_seen = 0](Direction& dir) mutable
		{
			if (m_graph->edge(p + VecDir[dir], edges_parcours[edges_seen % 2]))
				result |= (1 << edges_seen);
			edges_seen++;
		});
		return result;
	}

	void VoronoiDiagram::generateAccurateDiagram() {
		sf::Vector2u dim = m_graph->getImage().getSize();
		for (int x = 0; x < dim.x; x++) {
			for (int y = 0; y < dim.y; y++) {
				voronoiCellType type = extractType(IntPoint(x, y));
				if (!isOnBounds(IntPoint(x, y), dim) && !checkCellType(type)) {
					std::cout << "Error in similarity graph : " << x << ", " << y <<  std::endl;
					std::cout << "configuration : ";
					for (int i = 0; i < 8; i++) {
						std::cout << !!(type & 1 << i) << " : ";
					}
					std::cout << std::endl;
				}
				voronoiCell cell = cellsCalculation.possibleCells[type];
				for (auto& p : cell) p += Point(x + 0.5, y + 0.5);
				m_voronoiPoints[x].push_back(cell);
				// Populate hash table 
				for (auto& p : m_voronoiPoints[x][y]) {
					if (m_valency.find(p) == m_valency.end())
						m_valency[p] = 1;
					else
						m_valency[p]++;
				}

			}
		}
	}

	void VoronoiDiagram::simplifyDiagram() {
		sf::Vector2u dim = m_graph->getImage().getSize();
		for (int x = 0; x < dim.x; x++) {
			for (int y = 0; y < dim.y; y++) {
				// create diagram, removing valence 2
				int a = 0;
				int b = 1;
				int size = m_voronoiPoints[x][y].size();
				if (!size) continue;
				do {
					auto& pa = m_voronoiPoints[x][y][a];
					auto& pb = m_voronoiPoints[x][y][b];
					if (m_valency[pb] != 2 ) {
						if (m_diagram.find(pa) == m_diagram.end()) {
							m_diagram[pa] = std::vector<Point>({ pb });
						}
						else {
							m_diagram[pa].push_back(pb);
						}
						a = b;
						b = (b + 1) % size;
					}
					else {
						b = (b + 1) % size;
					}
				} while (b != 0);
				m_diagram[m_voronoiPoints[x][y][a]].push_back(m_voronoiPoints[x][y][b]);
			}
		}
	}

	void VoronoiDiagram::createDiagram()
	{
		generateAccurateDiagram();
		simplifyDiagram();
	}

	diagram VoronoiDiagram::getDiagram() {
		return m_diagram;
	}


}



/*void calculate_constants() {
		std::copy(node_parcours.begin(), node_parcours.end(), VecDir.begin());
		node_parcours[8] = IntPoint(0, 0);
		edges_parcours = std::array<CircleDir, 9>( {
			std::make_pair<Direction, Direction>(RIGHT, BOTTOM),
				std::make_pair<Direction, Direction>(RIGHT, LEFT),
				std::make_pair<Direction, Direction>(BOTTOM, LEFT),
				std::make_pair<Direction, Direction>(BOTTOM, TOP),
				std::make_pair<Direction, Direction>(LEFT, TOP),
				std::make_pair<Direction, Direction>(LEFT, RIGHT),
				std::make_pair<Direction, Direction>(TOP, RIGHT),
				std::make_pair<Direction, Direction>(TOP, BOTTOM),
				std::make_pair<Direction, Direction>(TOP_LEFT, LEFT)
		});

		for (int i = 0; i < 9; i++) {
			node_index_start[i] = std::accumulate(edges_parcours.begin(), edges_parcours.begin() + i,
				0, [](CircleDir& range) {
					return range.second > range.first ?
						range.second - range.first :
						(NUM_DIR - range.first) + range.second + 1;
				});
		}
	}*/