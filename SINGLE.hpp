
class SINGLE : public iBehaviour {

	Map::direction nextMove;
	Map::direction prevMove;
	bool move;

	class SINGLEFactory : public iBehaviourFactory {
	public:
		std::unique_ptr<iBehaviour> CreateBehaviour(unsigned int id) {
			return std::make_unique<SINGLE>(id);
		}
	};

	Map::direction North = Map::direction::North;
	Map::direction South = Map::direction::South;
	Map::direction East = Map::direction::East;
	Map::direction West = Map::direction::West;
	Map::direction NorthWest = Map::direction::NorthWest;
	Map::direction NorthEast = Map::direction::NorthEast;
	Map::direction SouthWest = Map::direction::SouthWest;
	Map::direction SouthEast = Map::direction::SouthEast;
	Map::nodeType Free = Map::nodeType::Free;
	Map::nodeType Obstacle = Map::nodeType::Obstacle;

	bool isEmpty(std::unordered_map<Map::direction, Map::NodeObj> lookData, Map::direction dir) {
		return lookData[dir].type == Free;
	}

	bool isObstacle(std::unordered_map<Map::direction, Map::NodeObj> lookData, Map::direction dir) {
		return lookData[dir].type == Obstacle;
	}

	Map::direction Opposite(Map::direction dir) {
		switch (dir) {
		case Map::direction::North: return South;
		case Map::direction::South: return North;
		case Map::direction::West: return East;
		case Map::direction::East: return West;
		}
	}


public:

	SINGLE(unsigned int id) : move{ true }, nextMove{East}
	{
		this->id = id;
	}

	std::vector<Map::direction> Look() {
		std::vector<Map::direction> ret = std::vector<Map::direction>();
		ret.push_back(North);
		ret.push_back(East);
		ret.push_back(West);
		ret.push_back(South);
		ret.push_back(NorthWest);
		ret.push_back(NorthEast);
		ret.push_back(SouthWest);
		ret.push_back(SouthEast);
		return ret;
	}

	void Compute(std::unordered_map<Map::direction, Map::NodeObj> lookData) {
		
		lookData.insert_or_assign(Opposite(prevMove), Map::NodeObj(Map::nodeType::Robot));

		if (isEmpty(lookData, East)) {
			nextMove = East;
			move = true;
		}
		else if (isEmpty(lookData, West)) {
			nextMove = West;
			move = true;
		}
		else if (isEmpty(lookData, South) && (isObstacle(lookData, SouthWest) || isObstacle(lookData, West))) {
			nextMove = South;
			move = true;
		}
		else if (isEmpty(lookData, North) && (isObstacle(lookData, NorthWest) || isObstacle(lookData, West))) {
			nextMove = North;
			move = true;
		}
		else {
			move = false;
		}
		if (move) {
			prevMove = nextMove;
		}
	}

	std::pair<bool, Map::direction> Move() {
		return std::pair<bool, Map::direction>(move, nextMove);
	}

	static std::unique_ptr<iBehaviourFactory> Factory() {
		return std::make_unique<SINGLEFactory>();
	}
};