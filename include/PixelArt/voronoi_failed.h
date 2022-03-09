#pragma once

#include <unordered_map>
#include "graph.h"
#include <iostream>
#include <assert.h>

/* Strategy :
* Constructing cell : easy heuristic, just check connected neighbouring nodes
* Collapsing 2-valence nodes : std::unordered_map, same on memory, constant access time, 
* no need to aprcours it and we prefer accessing element by its position in grid. 
* 
* Better stragey : simplified voronoi cell is determined locally by connections in the 3*3 grid
* centered on evaluated node. We first compute all possible combinations etc; Bit complicated. Must
* do it statically. First, simple strategy.
* We'll keep first strategy for nodes on extremities, not contained in 3*3 grids.

*/

namespace depix {

	using voronoiCell = std::vector<Point>;

	using voronoiCellType = uint8_t; // 8 bits
	using CircleDir = std::pair<Direction, Direction>;

	using diagram_hash_map = std::unordered_map<Point, std::vector<Point>, PointHasher>;
	using diagram = std::vector<std::pair<Point, std::vector<Point>>>;


	// Going in this fashion :
	// 0 1 2
	// 7 8 3
	// 6 5 4
	// checking diagonals of quadrant1 = 0178
	// checking diagonals of quadrant2 = 7865
	// checking diagonals of quadrant3 = 8354
	static std::array<Direction, 8> node_parcours{
		TOP_LEFT,
		LEFT,
		LEFT,
		BOTTOM_RIGHT,
		CENTER,
		BOTTOM,
		TOP,
		CENTER
	};

	static std::array<Direction, 2> edges_parcours{BOTTOM_RIGHT, TOP_RIGHT};

	// VORONOI DIAGRAM CALCULATION
	// Each corner can do three things:
	// 1. Gain space from other cells (via type 1 and 2 points) (if an edge exists in that direction)
	// 2. Lose space to other cells (via type 3) (if an external edge cuts that corner)
	// 3. Keep the same space (there are no edges) (original corner point)
	// All the edge midpoints are kept to complete the diagram
	// Edges through the edge midpoints don't affect this diagram
	// This addition of points is done in anti-clowise order
	// TOPLEFT -> LEFT -> BOTTOMLEFT -> BOTTOM -> BOTTOMRIGHT -> RIGHT -> TOPRIGHT -> TOP
	
	#define TOTAL_VORONOI_CELLS 256
	using possible_cells_list = std::array<voronoiCell, TOTAL_VORONOI_CELLS>;


	// Checks if cell type can be in similarity graph
	bool checkCellType(voronoiCellType type)
	{
		return !(((type & 0b11) == 0b11)
			|| (type & (0b11 << 2)) == (0b11 << 2)
			|| (type & (0b11 << 4)) == (0b11 << 4)
			|| (type & (0b11 << 6)) == (0b11 << 6));
	}

	// for initial cells calculation
	template<bool simplify = true>
	struct CellsCalculation {

		possible_cells_list possibleCells;


		// trigo parcours
		voronoiCell addPointsVoronoi(voronoiCellType type, Direction dir, Point offset = Point(0,0)) {
		#define CUSTOM_PARCOURS_EDGE(i) (type & 1<<i)

			voronoiCell cell({});

			switch (dir) {
			case TOP_LEFT:
				if (CUSTOM_PARCOURS_EDGE(0))
				{
					cell.emplace_back(-0.25, -0.75); // 1
					cell.emplace_back(-0.75, -0.25); // 2
				}
				else if (CUSTOM_PARCOURS_EDGE(1))
					cell.emplace_back(-0.25, -0.25); // 3
				else cell.emplace_back(-0.5, -0.5); // 4
				break;
			case LEFT:
				cell.emplace_back(-0.5, 0.0); // Mid-point

				break;
			case BOTTOM_LEFT:
				if (CUSTOM_PARCOURS_EDGE(7))
				{
					cell.emplace_back(-0.75, +0.25); // 1
					cell.emplace_back(-0.25, +0.75); // 2
				}
				else if (CUSTOM_PARCOURS_EDGE(6))
					cell.emplace_back(-0.25, +0.25); // 3
				else cell.emplace_back(-0.5, +0.5); // 4
				break;
			case BOTTOM:
				cell.emplace_back(0.0, +0.5); // Mid-point

				break;
			case BOTTOM_RIGHT:
				if (CUSTOM_PARCOURS_EDGE(4))
				{
					cell.emplace_back(+0.25, +0.75); // 1
					cell.emplace_back(+0.75, +0.25); // 2
				}
				else if (CUSTOM_PARCOURS_EDGE(5))
					cell.emplace_back(+0.25, +0.25); // 3
				else cell.emplace_back(+0.5, +0.5); // 4
				break;
			case RIGHT:
				cell.emplace_back(+0.5, 0.0); // Mid-point

				break;
			case TOP_RIGHT:
				if (CUSTOM_PARCOURS_EDGE(3))
				{
					cell.emplace_back(+0.75, -0.25); // 1
					cell.emplace_back(+0.25, -0.75); // 2
				}
				else if (CUSTOM_PARCOURS_EDGE(2))
					cell.emplace_back(+0.25, -0.25); // 3
				else cell.emplace_back(+0.5, -0.5); // 4

				break;
			case TOP:
				cell.emplace_back(0.0, -0.5); // Mid-point
				break;

			}
			for (auto& p : cell) p += offset;
			return cell;
		}

		voronoiCellType transformTypeSimpleDir(voronoiCellType type, Direction dir) {
			assert(dir == LEFT || dir == RIGHT || dir == BOTTOM || dir == TOP);
			// envoyer quadrant a sur quadrant b, 0<= a,b <= 3 
			#define TRANSFORM_QUADRANT(a, b) ((((type & (0b11 << a)) >> a) & 0b11) << b)

			voronoiCellType newType = 0;
			switch (dir) {
			case LEFT:
				newType |= TRANSFORM_QUADRANT(4, 1) | TRANSFORM_QUADRANT(3, 2);
				break;
			case BOTTOM:
				newType |= TRANSFORM_QUADRANT(1, 2) | TRANSFORM_QUADRANT(4, 3);
				break;
			case RIGHT:
				newType |= TRANSFORM_QUADRANT(1, 4) | TRANSFORM_QUADRANT(2, 3);
				break;
			case TOP:
				newType |= TRANSFORM_QUADRANT(2, 1) | TRANSFORM_QUADRANT(3, 4);
				break;
			}
			return newType;
		}

		voronoiCellType transformType(voronoiCellType type, Direction dir) {
			voronoiCellType newType = 0;
			switch (dir) {
			case TOP_LEFT :
				newType = transformTypeSimpleDir(type, TOP) | transformTypeSimpleDir(type, LEFT);
				break;
			case BOTTOM_LEFT:
				newType = transformTypeSimpleDir(type, BOTTOM) | transformTypeSimpleDir(type, LEFT);
				break;
			case BOTTOM_RIGHT:
				newType = transformTypeSimpleDir(type, BOTTOM) | transformTypeSimpleDir(type, RIGHT);
				break;
			case TOP_RIGHT:
				newType = transformTypeSimpleDir(type, TOP) | transformTypeSimpleDir(type, RIGHT);
				break;
			default :
				newType = transformTypeSimpleDir(type, dir);
				break;
			}
			return newType;
		}

		// We can see on the algorithm that only diagonals are needed to entiorrely determine a simplified cell
		// We'll do exactly that.
		voronoiCell generateCellByType(voronoiCellType type) {
			voronoiCell cell;
			// First, populate center cell
			for (int i = 0; i < NUM_DIR; i++) {
				auto new_points = addPointsVoronoi(type, static_cast<Direction>(i));
				cell.insert(cell.end(), new_points.begin(), new_points.end());
			}
			if constexpr (simplify) {
				std::unordered_map<Point, int, PointHasher> valency;
				// Now add to valency, then same with 8 surrounding points
				for (auto& p : cell) valency[p] = 1;
				// Calculating voronoi points for each points in the 3*3 grid surrounding center point
				// We will do displacements from origin and modify type so that algorithm is lured into thinking
				// We are in another 3*3 grid.

				std::array<CircleDir, 8> new_edges_parcours ({
				std::make_pair<Direction, Direction>(BOTTOM, RIGHT),
				std::make_pair<Direction, Direction>(BOTTOM, TOP),
				std::make_pair<Direction, Direction>(RIGHT, TOP),
				std::make_pair<Direction, Direction>(RIGHT, LEFT),
				std::make_pair<Direction, Direction>(TOP, LEFT),
				std::make_pair<Direction, Direction>(TOP, BOTTOM),
				std::make_pair<Direction, Direction>(LEFT, BOTTOM),
				std::make_pair<Direction, Direction>(LEFT, RIGHT)
				//std::make_pair<Direction, Direction>(TOP_LEFT, TOP)
					});

				for (int i = 0; i < NUM_DIR; i++) {
					// We have to convert type for each displacement
					voronoiCellType new_type = transformType(type, static_cast<Direction>(i));

					// and now add points in each relevant direction
					auto& pair = new_edges_parcours[i];
					int j = pair.first;
					while(j != ((pair.second + 1) % NUM_DIR)) {
						Direction d = static_cast<Direction>(j);
						for (auto& p : addPointsVoronoi(new_type, d, Point(VecDir[i].x, VecDir[i].y))) {
							if (valency.find(p) != valency.end())
								valency[p]++;
						}
						j = (j + 1) % NUM_DIR;
					}
					// ok

				}
				// Now calculate new simplified cell
				int a = 0;
				int b = 1;
				int size = cell.size();
				voronoiCell simpCell; 
				simpCell.push_back(cell[a]);
				do {
					auto& pa = cell[a];
					auto& pb = cell[b];

					if (valency[pb] != 2) {
						simpCell.push_back(pb);
						a = b;
						b = (b + 1) % size;
					}
					else {
						b = (b + 1) % size;
					}
				} while (b != 0);
				simpCell.push_back(cell[b]);
				return simpCell;
			}
			else {
				return cell;
			}
		}

		void generateAllCellTypes() {
			std::generate(possibleCells.begin(), possibleCells.end(),
				[this, i = 0]() mutable{return generateCellByType(i++); });
			/*std::generate(possibleCells.begin(), possibleCells.end(),
				[this, i = 0]() mutable{return addPointsVoronoi(0, TOP_LEFT, Point(0, 0)); });*/
		}

	public:
		CellsCalculation() {
			generateAllCellTypes();
		}
		~CellsCalculation() {
			
		}
	};


	template<bool simplify = true>
	class VoronoiDiagram {

		const PixelGraph* m_graph;

		//Voronoi points around each pixels starting from top_left, trigonometric parcours
		std::vector<std::vector<voronoiCell>> m_voronoiCells;

		// Store each voronoiCell possible configurations 2^8
		// In reality 3^4 since no crossing but well
		static CellsCalculation<simplify> cellsCalculation;

		// Valency of each voronoi point for collapsing
		std::unordered_map<Point, int, PointHasher> m_valency;

		// Final diagrams
		diagram_hash_map m_diagram_hash_map;
		diagram m_diagram;

		voronoiCellType extractType(const IntPoint& p) const {
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

		void calculateVoronoiCells() {
			sf::Vector2u dim = m_graph->getImage().getSize();
			for (int x = 0; x < dim.x; x++) {
				for (int y = 0; y < dim.y; y++) {
					voronoiCellType type = extractType(IntPoint(x, y));
					if (!isOnBounds(IntPoint(x, y), dim) && !checkCellType(type)) {
						std::cout << "Error in similarity graph : " << x << ", " << y << std::endl;
						std::cout << "configuration : ";
						for (int i = 0; i < 8; i++) {
							std::cout << !!(type & 1 << i) << " : ";
						}
						std::cout << std::endl;
					}
					voronoiCell cell = cellsCalculation.possibleCells[type];
					for (auto& p : cell) p += Point(x + 0.5, y + 0.5);
					m_voronoiCells[x].push_back(cell);
				}
			}
		}

		void populateDiagrams() {
			m_diagram_hash_map.clear();
			m_diagram.clear();
			for (auto& cell_list : m_voronoiCells) {
				for (auto& cell : cell_list) {
					for (int i = 0; i < cell.size(); i++) {
						int j = (i + 1) % cell.size();
						m_diagram_hash_map[cell[i]].push_back(cell[j]);
						m_diagram_hash_map[cell[j]].push_back(cell[i]);
					}
				}
			}

			for (auto& elem : m_diagram_hash_map) {
				m_diagram.push_back(elem);
			}
		}

	public:
		VoronoiDiagram() : m_graph(nullptr)
		{
			// instantiate calculations
			(void)cellsCalculation;

		}
		void setGraph(const PixelGraph& graph) {
			m_graph = &graph;
			sf::Vector2u dim = graph.getImage().getSize();

			for (int i = 0; i < dim.x; i++)
			{
				std::vector<voronoiCell> v2(0);
				m_voronoiCells.push_back(v2);
			}
		}

		const possible_cells_list getPossibleVoronoiCells() const {
			return cellsCalculation.possibleCells;
		}

		//Creates Voronoi Diagram
		void createDiagram()
		{
			calculateVoronoiCells();
			populateDiagrams();
		}

		// adjacency list
		diagram getDiagram() {
			return m_diagram;
		}

		const PixelGraph* getGraph() { return m_graph; };
	};
	// Declare
	template<bool simplify>
	CellsCalculation<simplify> VoronoiDiagram<simplify>::cellsCalculation;
}