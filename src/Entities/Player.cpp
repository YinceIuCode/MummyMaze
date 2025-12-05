#include <Entities/Player.hpp>
#include <Core/Maze.hpp>
#include <iostream>
#include <cmath>

Player::Player() {
    m_movementSpeed = 200.f;
    m_position = { 0.f, 0.f };

    // Mặc định load theme gốc để không bị lỗi texture rỗng
    m_targetPos = m_position;
    m_isMoving = false;

    this->loadTheme("Playmap");
}

Player::~Player() {

}

void Player::loadTheme(const std::string& themeName) {
    // Tạo đường dẫn: "assets/textures/Nobita/player.png"
    std::string path = "assets/textures/" + themeName + "/Explorer/exlporer1.1.1.3.png";

    if (!m_texture.loadFromFile(path)) {
        std::cerr << "ERROR::PLAYER::COULD_NOT_LOAD: " << path << "\n";
    }
}

void Player::setPosition(float x, float y) {
    m_position = { x, y };
    m_targetPos = { x, y };
    m_isMoving = false;
}

sf::Vector2f Player::getPosition() const {
    return m_position;
}

void Player::processInput(const Map& map) {
    if (m_isMoving) return;

    float tileSize = map.getTileSize();

    int gridX = static_cast<int>(m_position.x / tileSize);
    int gridY = static_cast<int>(m_position.y / tileSize);

    const Cell* currentCell = map.getCell(gridX, gridY);
}

void Player::update(float dt) {
    sf::Vector2f velocity(0.f, 0.f);
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Up)) velocity.y -= 1.f;
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Down)) velocity.y += 1.f;
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Left)) velocity.x -= 1.f;
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Right)) velocity.x += 1.f;

    if (velocity.x != 0 || velocity.y != 0) {
        m_position += velocity * m_movementSpeed * dt;
    }
}

void Player::render(sf::RenderWindow& window) {
    sf::Sprite sprite(m_texture);
    sprite.setPosition(m_position);
    sf::FloatRect m_rect = sprite.getGlobalBounds();
    sprite.setOrigin({ m_rect.size.x / 2.0f, m_rect.size.y - 10.0f });
    
    window.draw(sprite);
}