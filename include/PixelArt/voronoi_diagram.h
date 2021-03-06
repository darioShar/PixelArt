#pragma once

#include <unordered_map>
#include <iostream>
#include <PixelArt/pixel_graph.h>

/* Strategy :
* Constructing cell : easy heuristic, just check connected neighbouring nodes
* Collapsing 2-valence nodes : std::unordered_map, same on memory, constant access time, 
* no need to aprcours it and we prefer accessing element by its position in grid. 
* 
* Computing on beginning of program through static varibale all possible cell variations.

*/

namespace pa {

	// Some types
	using voronoiCell = std::vector<Point>;
	using voronoiCellType = uint8_t; // 8 bits

	using diagram = std::unordered_map<Point, std::vector<Point>>;

	struct EdgeProperties {
		std::vector<sf::Color> colors;
		Visibility v;
	};

	using edge_list = std::unordered_map<Edge, EdgeProperties>;


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


	// static struct will let us laucnh initial calculation on the first instantiation of the
	// Voronoi Diagram class. It will calculate all 81 possible cells and store them.
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


	// Usage : will take a PixelGraph, extract its internal graph and then do all the calculations
	// resulting in getting : 
	// - a diagram which is adjcency list of all the points obtained after voronoi cell calculation
	// and reduction of valence 2 nodes
	// - an edge list which contains all active edges as defined by the parameters given on VoronoiDiagram construction,
	// or set later on.
	class VoronoiDiagram {
		// Store each voronoiCell possible configurations 2^8
		// In reality 3^4 since no crossing but well
		static CellsCalculation cellsCalculation;

		// initial pixel graph
		const PixelGraph* m_graph;

		//Voronoi points around each pixels starting from top_left, trigonometric parcours
		std::vector<std::vector<voronoiCell>> m_voronoiPoints;

		// Valency of each voronoi point for collapsing
		std::unordered_map<Point, int> m_valency;

		// Final diagram
		diagram m_diagram;

		// active edges
		edge_list m_active_edges;

		// Functor for edge decision
		ImageOp<TestEdgeVisibility> m_test_visibility;

		// Determine cell type on point p
		voronoiCellType extractType(const IntPoint& p) const;

		// Calling this function over the whole voronoi diagram will
		// determine the active edges 
		void checkAndAddActiveEdge(Point& pa, Point& pb, int x, int y);

		// generate diagram without valence 2 reduction
		void generateAccurateDiagram();

		// valence 2 reduction and additional calculation to determine active edges
		void simplifyDiagram();

		// Called remove Edges calssified as None.
		void deleteNonActiveEdges();

	public:
		// Constructor
		VoronoiDiagram(EdgeDissimilarityParam p = EdgeDissimilarityParam());

		// Give pixel graph on which calculation will be done
		void setGraph(const PixelGraph& graph);

		// Give new set of parameter for active edge determination
		void setParam(const EdgeDissimilarityParam& p);

		// Computes simplfiied voronoi diagram and determines active edges.
		void compute();

		// get computed diagram
		const diagram& getDiagram();

		// get computed list of active edges
		const edge_list& getActiveEdges();

		// get underlying graph
		const PixelGraph* getGraph() { return m_graph; };

		// get all possible voronoi cell variation; Accurate reprensentation, not simplified.
		const possible_cells_list& getPossibleVoronoiCells() const;
	};

}