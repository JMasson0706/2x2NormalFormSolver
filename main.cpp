#include "game_solver.h"
#include <iostream>
#include <vector>
#include <string>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <cmath>
#include <random>
#include <chrono>
#include <thread>
#include <mutex>
#include <iomanip>

using namespace std;

// Implementation of NormalFormSolver methods

// Calculate expected payoff for a player given strategy profile
double NormalFormSolver::calculateExpectedPayoff(int player, const StrategyProfile& profile) const {
    double expected_payoff = 0.0;
    
    for (int i = 0; i < 2; i++) {
        for (int j = 0; j < 2; j++) {
            double prob_i = (i == 0) ? profile.p1_strategy1_prob : (1 - profile.p1_strategy1_prob);
            double prob_j = (j == 0) ? profile.p2_strategy1_prob : (1 - profile.p2_strategy1_prob);
            double joint_prob = prob_i * prob_j;
            expected_payoff += joint_prob * game.getPayoff(i, j, player);
        }
    }
    
    return expected_payoff;
}

// Find best response for a player given the other player's strategy
double NormalFormSolver::findBestResponse(int player, double other_player_prob, double tolerance) const {
    double best_payoff = -INFINITY;
    double best_strategy = 0.5;
    
    // Check pure strategies first
    for (int strategy = 0; strategy <= 1; strategy++) {
        StrategyProfile test_profile;
        if (player == 0) {
            test_profile.p1_strategy1_prob = strategy;
            test_profile.p2_strategy1_prob = other_player_prob;
        } else {
            test_profile.p1_strategy1_prob = other_player_prob;
            test_profile.p2_strategy1_prob = strategy;
        }
        
        double payoff = calculateExpectedPayoff(player, test_profile);
        if (payoff > best_payoff + tolerance) {
            best_payoff = payoff;
            best_strategy = strategy;
        }
    }
    
    // Check mixed strategies
    for (double prob = 0.0; prob <= 1.0; prob += 0.01) {
        StrategyProfile test_profile;
        if (player == 0) {
            test_profile.p1_strategy1_prob = prob;
            test_profile.p2_strategy1_prob = other_player_prob;
        } else {
            test_profile.p1_strategy1_prob = other_player_prob;
            test_profile.p2_strategy1_prob = prob;
        }
        
        double payoff = calculateExpectedPayoff(player, test_profile);
        if (payoff > best_payoff + tolerance) {
            best_payoff = payoff;
            best_strategy = prob;
        }
    }
    
    return best_strategy;
}

// Check if a strategy profile is a Nash equilibrium
bool NormalFormSolver::isNashEquilibrium(const StrategyProfile& profile, double tolerance) const {
    // Check if player 1 is playing best response
    double p1_best_response = findBestResponse(0, profile.p2_strategy1_prob, tolerance);
    if (abs(profile.p1_strategy1_prob - p1_best_response) > tolerance) {
        return false;
    }
    
    // Check if player 2 is playing best response
    double p2_best_response = findBestResponse(1, profile.p1_strategy1_prob, tolerance);
    if (abs(profile.p2_strategy1_prob - p2_best_response) > tolerance) {
        return false;
    }
    
    return true;
}

// Find all pure strategy Nash equilibria
vector<Equilibrium> NormalFormSolver::findPureNashEquilibria() const {
    vector<Equilibrium> equilibria;
    
    // Check all four pure strategy combinations
    for (int i = 0; i < 2; i++) {
        for (int j = 0; j < 2; j++) {
            StrategyProfile profile(i, j);
            
            // Check if this is a Nash equilibrium
            bool p1_best_response = true;
            bool p2_best_response = true;
            
            // Check if player 1 wants to deviate
            for (int k = 0; k < 2; k++) {
                if (k != i) {
                    if (game.getPayoff(k, j, 0) > game.getPayoff(i, j, 0)) {
                        p1_best_response = false;
                        break;
                    }
                }
            }
            
            // Check if player 2 wants to deviate
            for (int k = 0; k < 2; k++) {
                if (k != j) {
                    if (game.getPayoff(i, k, 1) > game.getPayoff(i, j, 1)) {
                        p2_best_response = false;
                        break;
                    }
                }
            }
            
            if (p1_best_response && p2_best_response) {
                string desc = "Pure strategy NE: Player 1 plays " + to_string(i) + 
                            ", Player 2 plays " + to_string(j);
                equilibria.emplace_back(profile, true, desc);
            }
        }
    }
    
    return equilibria;
}

// Find mixed strategy Nash equilibria
vector<Equilibrium> NormalFormSolver::findMixedNashEquilibria() const {
    vector<Equilibrium> equilibria;
    
    // For 2x2 games, mixed strategy equilibria occur when players are indifferent
    // between their strategies given the other player's mixed strategy
    
    // Solve for player 1's mixed strategy that makes player 2 indifferent
    for (double p1 = 0.0; p1 <= 1.0; p1 += 0.01) {
        // Calculate player 2's expected payoff from strategy 1
        double p2_strategy1_payoff = p1 * game.getPayoff(0, 0, 1) + (1 - p1) * game.getPayoff(1, 0, 1);
        
        // Calculate player 2's expected payoff from strategy 2
        double p2_strategy2_payoff = p1 * game.getPayoff(0, 1, 1) + (1 - p1) * game.getPayoff(1, 1, 1);
        
        // If player 2 is indifferent, find player 2's best response
        if (abs(p2_strategy1_payoff - p2_strategy2_payoff) < 0.01) {
            double p2 = 0.5; // Indifferent player can play any mixed strategy
            
            StrategyProfile profile(p1, p2);
            if (isNashEquilibrium(profile)) {
                string desc = "Mixed strategy NE: Player 1 plays strategy 1 with probability " + 
                            to_string(p1) + ", Player 2 plays strategy 1 with probability " + to_string(p2);
                equilibria.emplace_back(profile, false, desc);
            }
        }
    }
    
    return equilibria;
}

// Find all Nash equilibria (both pure and mixed)
vector<Equilibrium> NormalFormSolver::findAllNashEquilibria() const {
    vector<Equilibrium> all_equilibria;
    
    // Find pure strategy equilibria
    vector<Equilibrium> pure_equilibria = findPureNashEquilibria();
    all_equilibria.insert(all_equilibria.end(), pure_equilibria.begin(), pure_equilibria.end());
    
    // Find mixed strategy equilibria
    vector<Equilibrium> mixed_equilibria = findMixedNashEquilibria();
    all_equilibria.insert(all_equilibria.end(), mixed_equilibria.begin(), mixed_equilibria.end());
    
    return all_equilibria;
}

// Find non-myopic equilibrium (players look ahead and anticipate best responses)
vector<Equilibrium> NormalFormSolver::findNonMyopicEquilibria() const {
    vector<Equilibrium> non_myopic_equilibria;
    
    // Non-myopic equilibrium requires that players anticipate the other's best response
    // and choose strategies that lead to the best outcome given this anticipation
    
    // Check all possible strategy profiles with finer granularity
    for (double p1 = 0.0; p1 <= 1.0; p1 += 0.05) {
        for (double p2 = 0.0; p2 <= 1.0; p2 += 0.05) {
            StrategyProfile profile(p1, p2);
            
            // Check if this is a non-myopic equilibrium
            bool is_non_myopic = true;
            
            // Player 1 anticipates player 2's best response
            double p2_best_response = findBestResponse(1, p1);
            StrategyProfile anticipated_profile(p1, p2_best_response);
            double p1_anticipated_payoff = calculateExpectedPayoff(0, anticipated_profile);
            
            // Check if player 1 can do better by deviating
            for (double new_p1 = 0.0; new_p1 <= 1.0; new_p1 += 0.05) {
                if (abs(new_p1 - p1) > 1e-6) {
                    double new_p2_best_response = findBestResponse(1, new_p1);
                    StrategyProfile new_anticipated_profile(new_p1, new_p2_best_response);
                    double new_p1_anticipated_payoff = calculateExpectedPayoff(0, new_anticipated_profile);
                    
                    if (new_p1_anticipated_payoff > p1_anticipated_payoff + 1e-6) {
                        is_non_myopic = false;
                        break;
                    }
                }
            }
            
            if (is_non_myopic) {
                // Player 2 anticipates player 1's best response
                double p1_best_response = findBestResponse(0, p2);
                StrategyProfile p2_anticipated_profile(p1_best_response, p2);
                double p2_anticipated_payoff = calculateExpectedPayoff(1, p2_anticipated_profile);
                
                // Check if player 2 can do better by deviating
                for (double new_p2 = 0.0; new_p2 <= 1.0; new_p2 += 0.05) {
                    if (abs(new_p2 - p2) > 1e-6) {
                        double new_p1_best_response = findBestResponse(0, new_p2);
                        StrategyProfile new_p2_anticipated_profile(new_p1_best_response, new_p2);
                        double new_p2_anticipated_payoff = calculateExpectedPayoff(1, new_p2_anticipated_profile);
                        
                        if (new_p2_anticipated_payoff > p2_anticipated_payoff + 1e-6) {
                            is_non_myopic = false;
                            break;
                        }
                    }
                }
            }
            
            if (is_non_myopic) {
                stringstream ss;
                ss << fixed << setprecision(2);
                ss << "Non-myopic equilibrium: Player 1 plays strategy 1 with probability " << p1 
                   << ", Player 2 plays strategy 1 with probability " << p2;
                non_myopic_equilibria.emplace_back(profile, false, ss.str());
            }
        }
    }
    
    return non_myopic_equilibria;
}

// Print the game matrix
void NormalFormSolver::printGame() const {
    cout << "Game Matrix:" << endl;
    cout << "            Player 2" << endl;
    cout << "            Strategy 1    Strategy 2" << endl;
    cout << "Player 1 Strategy 1: (" << fixed << setprecision(1) << game.getPayoff(0, 0, 0) << ", " << game.getPayoff(0, 0, 1) 
         << ")    (" << game.getPayoff(0, 1, 0) << ", " << game.getPayoff(0, 1, 1) << ")" << endl;
    cout << "Player 1 Strategy 2: (" << game.getPayoff(1, 0, 0) << ", " << game.getPayoff(1, 0, 1) 
         << ")    (" << game.getPayoff(1, 1, 0) << ", " << game.getPayoff(1, 1, 1) << ")" << endl;
    cout << endl;
}

// Function to get user input for game matrix
Game2x2 getUserInput() {
    Game2x2 game;
    cout << "Enter the payoff matrix for the 2x2 game:" << endl;
    cout << "Format: (Player 1 payoff, Player 2 payoff)" << endl << endl;
    
    cout << "Top-left (Player 1 Strategy 1, Player 2 Strategy 1):" << endl;
    cout << "Player 1 payoff: ";
    double p1_tl, p2_tl;
    cin >> p1_tl;
    cout << "Player 2 payoff: ";
    cin >> p2_tl;
    game.setPayoff(0, 0, 0, p1_tl);
    game.setPayoff(0, 0, 1, p2_tl);
    
    cout << "\nTop-right (Player 1 Strategy 1, Player 2 Strategy 2):" << endl;
    cout << "Player 1 payoff: ";
    cin >> p1_tl;
    cout << "Player 2 payoff: ";
    cin >> p2_tl;
    game.setPayoff(0, 1, 0, p1_tl);
    game.setPayoff(0, 1, 1, p2_tl);
    
    cout << "\nBottom-left (Player 1 Strategy 2, Player 2 Strategy 1):" << endl;
    cout << "Player 1 payoff: ";
    cin >> p1_tl;
    cout << "Player 2 payoff: ";
    cin >> p2_tl;
    game.setPayoff(1, 0, 0, p1_tl);
    game.setPayoff(1, 0, 1, p2_tl);
    
    cout << "\nBottom-right (Player 1 Strategy 2, Player 2 Strategy 2):" << endl;
    cout << "Player 1 payoff: ";
    cin >> p1_tl;
    cout << "Player 2 payoff: ";
    cin >> p2_tl;
    game.setPayoff(1, 1, 0, p1_tl);
    game.setPayoff(1, 1, 1, p2_tl);
    
    return game;
}

int main() {
    cout << "=== 2x2 Normal Form Game Solver ===" << endl;
    cout << "This program finds Nash equilibria and non-myopic equilibria for 2x2 normal form games." << endl << endl;
    
    cout << "Choose an option:" << endl;
    cout << "1. Use example (Prisoner's Dilemma)" << endl;
    cout << "2. Enter custom game matrix" << endl;
    cout << "Enter your choice (1 or 2): ";
    
    int choice;
    cin >> choice;
    
    Game2x2 game;
    
    if (choice == 1) {
        // Example: Prisoner's Dilemma
        cout << "\nUsing Prisoner's Dilemma example:" << endl;
        
        // Set up the game matrix (Prisoner's Dilemma)
        // Top-left: (3, 3) - both cooperate
        game.setPayoff(0, 0, 0, 3.0); // Player 1 payoff
        game.setPayoff(0, 0, 1, 3.0); // Player 2 payoff
        
        // Top-right: (0, 5) - Player 1 cooperates, Player 2 defects
        game.setPayoff(0, 1, 0, 0.0);
        game.setPayoff(0, 1, 1, 5.0);
        
        // Bottom-left: (5, 0) - Player 1 defects, Player 2 cooperates
        game.setPayoff(1, 0, 0, 5.0);
        game.setPayoff(1, 0, 1, 0.0);
        
        // Bottom-right: (1, 1) - both defect
        game.setPayoff(1, 1, 0, 1.0);
        game.setPayoff(1, 1, 1, 1.0);
    } else if (choice == 2) {
        game = getUserInput();
    } else {
        cout << "Invalid choice. Using Prisoner's Dilemma example." << endl;
        // Default to Prisoner's Dilemma
        game.setPayoff(0, 0, 0, 3.0);
        game.setPayoff(0, 0, 1, 3.0);
        game.setPayoff(0, 1, 0, 0.0);
        game.setPayoff(0, 1, 1, 5.0);
        game.setPayoff(1, 0, 0, 5.0);
        game.setPayoff(1, 0, 1, 0.0);
        game.setPayoff(1, 1, 0, 1.0);
        game.setPayoff(1, 1, 1, 1.0);
    }
    
    NormalFormSolver solver(game);
    
    cout << "\n" << string(50, '=') << endl;
    solver.printGame();
    
    // Find all Nash equilibria
    vector<Equilibrium> nash_equilibria = solver.findAllNashEquilibria();
    cout << "Nash Equilibria:" << endl;
    if (nash_equilibria.empty()) {
        cout << "No Nash equilibria found." << endl;
    } else {
        for (const auto& eq : nash_equilibria) {
            cout << "- " << eq.description << endl;
        }
    }
    cout << endl;
    
    // Find non-myopic equilibria
    vector<Equilibrium> non_myopic_equilibria = solver.findNonMyopicEquilibria();
    cout << "Non-Myopic Equilibria:" << endl;
    if (non_myopic_equilibria.empty()) {
        cout << "No non-myopic equilibria found." << endl;
    } else {
        for (const auto& eq : non_myopic_equilibria) {
            cout << "- " << eq.description << endl;
        }
    }
    cout << endl;
    
    return 0;
}