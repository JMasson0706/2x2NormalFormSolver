#ifndef GAME_SOLVER_H
#define GAME_SOLVER_H

#include <iostream>
#include <vector>
#include <string>
#include <iomanip>
#include <cmath>
#include <sstream>

using namespace std;

// Structure to represent a 2x2 normal form game
struct Game2x2 {
    vector<vector<vector<double>>> payoffs; // [position][player][strategy]
    
    Game2x2() {
        // Initialize 2x2x2x2 structure: [row][col][player][strategy]
        payoffs = vector<vector<vector<double>>>(2, vector<vector<double>>(2, vector<double>(2, 0.0)));
    }
    
    // Set payoff for a specific outcome
    void setPayoff(int row, int col, int player, double payoff) {
        if (row >= 0 && row < 2 && col >= 0 && col < 2 && player >= 0 && player < 2) {
            payoffs[row][col][player] = payoff;
        }
    }
    
    // Get payoff for a specific outcome
    double getPayoff(int row, int col, int player) const {
        if (row >= 0 && row < 2 && col >= 0 && col < 2 && player >= 0 && player < 2) {
            return payoffs[row][col][player];
        }
        return 0.0;
    }
};

// Structure to represent a strategy profile
struct StrategyProfile {
    double p1_strategy1_prob; // Probability player 1 plays strategy 1
    double p2_strategy1_prob; // Probability player 2 plays strategy 1
    
    StrategyProfile(double p1 = 0.5, double p2 = 0.5) : p1_strategy1_prob(p1), p2_strategy1_prob(p2) {}
};

// Structure to represent an equilibrium
struct Equilibrium {
    StrategyProfile profile;
    bool isPure;
    string description;
    
    Equilibrium(const StrategyProfile& p, bool pure = false, const string& desc = "") 
        : profile(p), isPure(pure), description(desc) {}
};

class NormalFormSolver {
private:
    Game2x2 game;
    
    // Calculate expected payoff for a player given strategy profile
    double calculateExpectedPayoff(int player, const StrategyProfile& profile) const;
    
    // Find best response for a player given the other player's strategy
    double findBestResponse(int player, double other_player_prob, double tolerance = 1e-6) const;
    
    // Check if a strategy profile is a Nash equilibrium
    bool isNashEquilibrium(const StrategyProfile& profile, double tolerance = 1e-6) const;
    
    // Find all pure strategy Nash equilibria
    vector<Equilibrium> findPureNashEquilibria() const;
    
    // Find mixed strategy Nash equilibria
    vector<Equilibrium> findMixedNashEquilibria() const;

public:
    NormalFormSolver(const Game2x2& g) : game(g) {}
    
    // Find all Nash equilibria (both pure and mixed)
    vector<Equilibrium> findAllNashEquilibria() const;
    
    // Find non-myopic equilibrium (players look ahead and anticipate best responses)
    vector<Equilibrium> findNonMyopicEquilibria() const;
    
    // Print the game matrix
    void printGame() const;
};

#endif // GAME_SOLVER_H
