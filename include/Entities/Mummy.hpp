#pragma once

#include "Core/Maze.hpp"
#include "Entities/Player.hpp"
#include <vector>
#include <cmath>
#include <cstdlib>
#include <queue>

enum class mummyAlgorithm {
    Greedy,
    random
};

class Mummy {
private:
    int r, c; // Tọa độ Grid (Hàng, Cột)
    mummyAlgorithm mode = mummyAlgorithm::Greedy;

    // Biến cho đồ họa
    std::queue<sf::Vector2f> m_pathQueue;
    float m_pauseTimer;
    
    sf::Texture m_texture;
    std::optional<sf::Sprite> m_sprite;
    sf::Vector2f m_position; // Vị trí Pixel để vẽ mượt hơn

    sf::Vector2f m_targetPos; // Vị trí đích đến (Pixel)
    bool m_isMoving;          // Cờ đang di chuyển
    float m_moveSpeed;        // Tốc độ di chuyển (Pixel/giây)

    // Hàm phụ: Kiểm tra xem có tường chắn theo hướng di chuyển không
    // dirIndex: 0=Left, 1=Right, 2=Up, 3=Down (Khớp với mảng dr/dc trong cpp)
    bool hasWall(const Map& map, int currR, int currC, int dirIndex) const;

    int manhattan(int rr1, int cc1, int rr2, int cc2) const;

public:
    Mummy();

    // Khởi tạo texture và vị trí ban đầu
    void initTexture(const std::string& texturePath);

    void loadTheme(const std::string& themeName);

    void setSpawn(int startR, int startC, const Map& map);

    void setMode(mummyAlgorithm m) { mode = m; }
    // Logic di chuyển chính
    void move(const Map& map, const Player& player);

    // Các thuật toán cụ thể
    void moveOnceGreedy(const Map& map, int pR, int pC);
    void moveOnceRandom(const Map& map);

    // Render vẽ Mummy lên màn hình
    void render(sf::RenderWindow& window, float scaleRatio);

    int getR() const { return r; }
    int getC() const { return c; }
    void update(float dt);
    bool isMoving() const {
        return m_isMoving || !m_pathQueue.empty() || m_pauseTimer > 0.f;
    }
};