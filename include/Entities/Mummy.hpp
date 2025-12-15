#pragma once
#include <vector>
#include <cmath>
#include <cstdlib>
#include <queue>
#include <string>
#include <optional> 
#include <SFML/Graphics.hpp>

class Map;
class Player;

enum class MummyAlgorithm {
    Greedy,
    Random
};

class Mummy {
public:
    Mummy();

    void loadTheme(const std::string& themeName);
    void setSpawn(int startR, int startC, float pixelX, float pixelY);
    void setMode(MummyAlgorithm m) { mode = m; }

    void move(const Map& map, const Player& player);
    void update(float dt);
    void render(sf::RenderWindow& window, float scaleRatio);

    int getR() const { return r; }
    int getC() const { return c; }
    sf::Vector2f getPosition() const { return m_position; }

    bool isMoving() const {
        return m_isMoving || !m_pathQueue.empty() || m_pauseTimer > 0.f;
    }

private:
    int r, c;
    MummyAlgorithm mode;
    std::queue<sf::Vector2f> m_pathQueue;
    float m_pauseTimer;

    std::optional<sf::Sprite> m_sprite;
    std::vector<std::vector<sf::Texture>> m_textures;
    int m_currentDir;
    int m_currentFrame;

    float m_animTimer;
    int m_animStep;

    sf::Vector2f m_position;
    sf::Vector2f m_targetPos;
    bool m_isMoving;
    float m_moveSpeed;

    void moveOnceGreedy(const Map& map, int pR, int pC);
    void moveOnceRandom(const Map& map);

    bool hasWall(const Map& map, int currR, int currC, int dirIndex) const;
    int manhattan(int rr1, int cc1, int rr2, int cc2) const;
};