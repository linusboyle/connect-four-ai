#include "base.hpp"
#include <exception>

class unexpected_player : std::exception {
public:
    virtual const char* what() { return "invalid Player";}
};

Player revert(Player state) {
    switch (state) {
    case Player::P_SELF:
        return Player::P_RIVAL;
    case Player::P_RIVAL:
        return Player::P_SELF;
    default:
        throw new unexpected_player();
    }
}

int player2Code(Player state) {
    switch (state) {
    case Player::P_SELF:
        return 2;
    case Player::P_RIVAL:
        return 1;
    default:
        throw new unexpected_player();
    }
}
