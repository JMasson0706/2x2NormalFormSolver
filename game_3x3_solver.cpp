#include "game_solver.h"
#include <iostream>
#include <vector>
#include <string>
#include <iomanip>
#include <sstream>
#include <algorithm>
#include <map>
#include <functional>

using namespace std;

// Structure to represent a 3x3 normal form game
struct Game3x3 {
    vector<vector<vector<double>>> payoffs; // [row][col][player]
    
    Game3x3() {
        // Initialize 3x3x2 structure: [row][col][player]
        payoffs = vector<vector<vector<double>>>(3, vector<vector<double>>(3, vector<double>(2, 0.0)));
    }
    
    // Set payoff for a specific outcome
    void setPayoff(int row, int col, int player, double payoff) {
        if (row >= 0 && row < 3 && col >= 0 && col < 3 && player >= 0 && player < 2) {
            payoffs[row][col][player] = payoff;
        }
    }
    
    // Get payoff for a specific outcome
    double getPayoff(int row, int col, int player) const {
        if (row >= 0 && row < 3 && col >= 0 && col < 3 && player >= 0 && player < 2) {
            return payoffs[row][col][player];
        }
        return 0.0;
    }
    
    // Print the 3x3 game matrix
    void printGame() const {
        cout << "3x3 Game Matrix:" << endl;
        cout << "                    Player 2" << endl;
        cout << "            Strategy 1    Strategy 2    Strategy 3" << endl;
        
        for (int i = 0; i < 3; i++) {
            cout << "Player 1 Strategy " << (i+1) << ": ";
            for (int j = 0; j < 3; j++) {
                cout << "(" << fixed << setprecision(1) 
                     << getPayoff(i, j, 0) << ", " << getPayoff(i, j, 1) << ")";
                if (j < 2) cout << "    ";
            }
            cout << endl;
        }
        cout << endl;
    }
};

// Structure to represent a 2x2 submatrix with its source positions
struct Submatrix2x2 {
    Game2x2 game;
    vector<pair<int, int>> positions; // Original positions in 3x3 matrix: [(row1,col1), (row2,col2), (row3,col3), (row4,col4)]
    
    Submatrix2x2() {
        positions.resize(4);
    }
    
    void printSubmatrixInfo() const {
        cout << "2x2 Submatrix from positions: ";
        cout << "(" << positions[0].first << "," << positions[0].second << "), ";
        cout << "(" << positions[1].first << "," << positions[1].second << "), ";
        cout << "(" << positions[2].first << "," << positions[2].second << "), ";
        cout << "(" << positions[3].first << "," << positions[3].second << ")" << endl;
        
        cout << "            Player 2" << endl;
        cout << "            Strategy 1    Strategy 2" << endl;
        cout << "Player 1 Strategy 1: (" << fixed << setprecision(1) 
             << game.getPayoff(0, 0, 0) << ", " << game.getPayoff(0, 0, 1) 
             << ")    (" << game.getPayoff(0, 1, 0) << ", " << game.getPayoff(0, 1, 1) << ")" << endl;
        cout << "Player 1 Strategy 2: (" << game.getPayoff(1, 0, 0) << ", " << game.getPayoff(1, 0, 1) 
             << ")    (" << game.getPayoff(1, 1, 0) << ", " << game.getPayoff(1, 1, 1) << ")" << endl;
        cout << endl;
    }
};

class Game3x3Solver {
private:
    Game3x3 game3x3;
    
    // Generate all combinations of 4 positions from 9 positions
    vector<vector<int>> generateCombinations(int n, int k) {
        vector<vector<int>> combinations;
        vector<int> combination(k);
        
        function<void(int, int)> generateCombinationsHelper = [&](int start, int depth) {
            if (depth == k) {
                combinations.push_back(combination);
                return;
            }
            
            for (int i = start; i <= n - k + depth; i++) {
                combination[depth] = i;
                generateCombinationsHelper(i + 1, depth + 1);
            }
        };
        
        generateCombinationsHelper(0, 0);
        return combinations;
    }
    
public:
    Game3x3Solver(const Game3x3& game) : game3x3(game) {}
    
    // Extract all possible 2x2 submatrices from the 3x3 game
    vector<Submatrix2x2> extractAll2x2Submatrices() {
        vector<Submatrix2x2> submatrices;
        
        // Generate all combinations of 4 positions from 9 positions (0 to 8)
        vector<vector<int>> combinations = generateCombinations(9, 4);
        
        for (const auto& combo : combinations) {
            Submatrix2x2 submatrix;
            
            // Convert linear indices to (row, col) pairs
            for (int i = 0; i < 4; i++) {
                int row = combo[i] / 3;
                int col = combo[i] % 3;
                submatrix.positions[i] = make_pair(row, col);
            }
            
            // Sort positions to create a valid 2x2 matrix
            // We need to ensure we have 2 rows and 2 columns
            vector<int> rows, cols;
            for (const auto& pos : submatrix.positions) {
                if (find(rows.begin(), rows.end(), pos.first) == rows.end()) {
                    rows.push_back(pos.first);
                }
                if (find(cols.begin(), cols.end(), pos.second) == cols.end()) {
                    cols.push_back(pos.second);
                }
            }
            
            // Skip if we don't have exactly 2 rows and 2 columns
            if (rows.size() != 2 || cols.size() != 2) {
                continue;
            }
            
            // Sort rows and columns
            sort(rows.begin(), rows.end());
            sort(cols.begin(), cols.end());
            
            // Create the 2x2 game matrix in proper order
            submatrix.positions[0] = make_pair(rows[0], cols[0]); // Top-left
            submatrix.positions[1] = make_pair(rows[0], cols[1]); // Top-right
            submatrix.positions[2] = make_pair(rows[1], cols[0]); // Bottom-left
            submatrix.positions[3] = make_pair(rows[1], cols[1]); // Bottom-right
            
            // Fill the 2x2 game matrix
            for (int i = 0; i < 2; i++) {
                for (int j = 0; j < 2; j++) {
                    int pos_idx = i * 2 + j;
                    int orig_row = submatrix.positions[pos_idx].first;
                    int orig_col = submatrix.positions[pos_idx].second;
                    
                    submatrix.game.setPayoff(i, j, 0, game3x3.getPayoff(orig_row, orig_col, 0));
                    submatrix.game.setPayoff(i, j, 1, game3x3.getPayoff(orig_row, orig_col, 1));
                }
            }
            
            submatrices.push_back(submatrix);
        }
        
        return submatrices;
    }
    
    // Structure to hold submatrix analysis results
    struct SubmatrixAnalysis {
        Submatrix2x2 submatrix;
        vector<Equilibrium> nash_equilibria;
        vector<Equilibrium> non_myopic_equilibria;
        int submatrix_id;
    };
    
    // Analyze all 2x2 submatrices and return results
    vector<SubmatrixAnalysis> analyzeAll2x2SubmatricesDetailed() {
        vector<Submatrix2x2> submatrices = extractAll2x2Submatrices();
        vector<SubmatrixAnalysis> analyses;
        
        cout << "Found " << submatrices.size() << " valid 2x2 submatrices from the 3x3 game." << endl << endl;
        
        int count = 1;
        for (const auto& submatrix : submatrices) {
            SubmatrixAnalysis analysis;
            analysis.submatrix = submatrix;
            analysis.submatrix_id = count;
            
            cout << "=== Submatrix " << count << " ===" << endl;
            submatrix.printSubmatrixInfo();
            
            // Solve the 2x2 subgame
            NormalFormSolver solver(submatrix.game);
            
            // Find Nash equilibria
            analysis.nash_equilibria = solver.findAllNashEquilibria();
            cout << "Nash Equilibria for this submatrix:" << endl;
            if (analysis.nash_equilibria.empty()) {
                cout << "No Nash equilibria found." << endl;
            } else {
                for (const auto& eq : analysis.nash_equilibria) {
                    cout << "- " << eq.description << endl;
                }
            }
            
            // Find non-myopic equilibria
            analysis.non_myopic_equilibria = solver.findNonMyopicEquilibria();
            cout << "Non-Myopic Equilibria for this submatrix:" << endl;
            if (analysis.non_myopic_equilibria.empty()) {
                cout << "No non-myopic equilibria found." << endl;
            } else {
                for (const auto& eq : analysis.non_myopic_equilibria) {
                    cout << "- " << eq.description << endl;
                }
            }
            
            analyses.push_back(analysis);
            cout << string(50, '-') << endl << endl;
            count++;
        }
        
        return analyses;
    }
    
    // Analyze all 2x2 submatrices using the existing 2x2 solver
    void analyzeAll2x2Submatrices() {
        analyzeAll2x2SubmatricesDetailed();
    }
    
    // Calculate 3x3 NME based on subgame analysis
    void calculate3x3NMEFromSubgames() {
        cout << "\n" << string(60, '=') << endl;
        cout << "CALCULATING 3x3 NON-MYOPIC EQUILIBRIUM FROM SUBGAME ANALYSIS" << endl;
        cout << string(60, '=') << endl << endl;
        
        vector<SubmatrixAnalysis> analyses = analyzeAll2x2SubmatricesDetailed();
        
        // Strategy: Look for consistency across subgames
        // Count how often each pure strategy appears in NME across all subgames
        map<pair<int,int>, int> strategy_votes; // (row, col) -> count
        map<pair<int,int>, vector<int>> strategy_supporters; // (row, col) -> list of submatrix IDs
        
        cout << "Analyzing subgame NME patterns..." << endl << endl;
        
        for (const auto& analysis : analyses) {
            for (const auto& nme : analysis.non_myopic_equilibria) {
                // Convert probabilities to pure strategies (with tolerance)
                int p1_strategy = (nme.profile.p1_strategy1_prob > 0.5) ? 0 : 1;
                int p2_strategy = (nme.profile.p2_strategy1_prob > 0.5) ? 0 : 1;
                
                // Map back to 3x3 coordinates based on submatrix positions
                vector<int> rows, cols;
                for (const auto& pos : analysis.submatrix.positions) {
                    if (find(rows.begin(), rows.end(), pos.first) == rows.end()) {
                        rows.push_back(pos.first);
                    }
                    if (find(cols.begin(), cols.end(), pos.second) == cols.end()) {
                        cols.push_back(pos.second);
                    }
                }
                sort(rows.begin(), rows.end());
                sort(cols.begin(), cols.end());
                
                // Get the actual 3x3 coordinates
                int actual_row = rows[p1_strategy];
                int actual_col = cols[p2_strategy];
                
                pair<int,int> strategy_pair = make_pair(actual_row, actual_col);
                strategy_votes[strategy_pair]++;
                strategy_supporters[strategy_pair].push_back(analysis.submatrix_id);
                
                cout << "Submatrix " << analysis.submatrix_id << " votes for 3x3 outcome (" 
                     << actual_row << "," << actual_col << ")" << endl;
            }
        }
        
        cout << endl << "Vote Summary:" << endl;
        for (const auto& vote : strategy_votes) {
            cout << "Outcome (" << vote.first.first << "," << vote.first.second 
                 << "): " << vote.second << " votes from submatrices: ";
            for (int id : strategy_supporters[vote.first]) {
                cout << id << " ";
            }
            cout << endl;
        }
        
        // Find the most supported outcome(s)
        int max_votes = 0;
        for (const auto& vote : strategy_votes) {
            max_votes = max(max_votes, vote.second);
        }
        
        cout << endl << "PREDICTED 3x3 NON-MYOPIC EQUILIBRIA:" << endl;
        bool found_prediction = false;
        for (const auto& vote : strategy_votes) {
            if (vote.second == max_votes) {
                int row = vote.first.first;
                int col = vote.first.second;
                double payoff1 = game3x3.getPayoff(row, col, 0);
                double payoff2 = game3x3.getPayoff(row, col, 1);
                
                cout << "- Pure strategy: Player 1 plays strategy " << (row + 1) 
                     << ", Player 2 plays strategy " << (col + 1) 
                     << " -> Outcome (" << payoff1 << "," << payoff2 << ")"
                     << " [Supported by " << vote.second << "/" << analyses.size() << " subgames]" << endl;
                found_prediction = true;
            }
        }
        
        if (!found_prediction) {
            cout << "No clear consensus from subgame analysis." << endl;
        }
        
        // Additional analysis: Check for dominance patterns
        cout << endl << "DOMINANCE ANALYSIS:" << endl;
        map<int, int> row_preferences; // row -> vote count
        map<int, int> col_preferences; // col -> vote count
        
        for (const auto& vote : strategy_votes) {
            row_preferences[vote.first.first] += vote.second;
            col_preferences[vote.first.second] += vote.second;
        }
        
        cout << "Player 1 row preferences: ";
        for (const auto& pref : row_preferences) {
            cout << "Row " << (pref.first + 1) << "(" << pref.second << " votes) ";
        }
        cout << endl;
        
        cout << "Player 2 column preferences: ";
        for (const auto& pref : col_preferences) {
            cout << "Col " << (pref.first + 1) << "(" << pref.second << " votes) ";
        }
        cout << endl;
    }
    
    void printGame() const {
        game3x3.printGame();
    }
};

// Function to get user input for 3x3 game matrix
Game3x3 getUserInput3x3() {
    Game3x3 game;
    cout << "Enter the payoff matrix for the 3x3 game:" << endl;
    cout << "Format: (Player 1 payoff, Player 2 payoff)" << endl << endl;
    
    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 3; j++) {
            cout << "Position (" << (i+1) << "," << (j+1) << ") - Player 1 strategy " << (i+1) 
                 << ", Player 2 strategy " << (j+1) << ":" << endl;
            
            double p1_payoff, p2_payoff;
            cout << "Player 1 payoff: ";
            cin >> p1_payoff;
            cout << "Player 2 payoff: ";
            cin >> p2_payoff;
            
            game.setPayoff(i, j, 0, p1_payoff);
            game.setPayoff(i, j, 1, p2_payoff);
            cout << endl;
        }
    }
    
    return game;
}

// Function to create example 3x3 game
Game3x3 getExample3x3Game() {
    Game3x3 game;
    
    cout << "Using example 3x3 game (Extended Coordination Game):" << endl;
    
    // Fill in a test payoff matrix (coordination-style game with dominant strategy)
    // Row 1
    game.setPayoff(0, 0, 0, 3.0); game.setPayoff(0, 0, 1, 3.0); // (3,3)
    game.setPayoff(0, 1, 0, 0.0); game.setPayoff(0, 1, 1, 5.0); // (0,5)
    game.setPayoff(0, 2, 0, 2.0); game.setPayoff(0, 2, 1, 1.0); // (2,1)
    
    // Row 2
    game.setPayoff(1, 0, 0, 5.0); game.setPayoff(1, 0, 1, 0.0); // (5,0)
    game.setPayoff(1, 1, 0, 1.0); game.setPayoff(1, 1, 1, 1.0); // (1,1)
    game.setPayoff(1, 2, 0, 4.0); game.setPayoff(1, 2, 1, 2.0); // (4,2)
    
    // Row 3
    game.setPayoff(2, 0, 0, 1.0); game.setPayoff(2, 0, 1, 2.0); // (1,2)
    game.setPayoff(2, 1, 0, 2.0); game.setPayoff(2, 1, 1, 4.0); // (2,4)
    game.setPayoff(2, 2, 0, 6.0); game.setPayoff(2, 2, 1, 6.0); // (6,6)
    
    return game;
}

int main() {
    cout << "=== 3x3 Normal Form Game Solver ===" << endl;
    cout << "This program analyzes 3x3 normal form games by decomposing them into 2x2 subgames." << endl;
    cout << "It finds equilibria for each 2x2 subgame and predicts the overall 3x3 equilibrium." << endl << endl;
    
    cout << "Choose an option:" << endl;
    cout << "1. Use example (Extended Coordination Game)" << endl;
    cout << "2. Enter custom 3x3 game matrix" << endl;
    cout << "Enter your choice (1 or 2): ";
    
    int choice;
    cin >> choice;
    
    Game3x3 game;
    
    if (choice == 1) {
        game = getExample3x3Game();
    } else if (choice == 2) {
        game = getUserInput3x3();
    } else {
        cout << "Invalid choice. Using example game." << endl;
        game = getExample3x3Game();
    }
    
    cout << "\n" << string(60, '=') << endl;
    game.printGame();
    
    // Create solver and analyze
    Game3x3Solver solver(game);
    
    cout << "Analyzing all 2x2 submatrices..." << endl;
    solver.analyzeAll2x2Submatrices();
    
    // Calculate 3x3 NME from subgame analysis
    solver.calculate3x3NMEFromSubgames();
    
    cout << "\nAnalysis complete!" << endl;
    
    return 0;
}
