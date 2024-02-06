#include <iostream>
#include <vector>
#include <stack>
#include <format>
#include <unordered_set>
#include <queue>

/**
 * @struct State
 * @brief Represents the state of a missionary and cannibal problem.
 *
 * This struct stores the number of missionaries and cannibals on both sides of the river,
 * as well as the position of the boat.
 */
struct State {
    int leftMissionaries = 0;
    int leftCannibals = 0;
    int rightMissionaries = 0;
    int rightCannibals = 0;
    bool boatOnLeft = true;

    bool operator==(const State &other) const {
        return this->leftMissionaries == other.leftMissionaries &&
               this->leftCannibals == other.leftCannibals &&
               this->rightMissionaries == other.rightMissionaries &&
               this->rightCannibals == other.rightCannibals &&
               this->boatOnLeft == other.boatOnLeft;
    }
};

/**
 * @class StateHash
 * @brief Hash function object for class State
 *
 * This class provides a hash function object for the class State. It computes
 * the hash value based on the values of the member variables of the State object.
 */
struct StateHash {
    std::size_t operator()(State const &s) const noexcept {
        size_t h1 = std::hash<int>{}(s.leftMissionaries);
        size_t h2 = std::hash<int>{}(s.leftCannibals);
        size_t h3 = std::hash<int>{}(s.rightMissionaries);
        size_t h4 = std::hash<int>{}(s.rightCannibals);
        size_t h5 = s.boatOnLeft ? 1 : 0;
        return h1 ^ (h2 << 1) ^ (h3 << 2) ^ (h4 << 3) ^ (h5 << 4);
    }
};

bool isSafe(const State &state);

std::vector<State> getValidMoves(State currentState);

bool solveMissionariesCannibals(State initialState);

int main(int argc, char **argv) {
    State initialState;

    switch (argc) {
        case 3:
            initialState.leftCannibals = std::stoi(argv[2]);

            if (initialState.leftCannibals < 0) {
                std::cout << "Cannibal count cannot be negative." << std::endl;
                return 1;
            }

            // No break to allow for fall through
        case 2:
            initialState.leftMissionaries = std::stoi(argv[1]);

            if (initialState.leftMissionaries < 0) {
                std::cout << "Missionary count cannot be negative." << std::endl;
                return 1;
            }
            break;
        default:
            initialState.leftMissionaries = 3;
            initialState.leftCannibals = 3;
    }

    if (solveMissionariesCannibals(initialState)) {
        std::cout << "Solution found!" << std::endl;
    } else {
        std::cout << "No solution exists." << std::endl;
    }

    return 0;
}

/**
 * @fn bool isSafe(State state)
 * @brief Determines if the given state is safe.
 *
 * This function checks if the number of missionaries on each side of the river is not outnumbered by the cannibals.
 * If there are any missionaries and cannibals on the same side, it ensures that the missionaries are not outnumbered.
 *
 * @param state The state of the missionary and cannibal problem.
 * @return True if the state is safe, false otherwise.
 */
bool isSafe(const State &state) {
    // On both sides of river, if there are any missionaries, they don't get outnumbered and eaten by cannibals
    return (!(state.leftMissionaries != 0 && state.leftMissionaries < state.leftCannibals) &&
            !(state.rightMissionaries != 0 && state.rightMissionaries < state.rightCannibals));
}

/**
 * @brief Returns a vector of valid moves from the current state.
 *
 * This function takes a State object representing the current state and returns a vector
 * of valid moves that can be made from that state. A valid move is defined as moving 1 or 2
 * missionaries and/or cannibals from one side of the river to the other side, without violating
 * the constraints of the problem.
 *
 * @param currentState The current state of the problem.
 * @return A vector of valid moves from the current state.
 */
std::vector<State> getValidMoves(State currentState) {
    std::vector<State> moves;
    if (currentState.boatOnLeft) {
        for (int m = 0; m <= currentState.leftMissionaries; m++) {
            for (int c = 0; c <= currentState.leftCannibals; c++) {
                if ((m + c) >= 1 && (m + c) <= 2) {
                    State newState = currentState;
                    newState.leftMissionaries -= m;
                    newState.leftCannibals -= c;
                    newState.rightMissionaries += m;
                    newState.rightCannibals += c;
                    newState.boatOnLeft = false;
                    if (isSafe(newState)) {
                        moves.emplace_back(newState);
                    }
                }
            }
        }
    } else {
        for (int m = 0; m <= currentState.rightMissionaries; m++) {
            for (int c = 0; c <= currentState.rightCannibals; c++) {
                if ((m + c) >= 1 && (m + c) <= 2) {
                    State newState = currentState;
                    newState.leftMissionaries += m;
                    newState.leftCannibals += c;
                    newState.rightMissionaries -= m;
                    newState.rightCannibals -= c;
                    newState.boatOnLeft = true;
                    if (isSafe(newState)) {
                        moves.emplace_back(newState);
                    }
                }
            }
        }
    }

    return moves;
}

/**
 * @brief Solves the Missionaries and Cannibals problem.
 *
 * This function takes an initial state of the problem and solves it using a breadth-first search algorithm.
 * It explores the states of the problem and checks if the goal state has been reached.
 * If the goal state is reached, it returns true. Otherwise, it returns false.
 *
 * @param initialState The initial state of the problem.
 * @return True if the goal state is reached, false otherwise.
 */
bool solveMissionariesCannibals(State initialState) {
    std::unordered_set<State, StateHash> exploredStates;

    std::queue<State> statesToExplore;
    statesToExplore.push(initialState);

    std::string stateString = "Current State: \n"
                              "\tMissionaries on the left: {}\n"
                              "\tCannibals on the left: {}\n"
                              "\tMissionaries on the right: {}\n"
                              "\tCannibals on the right: {}\n"
                              "\tBoat on the left: {}\n";

    while (!statesToExplore.empty()) {
        State currentState = statesToExplore.front();
        statesToExplore.pop();

        if (exploredStates.find(currentState) != exploredStates.end()) { continue; }

        exploredStates.insert(currentState);

        std::cout << std::vformat(stateString,
                                  std::make_format_args(currentState.leftMissionaries, currentState.leftCannibals,
                                                        currentState.rightMissionaries, currentState.rightCannibals,
                                                        currentState.boatOnLeft));

        if (currentState.leftMissionaries == 0 && currentState.leftCannibals == 0 && !currentState.boatOnLeft) {
            return true;
        }

        std::vector<State> validMoves = getValidMoves(currentState);
        for (State move: validMoves) {
            if (exploredStates.find(move) == exploredStates.end())
                statesToExplore.push(move);
        }
    }

    return false;
}