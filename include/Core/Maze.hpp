#pragma once
#include <SFML/Graphics.hpp>
#include <vector>
#include <string>

class Player;

struct Cell {
	bool wallTop = false;
	bool wallBottom = false;
	bool wallLeft = false;
	bool wallRight = false;

	int floorVariant = 0;
};

class Map {
private:
	float m_tileSize;
	float scaleRatio;
	float dynamicOffset;
	sf::Vector2f posmap;
	std::vector<std::vector<Cell>> m_grid;
	int m_width;
	int m_height;

	sf::Texture m_texWallHoriz;           // tường ngang
	sf::Texture m_texWallVert;            // tường dọc
	std::vector<sf::Texture> m_texFloors; // Mảng chứa 5 loại gạch
	std::vector<sf::Texture> m_texBorders;// Mảng chứa 4 khung tường


public:
	Map();
	~Map();

	void loadMap(const std::string& filePath, Player& player);
	void loadTheme(const std::string& themeName);
	float getTileSize() const;
	const Cell* getCell(int x, int y) const;
	int getWidth() const;                   
	int getHeight() const;
	void setTileSize(float size);
	void draw(sf::RenderWindow& window, Player& player);
	void setPosition(float x, float y);
	sf::Vector2f getPosition() const;
};