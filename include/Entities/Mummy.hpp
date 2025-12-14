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

// --- ENUMS ---
enum class MummyAlgorithm {
    Greedy,
    Random // Viết hoa chữ đầu cho đẹp
};

class Mummy {
public:
    Mummy();

    // Load ảnh và theme
    void loadTheme(const std::string& themeName);

    // Đặt vị trí xuất hiện (Reset trạng thái)
    void setSpawn(int startR, int startC, float pixelX, float pixelY);

    // Cài đặt chế độ (Thông minh / Ngẫu nhiên)
    void setMode(MummyAlgorithm m) { mode = m; }

    // Logic tìm đường (được gọi theo lượt)
    void move(const Map& map, const Player& player);

    // Cập nhật chuyển động mượt (gọi mỗi frame)
    void update(float dt);

    // Vẽ lên màn hình
    void render(sf::RenderWindow& window, float scaleRatio);

    int getR() const { return r; }
    int getC() const { return c; }

    sf::Vector2f getPosition() const { return m_position; }

    bool isMoving() const {
        return m_isMoving || !m_pathQueue.empty() || m_pauseTimer > 0.f;
    }

private:
    // --- Logic Game ---
    int r, c;                           
    MummyAlgorithm mode;                
    std::queue<sf::Vector2f> m_pathQueue; 
    float m_pauseTimer;                 

    // --- Đồ họa & Chuyển động ---
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

    // Các thuật toán tìm đường cụ thể
    void moveOnceGreedy(const Map& map, int pR, int pC);
    void moveOnceRandom(const Map& map);

    // Kiểm tra tường chắn (0: Left, 1: Right, 2: Up, 3: Down)
    bool hasWall(const Map& map, int currR, int currC, int dirIndex) const;

    // Tính khoảng cách Manhattan
    int manhattan(int rr1, int cc1, int rr2, int cc2) const;
};