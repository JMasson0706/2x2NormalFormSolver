# Normal Form Game Solver

This C++ program solves for Nash equilibria and non-myopic equilibria in 2x2 normal form games.

## Features

- **Nash Equilibrium Detection**: Finds both pure and mixed strategy Nash equilibria
- **Non-Myopic Equilibrium Detection**: Finds equilibria where players anticipate each other's best responses
- **Interactive Input**: Allows users to input custom game matrices
- **Comprehensive Output**: Displays game matrix and all found equilibria

## Game Representation

The game is represented as a 2x2 matrix where each cell contains the payoffs for both players:

```
            Player 2
            Strategy 1    Strategy 2
Player 1 Strategy 1: (a, b)    (c, d)
Player 1 Strategy 2: (e, f)    (g, h)
```

Where:
- `(a, b)` = payoffs when Player 1 plays Strategy 1 and Player 2 plays Strategy 1
- `(c, d)` = payoffs when Player 1 plays Strategy 1 and Player 2 plays Strategy 2
- `(e, f)` = payoffs when Player 1 plays Strategy 2 and Player 2 plays Strategy 1
- `(g, h)` = payoffs when Player 1 plays Strategy 2 and Player 2 plays Strategy 2

## Compilation

```bash
g++ -std=c++11 -o normal_form_solver main.cpp
```

## Usage

Run the compiled program:

```bash
./normal_form_solver
```

The program will prompt you to choose between:
1. Using the example (Prisoner's Dilemma)
2. Entering a custom game matrix

### Example: Prisoner's Dilemma

The Prisoner's Dilemma is included as an example:

```
            Player 2
            Strategy 1    Strategy 2
Player 1 Strategy 1: (3.0, 3.0)    (0.0, 5.0)
Player 1 Strategy 2: (5.0, 0.0)    (1.0, 1.0)
```

Where:
- Strategy 1 = Cooperate
- Strategy 2 = Defect

### Custom Game Input

When choosing option 2, you'll be prompted to enter the payoffs for each cell in the game matrix. The program will guide you through entering:
- Top-left cell payoffs
- Top-right cell payoffs  
- Bottom-left cell payoffs
- Bottom-right cell payoffs

## Algorithm Details

### Nash Equilibrium

The program finds Nash equilibria by:
1. **Pure Strategy Equilibria**: Checking all four pure strategy combinations to see if either player has an incentive to deviate
2. **Mixed Strategy Equilibria**: Finding mixed strategies where players are indifferent between their pure strategies

### Non-Myopic Equilibrium

Non-myopic equilibria are found by:
1. For each possible strategy profile, checking if players anticipate each other's best responses
2. Verifying that no player can improve their expected payoff by deviating, given that the other player will respond optimally
3. Using a grid search with fine granularity (0.05) to explore the strategy space

## Output

The program outputs:
- The game matrix in a readable format
- All Nash equilibria (pure and mixed)
- All non-myopic equilibria

## Example Output

```
=== 2x2 Normal Form Game Solver ===
This program finds Nash equilibria and non-myopic equilibria for 2x2 normal form games.

Choose an option:
1. Use example (Prisoner's Dilemma)
2. Enter custom game matrix
Enter your choice (1 or 2): 1

Using Prisoner's Dilemma example:

==================================================
Game Matrix:
            Player 2
            Strategy 1    Strategy 2
Player 1 Strategy 1: (3.0, 3.0)    (0.0, 5.0)
Player 1 Strategy 2: (5.0, 0.0)    (1.0, 1.0)

Nash Equilibria:
- Pure strategy NE: Player 1 plays 1, Player 2 plays 1

Non-Myopic Equilibria:
- Non-myopic equilibrium: Player 1 plays strategy 1 with probability 0.00, Player 2 plays strategy 1 with probability 0.00
```

## Theory

### Nash Equilibrium
A Nash equilibrium is a strategy profile where no player can unilaterally improve their payoff by changing their strategy, given the other player's strategy. 

### Non-Myopic Equilibrium
A non-myopic equilibrium is a strategy profile where players anticipate each other's best responses and choose strategies that lead to the best outcome given this anticipation. This is a more sophisticated equilibrium concept that considers strategic foresight. 

The key idea is that each player’s mixed strategy (say, “play strategy 1 with probability p, strategy 2 with probability 1–p”) induces a joint distribution over the four possible outcomes in a 2×2 game. In calculateExpectedPayoff, for each pure‐strategy pair (i,j) it computes

prob_i = (i==0 ? p1 : 1–p1),  
prob_j = (j==0 ? p2 : 1–p2),  
joint_prob = prob_i * prob_j;

and then multiplies that joint probability by the payoff for (i,j). Summing over all four (i,j) gives the expected payoff under the mixed strategies.

## Dependencies

- C++11 or later
- Standard C++ libraries (iostream, vector, string, algorithm, cmath, iomanip)

## License

This project is open source and available under the MIT License.

