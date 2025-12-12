#include <Entities/Player.hpp>
#include <Core/Maze.hpp>
#include <iostream>
#include <cmath>

const std::string DIR_NAMES[] = { "Down", "Left", "Right", "Up" };

Player::Player() {
    m_tilePerSecond = 1.5f;
    m_position = { 0.f, 0.f };

    // Mặc định load theme gốc để không bị lỗi texture rỗng
    m_targetPos = m_position;
    m_isMoving = false;

    m_animStep = 0;
    m_animSwitchTime = 0.15f;
    m_animTimer = 0.f;
    m_currentFrame = 0;
    m_currentDir = Dir::Down;

    this->loadTheme("Playmap");
}

Player::~Player() {

}

void Player::loadTheme(const std::string& themeName) {
    std::string path = "assets/textures/" + themeName + "/Explorer/";

    m_textures.clear();
    m_textures.resize(4);

    for (int dir = 0; dir < 4; dir++) {
        m_textures[dir].resize(3);

        for (int frame = 0; frame < 3; frame++) {
            std::string fileName = DIR_NAMES[dir] + "_" + std::to_string(frame) + ".png";
            std::string fullPath = path + fileName;

            if (!m_textures[dir][frame].loadFromFile(fullPath)) {
                std::cerr << "ERROR: Khong tim thay anh: " << fullPath << "\n";
            }
        }
    }

    m_currentFrame = 0;
    m_currentDir = Dir::Down;
    m_animTimer = 0.f;
}

void Player::setPosition(float x, float y) {
    m_position = { x, y };
    m_targetPos = { x, y };
    m_isMoving = false;
}

sf::Vector2f Player::getPosition() const {
    return m_position;
}

bool Player::isMoving() const {
    return m_isMoving;
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
        m_currentDir = Dir::Right;
		std::cerr << currentCell->exitVariant << "\n";
        if (!currentCell->wallRight && (gridX < map.getWidth() - 1 || currentCell->exitVariant == 2)) {
            nextPos.x += tileSize;
            hasInput = true;
        }
    }
    else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::A) || sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Left)) {
        // Kiểm tra tường trái
        m_currentDir = Dir::Left;
        std::cerr << currentCell->exitVariant << "\n";
        if (!currentCell->wallLeft && (gridX > 0 || currentCell->exitVariant == 1)) {
            nextPos.x -= tileSize;
            hasInput = true;
        }
    }
    else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::W) || sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Up)) {
        // Kiểm tra tường trên
        m_currentDir = Dir::Up;
        std::cerr << currentCell->exitVariant << "\n";
        if (!currentCell->wallTop && (gridY > 0 || currentCell->exitVariant == 0)) {
            nextPos.y -= tileSize;
            hasInput = true;
        }
    }
    else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::S) || sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Down)) {
        // Kiểm tra tường dưới
        m_currentDir = Dir::Down;
        std::cerr << currentCell->exitVariant << "\n";
        if (!currentCell->wallBottom && (gridY < map.getHeight() - 1 || currentCell->exitVariant == 3)) {
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
    if (!m_isMoving) {
        m_currentFrame = 0;
        m_animStep = 0;
        return;
    }

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

    m_animTimer += dt;
    if (m_animTimer >= 0.15f) {
        m_animTimer = 0.f;

        m_animStep++;

        int stepCycle = m_animStep % 4;

        if (stepCycle == 0) m_currentFrame = 0;      // Đứng (Trung gian)
        else if (stepCycle == 1) m_currentFrame = 1; // Chân trái
        else if (stepCycle == 2) m_currentFrame = 0; // Đứng (Trung gian)
        else if (stepCycle == 3) m_currentFrame = 2; // Chân phải
    }
}

void Player::render(sf::RenderWindow& window, float scaleRatio) {
    sf::Sprite sprite(m_textures[m_currentDir][m_currentFrame]);
    sprite.setPosition(m_position);
    sf::FloatRect m_rect = sprite.getGlobalBounds();
    sprite.setOrigin({ m_rect.size.x / 2.0f, m_rect.size.y - 10.0f });
    sprite.setScale({ scaleRatio, scaleRatio });    
    window.draw(sprite);
} 