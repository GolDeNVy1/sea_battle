﻿#include <iostream>
#include <thread>
#include <chrono>
#include <vector>
#include <cctype>
#include <limits>
#include "game.h"
#include "utils.h"

void Game::setup() {
	// Инициализируем игровые доски для игрока и компьютера
	playerBoard.resize(BOARD_SIZE, std::vector<char>(BOARD_SIZE, '-'));
	computerBoard.resize(BOARD_SIZE, std::vector<char>(BOARD_SIZE, '-'));

	std::cout << "Placing ships for the player..." << std::endl;
	placeShips(playerBoard); // Размещаем корабли игрока
	std::cout << "Placing ships for the computer..." << std::endl;
	placeShips(computerBoard); // Размещаем корабли компьютера
}

void Game::play() {
	bool gameRunning = true;

	while (gameRunning) {
		// Выводим доски игрока и компьютера
		std::cout << "Player's board:" << std::endl;
		printBoard(playerBoard, true);

		std::cout << "Computer's board:" << std::endl;
		printBoard(computerBoard, false); // Доска компьютера скрыта, показываем только результаты атак

		// Ход игрока
		while (true) {
			bool playerTurnContinue = playerTurn(); // Игрок делает свой ход
			if (allShipsSunk(computerBoard)) { // Проверка, все ли корабли компьютера потоплены
				std::cout << "You won! All computer ships are sunk!" << std::endl;
				return;
			}
			if (!playerTurnContinue) { // Если игрок промахнулся, переходим к ходу компьютера
				break;
			}
		}

		// Ход компьютера
		while (true) {
			bool computerTurnContinue = computerTurn(); // Компьютер делает свой ход
			if (allShipsSunk(playerBoard)) { // Проверка, все ли корабли игрока потоплены
				std::cout << "Computer won! All your ships are sunk!" << std::endl;
				return;
			}
			if (!computerTurnContinue) { // Если компьютер промахнулся, переходим к следующему ходу игрока
				break;
			}
		}

		std::this_thread::sleep_for(std::chrono::seconds(1)); // Пауза перед следующим ходом
	}
}

void Game::initializeBoard(std::vector<std::vector<char>>& board) {
	for (int i = 0; i < BOARD_SIZE; ++i) {
		for (int j = 0; j < BOARD_SIZE; ++j) {
			board[i][j] = '-'; // Инициализируем пустую доску
		}
	}
}

void Game::printBoard(const std::vector<std::vector<char>>& board, bool reveal) {
	std::cout << "  ";
	for (int j = 0; j < BOARD_SIZE; ++j) {
		std::cout << static_cast<char>('A' + j) << " ";  // Выводим заголовок с буквами
	}
	std::cout << std::endl;

	for (int i = 0; i < BOARD_SIZE; ++i) {
		std::cout << i << " ";  // Выводим номер строки
		for (int j = 0; j < BOARD_SIZE; ++j) {
			if (reveal || board[i][j] == 'X' || board[i][j] == 'O') {
				// Если нужно раскрыть доску или это результат атаки, показываем содержимое
				std::cout << board[i][j] << ' ';
			}
			else {
				// Если это не результат атаки, показываем только пустые клетки
				std::cout << "- ";
			}
		}
		std::cout << std::endl;
	}
}

void Game::placeShips(std::vector<std::vector<char>>& board) {
	int shipSizes[] = { 4, 3, 3, 2, 2, 2, 1, 1, 1, 1 }; // Размеры кораблей

	for (int size : shipSizes) {
		bool placed = false;
		while (!placed) {
			int x = getRandomNumber(0, BOARD_SIZE - 1); // Случайная координата X
			int y = getRandomNumber(0, BOARD_SIZE - 1); // Случайная координата Y
			int direction = getRandomNumber(0, 1); // Случайное направление (горизонтальное или вертикальное)

			// Проверяем, можем ли разместить корабль на выбранных координатах
			if (canPlaceShip(board, x, y, size, direction)) {
				for (int i = 0; i < size; ++i) {
					// Размещаем корабль на доске
					if (direction == 0) {
						board[x][y + i] = 'S'; // Горизонтальное размещение
					}
					else {
						board[x + i][y] = 'S'; // Вертикальное размещение
					}
				}
				placed = true; // Успешно разместили корабль
			}
		}
	}
}

bool Game::canPlaceShip(const std::vector<std::vector<char>>& board, int x, int y, int size, int direction) {
	if (direction == 0) { // Горизонтальное размещение
		if (y + size > BOARD_SIZE) return false; // Проверка выхода за пределы

		for (int i = -1; i <= size; ++i) {
			for (int j = -1; j <= 1; ++j) {
				// Проверяем, есть ли свободное место вокруг корабля
				int checkX = x + j;
				int checkY = y + i;

				if (checkX >= 0 && checkX < BOARD_SIZE && checkY >= 0 && checkY < BOARD_SIZE) {
					if (i >= 0 && i < size && j == 0) continue; // Пропускаем сам корабль
					if (board[checkX][checkY] != '-') return false; // Если находим занятое место, возвращаем false
				}
			}
		}
	}
	else { // Вертикальное размещение
		if (x + size > BOARD_SIZE) return false; // Проверка выхода за пределы

		for (int i = -1; i <= size; ++i) {
			for (int j = -1; j <= 1; ++j) {
				int checkX = x + i;
				int checkY = y + j;

				if (checkX >= 0 && checkX < BOARD_SIZE && checkY >= 0 && checkY < BOARD_SIZE) {
					if (i >= 0 && i < size && j == 0) continue; // Пропускаем сам корабль
					if (board[checkX][checkY] != '-') return false; // Если находим занятое место, возвращаем false
				}
			}
		}
	}
	return true; // Если всё в порядке, возвращаем true
}

bool Game::isShipSunk(const std::vector<std::vector<char>>& board, int x, int y) {
	// Проверяем, все ли части корабля на позиции потоплены
	for (int i = y; i >= 0 && board[x][i] != '-'; --i) {
		if (board[x][i] == 'S') return false; // Если находим часть корабля, возвращаем false
	}
	for (int i = y; i < BOARD_SIZE && board[x][i] != '-'; ++i) {
		if (board[x][i] == 'S') return false;
	}

	for (int i = x; i >= 0 && board[i][y] != '-'; --i) {
		if (board[i][y] == 'S') return false;
	}
	for (int i = x; i < BOARD_SIZE && board[i][y] != '-'; ++i) {
		if (board[i][y] == 'S') return false;
	}

	return true; // Если все части потоплены, возвращаем true
}

bool Game::isHit(const std::vector<std::vector<char>>& board, int x, int y) {
	// Проверяем, попали ли мы по кораблю
	return board[x][y] == 'S';
}

bool Game::playerTurn() {
	char rowChar;
	int col;

	while (true) {
		std::cout << "Enter coordinates to attack (format: A1): ";
		std::string input;
		std::cin >> input;

		
		if (input.length() != 2) {
			std::cout << "Invalid coordinates! Format must be like A1. Try again." << std::endl;
			continue;
		}

		rowChar = std::toupper(input[0]); // Приводим букву к верхнему регистру

		// Проверяем, попадает ли буква в диапазон A-J
		if (rowChar < 'A' || rowChar > 'J') {
			std::cout << "Invalid row! Only A-J allowed. Try again." << std::endl;
			continue;
		}

		// Проверяем, попадает ли цифра в диапазон 0-9
		if (!std::isdigit(input[1])) {
			std::cout << "Invalid column! Only 0-9 allowed. Try again." << std::endl;
			continue;
		}

		col = input[1] - '0';  // Преобразуем символ цифры в число

		if (col < 0 || col >= BOARD_SIZE) {
			std::cout << "Invalid column! Only 0-9 allowed. Try again." << std::endl;
			continue;
		}

		int x = col;            
		int y = rowChar - 'A';  // Y-координата, вычисленная из буквы

		std::cout << "Player attacks at (" << rowChar << ", " << x << ")" << std::endl;

		// Проверяем, попали ли мы по кораблю компьютера
		if (isHit(computerBoard, x, y)) {
			std::cout << "Hit!" << std::endl;
			computerBoard[x][y] = 'X'; // Помечаем попадание
			printBoard(computerBoard, false); // Печатаем доску компьютера
			if (isShipSunk(computerBoard, x, y)) { // Проверяем, потоплен ли корабль
				std::cout << "Ship sunk!" << std::endl;
			}
			return true; 
		}
		else {
			std::cout << "Miss!" << std::endl;
			computerBoard[x][y] = 'O'; // Помечаем промах
			return false; 
		}
	}
}


bool Game::computerTurn() {
	int x = getRandomNumber(0, BOARD_SIZE - 1);
	int y = getRandomNumber(0, BOARD_SIZE - 1); // Компьютер выбирает случайные координаты для атаки

	std::cout << "Computer attacks at (" << static_cast<char>('A' + y) << ", " << x << ")" << std::endl;

	if (isHit(playerBoard, x, y)) {
		std::cout << "Computer hit!" << std::endl;
		playerBoard[x][y] = 'X'; // Помечаем попадание
		if (isShipSunk(playerBoard, x, y)) { // Проверяем, потоплен ли корабль игрока
			std::cout << "Your ship is sunk!" << std::endl;
		}
		return true; // Возвращаем true, если был успех
	}
	else {
		std::cout << "Computer missed!" << std::endl;
		playerBoard[x][y] = 'O'; // Помечаем промах
		return false; // Возвращаем false, если не попали
	}
}

bool Game::allShipsSunk(const std::vector<std::vector<char>>& board) {
	// Проверяем, остались ли корабли на доске
	for (const auto& row : board) {
		for (char cell : row) {
			if (cell == 'S') return false; // Если находим хотя бы одну часть корабля, возвращаем false
		}
	}
	return true; // Если кораблей не осталось, возвращаем true
}
