#pragma once

// --- STANDARD LIBRARIES ---
#include <vector>
#include <cmath>
#include <cstdlib>
#include <queue>
#include <string>
#include <optional> // Bắt buộc cho std::optional

// --- SFML ---
#include <SFML/Graphics.hpp>

// --- FORWARD DECLARATIONS ---
// (Giúp compile nhanh hơn, tránh include vòng tròn)
class Map;
class Player;

// --- ENUMS ---
enum class MummyAlgorithm {
    Greedy,
    Random // Viết hoa chữ đầu cho đẹp
};

class Mummy {
public:
    // ==============================
    // 1. CONSTRUCTOR & SETUP
    // ==============================
    Mummy();

    // Load ảnh và theme
    void initTexture(const std::string& texturePath);
    void loadTheme(const std::string& themeName);

    // Đặt vị trí xuất hiện (Reset trạng thái)
    void setSpawn(int startR, int startC, float pixelX, float pixelY);

    // Cài đặt chế độ (Thông minh / Ngẫu nhiên)
    void setMode(MummyAlgorithm m) { mode = m; }

    // ==============================
    // 2. MAIN GAME LOOP
    // ==============================
    // Logic tìm đường (được gọi theo lượt)
    void move(const Map& map, const Player& player);

    // Cập nhật chuyển động mượt (gọi mỗi frame)
    void update(float dt);

    // Vẽ lên màn hình
    void render(sf::RenderWindow& window, float scaleRatio);

    // ==============================
    // 3. GETTERS (Lấy thông tin)
    // ==============================
    int getR() const { return r; }
    int getC() const { return c; }

    // Cần hàm này để Map tính toán vẽ chồng hình (Z-Order)
    sf::Vector2f getPosition() const { return m_position; }

    // Kiểm tra xem Mummy có đang bận không (đang trượt hoặc đang khựng lại)
    bool isMoving() const {
        return m_isMoving || !m_pathQueue.empty() || m_pauseTimer > 0.f;
    }

private:
    // ==============================
    // 4. PRIVATE VARIABLES
    // ==============================
    // --- Logic Game ---
    int r, c;                           // Tọa độ lưới (Grid)
    MummyAlgorithm mode;                // Chế độ hiện tại
    std::queue<sf::Vector2f> m_pathQueue; // Hàng đợi các bước đi
    float m_pauseTimer;                 // Bộ đếm thời gian nghỉ

    // --- Đồ họa & Chuyển động ---
    sf::Texture m_texture;
    std::optional<sf::Sprite> m_sprite; // Sprite có thể rỗng lúc đầu

    sf::Vector2f m_position;            // Vị trí vẽ hiện tại (Pixel)
    sf::Vector2f m_targetPos;           // Vị trí đang trượt tới (Pixel)
    bool m_isMoving;                    // Cờ đánh dấu đang trượt
    float m_moveSpeed;                  // Tốc độ trượt

    // ==============================
    // 5. HELPER FUNCTIONS
    // ==============================
    // Các thuật toán tìm đường cụ thể
    void moveOnceGreedy(const Map& map, int pR, int pC);
    void moveOnceRandom(const Map& map);

    // Kiểm tra tường chắn (0: Left, 1: Right, 2: Up, 3: Down)
    bool hasWall(const Map& map, int currR, int currC, int dirIndex) const;

    // Tính khoảng cách Manhattan
    int manhattan(int rr1, int cc1, int rr2, int cc2) const;
};