#pragma once
#include <SFML/Graphics.hpp>

class Map;

enum Dir { Down = 0, Left = 1, Right = 2, Up = 3 };

class Player {
private:
    sf::Vector2f m_position;
    sf::Vector2f m_targetPos;
    bool m_isMoving;
    float m_movementSpeed;
    float m_tilePerSecond;

    std::vector<std::vector<sf::Texture>> m_textures;

    sf::Vector2i m_frameSize;
    sf::IntRect m_textureRect;
    int m_animStep;

    int m_currentFrame;
    int m_currentDir;

    float m_animTimer;
    float m_animSwitchTime;

public:
    Player();
    virtual ~Player();

    void loadTheme(const std::string& themeName);
    void setPosition(float x, float y);
    sf::Vector2f getPosition() const;

    void processInput(const Map& map);
    bool isMoving() const;

    void update(float dt);
    void render(sf::RenderWindow& window, float scaleRatio);
};