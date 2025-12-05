#pragma once
#include <SFML/Graphics.hpp>

class Map;

class Player {
private:
	sf::Texture m_texture;

	sf::Vector2f m_position;
	sf::Vector2f m_targetPos;
	bool m_isMoving;
	float m_movementSpeed;
	float m_tilePerSecond;
public:
	Player();
	virtual ~Player();

	void loadTheme(const std::string& themeName);
	void setPosition(float x, float y);
	sf::Vector2f getPosition() const;

	void processInput(const Map& map);

	void update(float dt);
	void render(sf::RenderWindow& window, float scaleRatio);
};