#include "States/GameState.hpp"
#include <iostream>

GameState::GameState(sf::RenderWindow* window, std::stack<std::unique_ptr<State>>* states, int mapSize)
    : State(window, states)
{
	this->m_currentMapSize = mapSize;
    this->initVariables();
}

GameState::~GameState() {}

void GameState::generateNewMaze(int mapsize) {
    generate_maze maze(mapsize);
    do {
        maze.generate();
    } while (maze.can_solve_the_maze() > 2);
    maze.print_maze();
}

void GameState::initVariables() {

    this->generateNewMaze(m_currentMapSize);

    float maxMapHeight = 720.0f;
    float tileSize = maxMapHeight / m_currentMapSize;

    m_map.setTileSize(tileSize);
	m_map.loadTheme(GameData::currentTheme);
	m_player.loadTheme(GameData::currentTheme);

    m_map.loadMap("assets/mazes/maze1.txt", m_player);

    float offsetX = 1290 / 2.f - 300.f;
    float offsetY = 210.f;

    m_map.setPosition(offsetX, offsetY);
}

void GameState::update(float dt) {
	// Xử lý Input (Theme)
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Escape)) {
        m_states->pop();
        sf::sleep(sf::milliseconds(200));
        return; // Return ngay tránh lỗi
    }

	// Xử lý Player
	m_player.processInput(m_map);
	m_player.update(dt);
}

void GameState::render(sf::RenderWindow& window) {
    m_map.draw(window, m_player);
}