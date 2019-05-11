#pragma once

// remove the comment to enable debug log
// #define DEBUG

// constant values
constexpr int max_board_width = 12;
constexpr double max_search_time = 1.5;
constexpr double ucb_coefficient = 0.7;

enum class Player {
    P_RIVAL,
    P_SELF
};

// operations on player
Player revert(Player state);
int player2Code(Player state);
