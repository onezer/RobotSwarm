
//A simple robot algorithm for testing the simulator
class Filling : public iBehaviour {
	Map::direction nextMove;
	Map::direction prevMove;
	bool move;

	class FillingFactory : public iBehaviourFactory {
	public:
		std::unique_ptr<iBehaviour> CreateBehaviour(unsigned int id) {
			return std::make_unique<Filling>(id);
		}
	};

	Map::direction North = Map::direction::North;
	Map::direction South = Map::direction::South;
	Map::direction East = Map::direction::East;
	Map::direction West = Map::direction::West;
	Map::nodeType Free = Map::nodeType::Free;

public:
	Filling(unsigned int id) :move{ true }, nextMove{West}
	{
		this->id = id;
	}

	std::vector<Map::direction> Look() {
		std::vector<Map::direction> ret = std::vector<Map::direction>();
		ret.push_back(North);
		ret.push_back(East);
		ret.push_back(West);
		ret.push_back(South);
		return ret;
	}

	void Compute(std::unordered_map<Map::direction, Map::NodeObj> lookData) {
		prevMove = nextMove;

		if (
			lookData[North].type != Free &&
			lookData[South].type == Free &&
			lookData[West].type != Free &&
			lookData[East].type == Free
			) {
			move = false;
			return;
		}

		if (
			lookData[North].type != Free &&
			lookData[South].type == Free &&
			lookData[West].type == Free &&
			lookData[East].type != Free
			) {
			move = false;
			return;
		}

		if (
			lookData[North].type == Free &&
			lookData[South].type != Free &&
			lookData[West].type != Free &&
			lookData[East].type == Free
			) {
			move = false;
			return;
		}

		if (
			lookData[North].type == Free &&
			lookData[South].type != Free &&
			lookData[West].type == Free &&
			lookData[East].type != Free
			) {
			move = false;
			return;
		}

		if (
			lookData[North].type != Free &&
			lookData[South].type != Free &&
			lookData[West].type != Free &&
			lookData[East].type != Free
			) {
			move = false;
			return;
		}
		if (
			lookData[North].type == Free &&
			lookData[South].type != Free &&
			lookData[West].type != Free &&
			lookData[East].type != Free
			) {
			move = false;
			return;
		}
		if (
			lookData[North].type != Free &&
			lookData[South].type == Free &&
			lookData[West].type != Free &&
			lookData[East].type != Free
			) {
			move = false;
			return;
		}
		if (
			lookData[North].type != Free &&
			lookData[South].type != Free &&
			lookData[West].type == Free &&
			lookData[East].type != Free
			) {
			move = false;
			return;
		}
		if (
			lookData[North].type != Free &&
			lookData[South].type != Free &&
			lookData[West].type != Free &&
			lookData[East].type == Free
			) {
			move = false;
			return;
		}

		if (
			lookData[North].type == Free &&
			lookData[South].type == Free &&
			lookData[West].type == Free &&
			lookData[East].type == Free
			) {
			nextMove = West;
		}

		if (
			lookData[North].type != Free &&
			lookData[South].type == Free &&
			lookData[West].type == Free &&
			lookData[East].type == Free
			) {
			nextMove = East;
		}

		if (
			lookData[North].type == Free &&
			lookData[South].type != Free &&
			lookData[West].type == Free &&
			lookData[East].type == Free
			) {
			nextMove = West;
		}

		if (
			lookData[North].type == Free &&
			lookData[South].type == Free &&
			lookData[West].type != Free &&
			lookData[East].type == Free
			) {
			nextMove = South;
		}

		if (
			lookData[North].type == Free &&
			lookData[South].type == Free &&
			lookData[West].type == Free &&
			lookData[East].type != Free
			) {
			nextMove = North;
		}

		if (
			lookData[North].type != Free &&
			lookData[South].type != Free &&
			lookData[West].type == Free &&
			lookData[East].type == Free
			) {
			nextMove = prevMove;
		}

		if (
			lookData[North].type == Free &&
			lookData[South].type == Free &&
			lookData[West].type != Free &&
			lookData[East].type != Free
			) {
			nextMove = prevMove;
		}

		if ((nextMove == South && prevMove == North) || (nextMove == North && prevMove == South)) {
			if (lookData[West].type == Free) {
				nextMove = West;
				return;
			}
			if (lookData[East].type == Free) {
				nextMove = East;
				return;
			}
		}

		if ((nextMove == West && prevMove == East) || (nextMove == East && prevMove == West)) {
			if (lookData[North].type == Free) {
				nextMove = North;
				return;
			}
			if (lookData[South].type == Free) {
				nextMove = South;
				return;
			}
		}

	}


	std::pair<bool, Map::direction> Move() {
		return std::pair<bool, Map::direction>(move, nextMove);
	}

	static std::unique_ptr<iBehaviourFactory> Factory() {
		return std::make_unique<FillingFactory>();
	}
};