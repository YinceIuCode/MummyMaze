#pragma once
#include <SFML/Graphics.hpp>
#include <Core/Maze.hpp>
#include <Entities/Player.hpp>

class Game {
private:
	sf::RenderWindow m_window;
	sf::VideoMode m_videoMode;
	
	sf::Clock m_dtClock;
	float m_dt;

	Map m_map;
	Player m_player;

	void initVariables();
	void initWindow();

	void updateDt();

public:
	Game();
	virtual ~Game();

	const bool isRunning() const; // Kiểm tra game có chạy không
	void update();				  // Xử lý logic, event
	void render();				  // Vẽ mọi thứ ra màn hình
};