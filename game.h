#ifndef GAME_H
#define GAME_H

#include <vector>

class Game {
public:
    void setup();
    void play();

private:
    static const int BOARD_SIZE = 10;
    std::vector<std::vector<char>> playerBoard;
    std::vector<std::vector<char>> computerBoard;

    void initializeBoard(std::vector<std::vector<char>>& board);
    void printBoard(const std::vector<std::vector<char>>& board, bool reveal = false);
    bool isHit(const std::vector<std::vector<char>>& board, int x, int y);
    void placeShips(std::vector<std::vector<char>>& board);
    bool playerTurn();
    bool computerTurn();
    bool allShipsSunk(const std::vector<std::vector<char>>& board);
    bool canPlaceShip(const std::vector<std::vector<char>>& board, int x, int y, int size, int direction);
    bool isShipSunk(const std::vector<std::vector<char>>& board, int x, int y);  // Изменили void на bool
};

#endif
