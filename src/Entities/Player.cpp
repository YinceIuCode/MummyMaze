#include <Entities/Player.hpp>
#include <Core/Maze.hpp>
#include <iostream>
#include <cmath>

Player::Player() {
    m_tilePerSecond = 2.0f;
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
    m_movementSpeed = tileSize * m_tilePerSecond;

    sf::Vector2f mapOffset = map.getPosition();

    float relativeX = m_position.x - mapOffset.x;
    float relativeY = m_position.y - mapOffset.y;

    int gridX = static_cast<int>((relativeX + tileSize / 2) / tileSize);
    int gridY = static_cast<int>((relativeY + tileSize / 2) / tileSize);

    const Cell* currentCell = map.getCell(gridX, gridY);
    if (!currentCell) return;

    sf::Vector2f nextPos = m_position;
    bool hasInput = false;

    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::D) || sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Right)) {
        // Kiểm tra tường phải và giới hạn map
        if (!currentCell->wallRight && gridX < map.getWidth() - 1) {
            nextPos.x += tileSize;
            hasInput = true;
        }
    }
    else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::A) || sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Left)) {
        // Kiểm tra tường trái
        if (!currentCell->wallLeft && gridX > 0) {
            nextPos.x -= tileSize;
            hasInput = true;
        }
    }
    else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::W) || sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Up)) {
        // Kiểm tra tường trên
        if (!currentCell->wallTop && gridY > 0) {
            nextPos.y -= tileSize;
            hasInput = true;
        }
    }
    else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::S) || sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Down)) {
        // Kiểm tra tường dưới
        if (!currentCell->wallBottom && gridY < map.getHeight() - 1) {
            nextPos.y += tileSize;
            hasInput = true;
        }
    }

    if (hasInput) {
        m_targetPos = nextPos;
        m_isMoving = true;
    }
}

void Player::update(float dt) {
    if (!m_isMoving) return;

    // Logic di chuyển mượt (Interpolation) từ code cũ
    sf::Vector2f direction = m_targetPos - m_position;
    float distance = std::sqrt(direction.x * direction.x + direction.y * direction.y);

    // Quãng đường đi được trong frame này
    float step = m_movementSpeed * dt;

    if (distance <= step) {
        // Đã tới nơi
        m_position = m_targetPos;
        m_isMoving = false;
    }
    else {
        // Chưa tới nơi -> Nhích thêm 1 đoạn step
        sf::Vector2f moveVec = (direction / distance) * step; // Normalize * step
        m_position += moveVec;
    }
}

void Player::render(sf::RenderWindow& window, float scaleRatio) {
    sf::Sprite sprite(m_texture);
    sprite.setPosition(m_position);
    sf::FloatRect m_rect = sprite.getGlobalBounds();
    sprite.setOrigin({ m_rect.size.x / 2.0f, m_rect.size.y - 10.0f });
    sprite.setScale({ scaleRatio, scaleRatio });    
    window.draw(sprite);
}