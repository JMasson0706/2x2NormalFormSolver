#include <iostream>
#include <vector>
#include <string>
#include <iomanip>
#include "main.cpp" // Include the main solver

using namespace std;

void testGame(const string& name, const Game2x2& game) {
    cout << "\n" << string(60, '=') << endl;
    cout << "Testing: " << name << endl;
    cout << string(60, '=') << endl;
    
    NormalFormSolver solver(game);
    solver.printGame();
    
    // Find Nash equilibria
    vector<Equilibrium> nash_equilibria = solver.findAllNashEquilibria();
    cout << "Nash Equilibria:" << endl;
    if (nash_equilibria.empty()) {
        cout << "No Nash equilibria found." << endl;
    } else {
        for (const auto& eq : nash_equilibria) {
            cout << "- " << eq.description << endl;
        }
    }
    
    // Find non-myopic equilibria
    vector<Equilibrium> non_myopic_equilibria = solver.findNonMyopicEquilibria();
    cout << "\nNon-Myopic Equilibria:" << endl;
    if (non_myopic_equilibria.empty()) {
        cout << "No non-myopic equilibria found." << endl;
    } else {
        for (const auto& eq : non_myopic_equilibria) {
            cout << "- " << eq.description << endl;
        }
    }
    cout << endl;
}

int main() {
    cout << "=== Testing Different Game Types ===" << endl;
    
    // 1. Prisoner's Dilemma
    Game2x2 prisoner_dilemma;
    prisoner_dilemma.setPayoff(0, 0, 0, 0.0); // Both cooperate
    prisoner_dilemma.setPayoff(0, 0, 1, 3.0);
    prisoner_dilemma.setPayoff(0, 1, 0, 0.0); // P1 cooperates, P2 defects
    prisoner_dilemma.setPayoff(0, 1, 1, 5.0);
    prisoner_dilemma.setPayoff(1, 0, 0, 5.0); // P1 defects, P2 cooperates
    prisoner_dilemma.setPayoff(1, 0, 1, 0.0);
    prisoner_dilemma.setPayoff(1, 1, 0, 1.0); // Both defect
    prisoner_dilemma.setPayoff(1, 1, 1, 1.0);
    
    testGame("Prisoner's Dilemma", prisoner_dilemma);
    
    // 2. Battle of the Sexes
    Game2x2 battle_of_sexes;
    battle_of_sexes.setPayoff(0, 0, 0, 3.0); // Both prefer same activity
    battle_of_sexes.setPayoff(0, 0, 1, 2.0);
    battle_of_sexes.setPayoff(0, 1, 0, 0.0); // Different preferences
    battle_of_sexes.setPayoff(0, 1, 1, 0.0);
    battle_of_sexes.setPayoff(1, 0, 0, 0.0); // Different preferences
    battle_of_sexes.setPayoff(1, 0, 1, 0.0);
    battle_of_sexes.setPayoff(1, 1, 0, 2.0); // Both prefer same activity
    battle_of_sexes.setPayoff(1, 1, 1, 3.0);
    
    testGame("Battle of the Sexes", battle_of_sexes);
    
    // 3. Coordination Game
    Game2x2 coordination;
    coordination.setPayoff(0, 0, 0, 2.0); // Both choose A
    coordination.setPayoff(0, 0, 1, 2.0);
    coordination.setPayoff(0, 1, 0, 0.0); // Different choices
    coordination.setPayoff(0, 1, 1, 0.0);
    coordination.setPayoff(1, 0, 0, 0.0); // Different choices
    coordination.setPayoff(1, 0, 1, 0.0);
    coordination.setPayoff(1, 1, 0, 1.0); // Both choose B
    coordination.setPayoff(1, 1, 1, 1.0);
    
    testGame("Coordination Game", coordination);
    
    // 4. Matching Pennies
    Game2x2 matching_pennies;
    matching_pennies.setPayoff(0, 0, 0, 1.0); // Same choice
    matching_pennies.setPayoff(0, 0, 1, -1.0);
    matching_pennies.setPayoff(0, 1, 0, -1.0); // Different choices
    matching_pennies.setPayoff(0, 1, 1, 1.0);
    matching_pennies.setPayoff(1, 0, 0, -1.0); // Different choices
    matching_pennies.setPayoff(1, 0, 1, 1.0);
    matching_pennies.setPayoff(1, 1, 0, 1.0); // Same choice
    matching_pennies.setPayoff(1, 1, 1, -1.0);
    
    testGame("Matching Pennies", matching_pennies);
    
    return 0;
}
