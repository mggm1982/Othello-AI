#include <iostream>
#include <vector>
#include <string>
#include <climits>
#include <cstdlib>
#include <ctime>
#include <cctype>
#include <iomanip>
#include <algorithm>

using namespace std;

// --- Enums and Structs for Game State ---
enum class CellState {
    EMPTY,
    BLACK, // Player 1
    WHITE  // Player 2
};

struct Move {
    int row = -1;
    int col = -1;
};

// --- Player Class ---
class Player {
private:
    string name;
    CellState symbol;
    bool isHuman;
    int skillLevel;

public:
    Player(string n, CellState s, bool human = true, int level = 0)
        : name(n), symbol(s), isHuman(human), skillLevel(level) {}

    string getName() const { return name; }
    CellState getSymbol() const { return symbol; }
    bool getIsHuman() const { return isHuman; }
    int getSkillLevel() const { return skillLevel; }
};

// --- Board Class ---
class Board {
private:
    const int SIZE = 8;
    vector<vector<CellState>> grid;

    const int positionalWeights[8][8] = {
        {120, -20, 20,  5,  5, 20, -20, 120},
        {-20, -40, -5, -5, -5, -5, -40, -20},
        { 20,  -5, 15,  3,  3, 15,  -5,  20},
        {  5,  -5,  3,  3,  3,  3,  -5,   5},
        {  5,  -5,  3,  3,  3,  3,  -5,   5},
        { 20,  -5, 15,  3,  3, 15,  -5,  20},
        {-20, -40, -5, -5, -5, -5, -40, -20},
        {120, -20, 20,  5,  5, 20, -20, 120}
    };

    int checkDirection(CellState player, int row, int col, int dRow, int dCol) const {
        CellState opponent = (player == CellState::BLACK) ? CellState::WHITE : CellState::BLACK;
        int flips = 0;
        int r = row + dRow;
        int c = col + dCol;
        while (r >= 0 && r < SIZE && c >= 0 && c < SIZE && grid[r][c] == opponent) {
            flips++;
            r += dRow;
            c += dCol;
        }
        if (flips > 0 && r >= 0 && r < SIZE && c >= 0 && c < SIZE && grid[r][c] == player) {
            return flips;
        }
        return 0;
    }

    void flipDirection(CellState player, int row, int col, int dRow, int dCol) {
        CellState opponent = (player == CellState::BLACK) ? CellState::WHITE : CellState::BLACK;
        int r = row + dRow;
        int c = col + dCol;
        while (r >= 0 && r < SIZE && c >= 0 && c < SIZE && grid[r][c] == opponent) {
            grid[r][c] = player;
            r += dRow;
            c += dCol;
        }
    }

    vector<Move> makeMoveAI(CellState player, int row, int col) {
        vector<Move> flipped;
        grid[row][col] = player;

        CellState opponent = (player == CellState::BLACK) ?
        CellState::WHITE : CellState::BLACK;
        for (int dRow = -1; dRow <= 1; ++dRow) {
            for (int dCol = -1; dCol <= 1; ++dCol) {
                if (dRow == 0 && dCol == 0) continue;
                if (checkDirection(player, row, col, dRow, dCol) > 0) {
                    int r = row + dRow;
                    int c = col + dCol;
                    while (grid[r][c] == opponent) {
                        grid[r][c] = player;
                        flipped.push_back({r, c});
                        r += dRow;
                        c += dCol;
                    }
                }
            }
        }
        return flipped;
    }

    void unmakeMoveAI(CellState player, const vector<Move>& flipped, int row, int col) {
        grid[row][col] = CellState::EMPTY;
        CellState opponent = (player == CellState::BLACK) ? CellState::WHITE : CellState::BLACK;
        for (const auto& f : flipped) {
            grid[f.row][f.col] = opponent;
        }
    }

public:
    Board() : SIZE(8), grid(8, vector<CellState>(8, CellState::EMPTY)) {
        int center = SIZE / 2;
        grid[center - 1][center - 1] = CellState::WHITE;
        grid[center][center] = CellState::WHITE;
        grid[center - 1][center] = CellState::BLACK;
        grid[center][center - 1] = CellState::BLACK;
    }

    void printBoard() const {
        cout << "\n    ";
        for (char c = 'A'; c < 'A' + SIZE; ++c) cout << c << " ";
        cout << "\n  +";
        for (int i = 0; i < SIZE; ++i) cout << "--";
        cout << "-+\n";

        for (int i = 0; i < SIZE; ++i) {
            cout << setw(2) << i + 1 << "| ";
            for (int j = 0; j < SIZE; ++j) {
                char symbol = (grid[i][j] == CellState::BLACK) ? 'B' :
                (grid[i][j] == CellState::WHITE) ? 'W' : '.';
                cout << symbol << " ";
            }
            cout << "|\n";
        }

        cout << "  +";
        for (int i = 0; i < SIZE; ++i) cout << "--";
        cout << "-+\n\n";
    }

    bool isValidMove(CellState player, int row, int col) const {
        if (row < 0 || row >= SIZE || col < 0 || col >= SIZE || grid[row][col] != CellState::EMPTY) {
            return false;
        }
        for (int dRow = -1; dRow <= 1; ++dRow) {
            for (int dCol = -1; dCol <= 1; ++dCol) {
                if (dRow == 0 && dCol == 0) continue;
                if (checkDirection(player, row, col, dRow, dCol) > 0) return true;
            }
        }
        return false;
    }

    bool makeMove(CellState player, int row, int col) {
        if (!isValidMove(player, row, col)) return false;

        grid[row][col] = player;
        for (int dRow = -1; dRow <= 1; ++dRow) {
            for (int dCol = -1; dCol <= 1; ++dCol) {
                if (dRow == 0 && dCol == 0) continue;
                if (checkDirection(player, row, col, dRow, dCol) > 0) {
                    flipDirection(player, row, col, dRow, dCol);
                }
            }
        }
        return true;
    }

    pair<int, int> countDiscs() const {
        int blackCount = 0, whiteCount = 0;
        for (int i = 0; i < SIZE; ++i) {
            for (int j = 0; j < SIZE; ++j) {
                if (grid[i][j] == CellState::BLACK)
                    blackCount++;
                else if (grid[i][j] == CellState::WHITE)
                    whiteCount++;
            }
        }
        return {blackCount, whiteCount};
    }

    vector<Move> getValidMoves(CellState player) const {
        vector<Move> moves;
        for (int i = 0; i < SIZE; ++i) {
            for (int j = 0; j < SIZE; ++j) {
                if (isValidMove(player, i, j)) {
                    moves.push_back({i, j});
                }
            }
        }
        return moves;
    }

    int countFlippableDiscs(CellState player, int row, int col) const {
        if (!isValidMove(player, row, col)) return 0;
        int totalFlips = 0;
        for (int dRow = -1; dRow <= 1; ++dRow) {
            for (int dCol = -1; dCol <= 1; ++dCol) {
                if (dRow == 0 && dCol == 0) continue;
                totalFlips += checkDirection(player, row, col, dRow, dCol);
            }
        }
        return totalFlips;
    }

    int maxValue(int depth, int alpha, int beta, CellState player);
    int minValue(int depth, int alpha, int beta, CellState player);

    int evaluateBoard(CellState player) {
        int myScore = 0;
        int opponentScore = 0;
        CellState opponent = (player == CellState::BLACK) ? CellState::WHITE : CellState::BLACK;

        pair<int, int> discCounts = countDiscs();
        int totalDiscs = discCounts.first + discCounts.second;
        bool isEndGame = totalDiscs > (SIZE * SIZE - 12);

        for (int i = 0; i < SIZE; ++i) {
            for (int j = 0; j < SIZE; ++j) {
                if (grid[i][j] == player) myScore += positionalWeights[i][j];
                else if (grid[i][j] == opponent) opponentScore += positionalWeights[i][j];
            }
        }

        int myMoves = getValidMoves(player).size();
        int opponentMoves = getValidMoves(opponent).size();
        int mobilityScore = 20 * (myMoves - opponentMoves);

        int parityScore = (player == CellState::BLACK) ? (discCounts.first - discCounts.second) : (discCounts.second - discCounts.first);

        if (isEndGame) {
            return (myScore - opponentScore) + (50 * parityScore);
        } else {
            return (myScore - opponentScore) + mobilityScore;
        }
    }

    Move findBestMove(CellState player, int depth) {
        vector<Move> validMoves = getValidMoves(player);
        if (validMoves.empty()) return {-1, -1};

        int bestScore = INT_MIN;
        Move bestMove = validMoves[0];
        int alpha = INT_MIN;
        int beta = INT_MAX;

        for (const auto& move : validMoves) {
            CellState nextPlayer = (player == CellState::BLACK) ?
            CellState::WHITE : CellState::BLACK;
            vector<Move> flipped = makeMoveAI(player, move.row, move.col);
            int score = minValue(depth - 1, alpha, beta, nextPlayer);
            unmakeMoveAI(player, flipped, move.row, move.col);

            if (score > bestScore) {
                bestScore = score;
                bestMove = move;
            }
            alpha = max(alpha, bestScore);
        }
        return bestMove;
    }
};

int Board::maxValue(int depth, int alpha, int beta, CellState player) {
    if (depth == 0) return evaluateBoard(player);

    vector<Move> validMoves = getValidMoves(player);
    if (validMoves.empty()) {
        CellState opponent = (player == CellState::BLACK) ?
         CellState::WHITE : CellState::BLACK;
        if (getValidMoves(opponent).empty()) return evaluateBoard(player);
        return minValue(depth - 1, alpha, beta, opponent);
    }

    int value = INT_MIN;
    for (const auto& move : validMoves) {
        CellState nextPlayer = (player == CellState::BLACK) ?
         CellState::WHITE : CellState::BLACK;
        vector<Move> flipped = makeMoveAI(player, move.row, move.col);
        value = max(value, minValue(depth - 1, alpha, beta, nextPlayer));
        unmakeMoveAI(player, flipped, move.row, move.col);

        if (value >= beta) return value;
        alpha = max(alpha, value);
    }
    return value;
}

int Board::minValue(int depth, int alpha, int beta, CellState player) {
    if (depth == 0) return evaluateBoard(player);

    vector<Move> validMoves = getValidMoves(player);
    if (validMoves.empty()) {
        CellState opponent = (player == CellState::BLACK) ?
         CellState::WHITE : CellState::BLACK;
        if (getValidMoves(opponent).empty()) return evaluateBoard(player);
        return maxValue(depth - 1, alpha, beta, opponent);
    }

    int value = INT_MAX;
    for (const auto& move : validMoves) {
        CellState nextPlayer = (player == CellState::BLACK) ?
         CellState::WHITE : CellState::BLACK;
        vector<Move> flipped = makeMoveAI(player, move.row, move.col);
        value = min(value, maxValue(depth - 1, alpha, beta, nextPlayer));
        unmakeMoveAI(player, flipped, move.row, move.col);

        if (value <= alpha) return value;
        beta = min(beta, value);
    }
    return value;
}

// --- Game Class ---
class Game {
private:
    Board board;
    Player player1, player2;
    Player* currentPlayer;

    Move getPlayerMove() {
        string input;
        cout << "Enter your move (e.g., A1) or 'H' for hint: ";
        getline(cin, input);

        input.erase(0, input.find_first_not_of(" \t\n\r"));
        input.erase(input.find_last_not_of(" \t\n\r") + 1);

        if (input.length() == 1 && toupper(input[0]) == 'H') return {-2, -2};
        if (input.length() < 2) return {-1, -1};

        int col = toupper(input[0]) - 'A';
        int row;
        try { row = stoi(input.substr(1)) - 1; }
        catch (const exception& e) { return {-1, -1}; }
        return {row, col};
    }

public:
    Game(string n1, string n2, bool isP2Human, int level)
        : board(), player1(n1, CellState::BLACK),
         player2(n2, CellState::WHITE, !isP2Human, level) {
        currentPlayer = &player1;
    }

    void start() {
        cout << "Welcome to Othello Reimagined!" << endl;

        char choice;
        bool isHumanVsHuman = true;
        int aiLevel = 0;
        string input;

        while (true) {
            cout << "Choose mode: (H)uman vs Human or (C)omputer: ";
            getline(cin, input);
            if (!input.empty()) {
                char firstChar = toupper(input[0]);
                if (firstChar == 'H' || firstChar == 'C') {
                    choice = firstChar;
                    break;
                }
            }
            cout << "Invalid. Enter 'H' or 'C'." << endl;
        }

        if (choice == 'C') {
            isHumanVsHuman = false;
            while (true) {
                cout << "Choose AI level:\n"
                     << "1. Easy (Random)\n"
                     << "2. Medium (Greedy)\n"
                     << "3. Hard (Strategic AI)\n"
                     << "Enter level: ";
                getline(cin, input);
                try {
                    aiLevel = stoi(input);
                    if (aiLevel >= 1 && aiLevel <= 3) break;
                } catch (const exception& e) {}
                cout << "Invalid. Enter 1-3." << endl;
            }
        }

        player1 = Player("Human", CellState::BLACK);
        if (isHumanVsHuman) player2 = Player("Human 2", CellState::WHITE);
        else player2 = Player("Computer", CellState::WHITE, false, aiLevel);
        currentPlayer = &player1;

        while (true) {
            board.printBoard();
            pair<int, int> score = board.countDiscs();
            cout << "Score: Black (B) - " << score.first << " | White (W) - " << score.second << endl;

            vector<Move> currentValidMoves = board.getValidMoves(currentPlayer->getSymbol());
            if (currentValidMoves.empty()) {
                cout << currentPlayer->getName() << " has no valid moves. Passing turn." << endl;
                switchTurn();
                if (board.getValidMoves(currentPlayer->getSymbol()).empty()) {
                    cout << "\nNo moves for either player. Game Over!" << endl;
                    break;
                }
                continue;
            }

            cout << "\n" << currentPlayer->getName() << "'s turn (" << ((currentPlayer->getSymbol()== CellState::BLACK)?
                "B" : "W") << "):" << endl;

            if (currentPlayer->getIsHuman()) {
                Move move = getPlayerMove();
                if (move.row == -2) {
                    cout << "Calculating the best possible move..." << endl;
                    Move bestMove = board.findBestMove(currentPlayer->getSymbol(), 8);
                    cout << "AI Suggestion: " << (char)('A' + bestMove.col) << bestMove.row + 1 << endl;
                    continue;
                }
                if (board.makeMove(currentPlayer->getSymbol(), move.row, move.col)) {
                    switchTurn();
                } else {
                    cout << "Invalid move. Try again." << endl;
                }
            } else {
                cout << "Computer is thinking..." << endl;
                Move computerMove;
                int skill = currentPlayer->getSkillLevel();
                if (skill == 1) {
                    computerMove = currentValidMoves[rand() % currentValidMoves.size()];
                } else if (skill == 2) {
                    int bestFlips = -1;
                    for (const auto& m : currentValidMoves) {
                        int flips = board.countFlippableDiscs(currentPlayer->getSymbol(), m.row, m.col);
                        if (flips > bestFlips) { bestFlips = flips; computerMove = m; }
                    }
                } else {
                    int depth = 8;
                    computerMove = board.findBestMove(currentPlayer->getSymbol(), depth);
                }

                board.makeMove(currentPlayer->getSymbol(), computerMove.row, computerMove.col);
                cout << "Computer plays: " << (char)('A' + computerMove.col) << computerMove.row + 1 << endl;
                switchTurn();
            }
        }

        board.printBoard();
        pair<int, int> finalScore = board.countDiscs();
        cout << "\n--- FINAL SCORE ---\n"
             << player1.getName() << " (Black): " << finalScore.first << "\n"
             << player2.getName() << " (White): " << finalScore.second << endl;

        if (finalScore.first > finalScore.second) cout << player1.getName() << " wins!" << endl;
        else if (finalScore.second > finalScore.first) cout << player2.getName() << " wins!" << endl;
        else cout << "It's a draw!" << endl;
    }

    void switchTurn() {
        currentPlayer = (currentPlayer == &player1) ? &player2 : &player1;
    }
};

// --- Main Function ---
int main() {
    srand(time(0));

    Game game("Human", "Computer", true, 0);
    game.start();
    return 0;
}