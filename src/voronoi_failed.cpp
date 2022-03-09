

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