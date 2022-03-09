#pragma once

#include <unordered_map>
#include "graph.h"
#include <iostream>

/* Strategy :
* Constructing cell : easy heuristic, just check connected neighbouring nodes
* Collapsing 2-valence nodes : std::unordered_map, same on memory, constant access time, 
* no need to aprcours it and we prefer accessing element by its position in grid. 
* 
* Computing on beginning of program through static varibale all possible cell variations.

*/

namespace depix {

	using voronoiCell = std::vector<Point>;
	using voronoiCellType = uint8_t; // 8 bits
	using CircleDir = std::pair<Direction, Direction>;
	using diagram = std::unordered_map<Point, std::vector<Point>>;

	using diagram = std::unordered_map<Point, std::vector<Point>>;
	using edge_list = std::unordered_map<Edge, std::vector<sf::Color>>;


	// Going in this fashion :
	// 0 1 2
	// 7 8 3
	// 6 5 4
	// checking diagonals of quadrant1 = 0178
	// checking diagonals of quadrant2 = 1283
	// checking diagonals of quadrant3 = 7865
	//...
	static std::array<Direction, 8> node_parcours{
		TOP_LEFT,
		LEFT,
		TOP,
		CENTER,
		CENTER,
		BOTTOM,
		LEFT,
		BOTTOM_LEFT
	};

	static std::array<Direction, 2> edges_parcours{BOTTOM_RIGHT, TOP_RIGHT};

#define TOTAL_VORONOI_CELLS 256
	using possible_cells_list = std::array<voronoiCell, TOTAL_VORONOI_CELLS>;

	// Checks if cell type can be in similarity graph
	bool checkCellType(voronoiCellType type);

	struct CellsCalculation {

		possible_cells_list possibleCells;

		// We can see on the algorithm that we have 3^4 combinations possible
		// for voronoi cell generation. If we want to generate all 2 valence possible
		// cimbinations, we have (3^4)^8=3^32 (8 adjacent cells) that is too much.
		// so only generating Cell by type then 2-valence reduction.
		voronoiCell generateCellByType(voronoiCellType type);

		void generateAllCellTypes();
	public:
		CellsCalculation() {
			generateAllCellTypes();
		}
		~CellsCalculation() {
			
		}
	};

	class VoronoiDiagram {

		const PixelGraph* m_graph;

		//Voronoi points around each pixels starting from top_left, trigonometric parcours
		std::vector<std::vector<voronoiCell>> m_voronoiPoints;

		// Store each voronoiCell possible configurations 2^8
		// In reality 3^4 since no crossing but well
		static CellsCalculation cellsCalculation;

		// Valency of each voronoi point for collapsing
		std::unordered_map<Point, int> m_valency;

		// Final diagram
		diagram m_diagram;

		// active edges
		edge_list m_active_edges;

		ColorImageOp<TestEdgeVisibility> m_test_visibility;

		voronoiCellType extractType(const IntPoint& p) const;

		void checkAndAddActiveEdge(Point& pa, Point& pb, int x, int y);

		void generateAccurateDiagram();

		void simplifyDiagram();

		void deleteNonActiveEdges();

	public:
		VoronoiDiagram(EdgeDissimilarityParam p = EdgeDissimilarityParam());

		void setGraph(const PixelGraph& graph);

		const possible_cells_list getPossibleVoronoiCells() const;

		//Creates Voronoi Diagram
		void createDiagram();

		diagram getDiagram();

		edge_list getActiveEdges();

		const PixelGraph* getGraph() { return m_graph; };
	};

	// Declare
	//CellsCalculation VoronoiDiagram::cellsCalculation;
}