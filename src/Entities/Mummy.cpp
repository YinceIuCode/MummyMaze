#include "Entities/Mummy.hpp"

// --- QUAN TRỌNG: Phải include 2 file này vì header chỉ khai báo trước ---
#include "Core/Maze.hpp"
#include "Entities/Player.hpp"

#include <iostream>

// Định nghĩa thứ tự hướng đi: 0: Left, 1: Right, 2: Up, 3: Down
const int dr[4] = { 0, 0, -1, 1 };
const int dc[4] = { -1, 1, 0, 0 };

// ==============================
// 1. CONSTRUCTOR & SETUP
// ==============================

Mummy::Mummy() : r(0), c(0) {
    m_isMoving = false;
    m_moveSpeed = 200.0f; // Tốc độ pixel/giây (Tăng lên 200 cho mượt)
    m_pauseTimer = 0.f;
    mode = MummyAlgorithm::Greedy; // Mặc định là Greedy
}

void Mummy::initTexture(const std::string& texturePath) {
    if (!m_texture.loadFromFile(texturePath)) {
        std::cerr << "ERROR: Khong tim thay anh Mummy: " << texturePath << "\n";
    }
    m_sprite.emplace(m_texture);

    // Mặc định cho ẩn đi nếu chưa setSpawn
    m_position = { -1000.f, -1000.f };
    if (m_sprite.has_value()) {
        m_sprite->setPosition(m_position);
    }
}

void Mummy::loadTheme(const std::string& themeName) {
    // Đảm bảo đường dẫn file ảnh đúng với cấu trúc thư mục của bạn
    std::string path = "assets/textures/" + themeName + "/Mummy/mummy1.1.1.3.png";
    this->initTexture(path);
}

// Sửa tham số hàm setSpawn để nhận thêm pixelX, pixelY
void Mummy::setSpawn(int startR, int startC, float pixelX, float pixelY) {
    r = startR;
    c = startC;

    // Không cần tính toán lại dựa trên Map nữa, dùng luôn tọa độ Map gửi sang
    m_position = { pixelX, pixelY };
    m_targetPos = { pixelX, pixelY };

    // Reset trạng thái
    while (!m_pathQueue.empty()) m_pathQueue.pop();
    m_pauseTimer = 0.f;
    m_isMoving = false;

    if (m_sprite.has_value()) {
        m_sprite->setPosition(m_position);
    }
}

// ==============================
// 2. HELPER FUNCTIONS
// ==============================

bool Mummy::hasWall(const Map& map, int currR, int currC, int dirIndex) const {
    const Cell* cell = map.getCell(currC, currR); // Map::getCell nhận (x, y) tức là (col, row)
    if (!cell) return true;

    switch (dirIndex) {
    case 0: return cell->wallLeft;
    case 1: return cell->wallRight;
    case 2: return cell->wallTop;
    case 3: return cell->wallBottom;
    }
    return false;
}

int Mummy::manhattan(int rr1, int cc1, int rr2, int cc2) const {
    return std::abs(rr1 - rr2) + std::abs(cc1 - cc2);
}

// ==============================
// 3. MOVEMENT ALGORITHMS
// ==============================

void Mummy::moveOnceRandom(const Map& map) {
    // Mảng lưu các nước đi khả thi
    struct Step { int r1, c1, r2, c2; };
    std::vector<Step> possibleMoves;

    for (int k = 0; k < 4; k++) {
        // --- BƯỚC 1 ---
        if (hasWall(map, r, c, k)) continue;
        int nr1 = r + dr[k];
        int nc1 = c + dc[k];
        if (nr1 < 0 || nr1 >= map.getHeight() || nc1 < 0 || nc1 >= map.getWidth()) continue;

        // --- BƯỚC 2 ---
        if (hasWall(map, nr1, nc1, k)) continue;
        int nr2 = nr1 + dr[k];
        int nc2 = nc1 + dc[k];
        if (nr2 < 0 || nr2 >= map.getHeight() || nc2 < 0 || nc2 >= map.getWidth()) continue;

        // Lưu lại cặp bước đi (Bước 1 & Bước 2)
        possibleMoves.push_back({ nr1, nc1, nr2, nc2 });
    }

    if (possibleMoves.empty()) return; // Kẹt cứng

    // Chọn ngẫu nhiên
    int idx = std::rand() % possibleMoves.size();
    Step move = possibleMoves[idx];

    // --- NẠP VÀO HÀNG ĐỢI ---
    float tileSize = map.getTileSize();
    sf::Vector2f mapPos = map.getPosition();

    // 1. Lấy Dynamic Offset từ Map
    float offset = map.getDynamicOffset();

    // 2. Trừ offset khi tính tọa độ
    // Push Bước 1
    m_pathQueue.push({
        move.c1 * tileSize + mapPos.x - offset,  // Trừ offset
        move.r1 * tileSize + mapPos.y - offset   // Trừ offset
        });

    // Push Bước 2
    m_pathQueue.push({
        move.c2 * tileSize + mapPos.x - offset,  // Trừ offset
        move.r2 * tileSize + mapPos.y - offset   // Trừ offset
        });

    // Cập nhật tọa độ logic
    r = move.r2;
    c = move.c2;
}

void Mummy::moveOnceGreedy(const Map& map, int pR, int pC) {
    // --- TÍNH TOÁN BƯỚC 1 ---
    int r1 = r, c1 = c;
    int bestDist1 = manhattan(r, c, pR, pC);
    bool foundStep1 = false;

    for (int k = 0; k < 4; k++) {
        if (hasWall(map, r, c, k)) continue;
        int nr = r + dr[k];
        int nc = c + dc[k];
        if (nr < 0 || nr >= map.getHeight() || nc < 0 || nc >= map.getWidth()) continue;

        int d = manhattan(nr, nc, pR, pC);
        if (d < bestDist1) {
            bestDist1 = d;
            r1 = nr;
            c1 = nc;
            foundStep1 = true;
        }
    }

    // Nếu không tìm được bước 1 nào tốt hơn -> Đứng yên
    if (!foundStep1) return;

    // --- TÍNH TOÁN BƯỚC 2 (Từ vị trí r1, c1) ---
    int r2 = r1, c2 = c1;
    int bestDist2 = bestDist1;

    // Chỉ tính bước 2 nếu chưa bắt được Player
    if (bestDist1 > 0) {
        for (int k = 0; k < 4; k++) {
            if (hasWall(map, r1, c1, k)) continue;
            int nr = r1 + dr[k];
            int nc = c1 + dc[k];
            if (nr < 0 || nr >= map.getHeight() || nc < 0 || nc >= map.getWidth()) continue;

            int d = manhattan(nr, nc, pR, pC);
            if (d < bestDist2) {
                bestDist2 = d;
                r2 = nr;
                c2 = nc;
            }
        }
    }

    // --- NẠP VÀO HÀNG ĐỢI ---
    float tileSize = map.getTileSize();
    sf::Vector2f mapPos = map.getPosition();

    // 1. Lấy Dynamic Offset từ Map
    float offset = map.getDynamicOffset();

    // 2. Luôn đẩy Bước 1 (NHỚ TRỪ OFFSET)
    m_pathQueue.push({
        c1 * tileSize + mapPos.x - offset,
        r1 * tileSize + mapPos.y - offset
        });

    // 3. Chỉ đẩy Bước 2 nếu khác Bước 1 (NHỚ TRỪ OFFSET)
    if (r2 != r1 || c2 != c1) {
        m_pathQueue.push({
            c2 * tileSize + mapPos.x - offset,
            r2 * tileSize + mapPos.y - offset
            });
    }

    // Cập nhật tọa độ logic
    r = r2;
    c = c2;
}

// ==============================
// 4. MAIN GAME LOOP
// ==============================

void Mummy::move(const Map& map, const Player& player) {
    // Nếu đang di chuyển hoặc hàng đợi chưa rỗng thì không nhận lệnh mới
    if (isMoving()) return;

    // 1. Quy đổi tọa độ Player từ Pixel sang Grid
    float tileSize = map.getTileSize();
    sf::Vector2f mapPos = map.getPosition();
    sf::Vector2f pPos = player.getPosition();

    int pC = static_cast<int>((pPos.x - mapPos.x + tileSize / 2) / tileSize);
    int pR = static_cast<int>((pPos.y - mapPos.y + tileSize / 2) / tileSize);

    // 2. Gọi thuật toán di chuyển (Các hàm này sẽ nạp bước đi vào m_pathQueue)
    switch (mode) {
    case MummyAlgorithm::Greedy:
        moveOnceGreedy(map, pR, pC);
        break;
    case MummyAlgorithm::Random:
        moveOnceRandom(map);
        break;
    }

    // 3. KICKSTART: Lấy ngay điểm đầu tiên để bắt đầu di chuyển
    if (!m_pathQueue.empty()) {
        m_targetPos = m_pathQueue.front();
        m_pathQueue.pop();
        m_isMoving = true;
    }
}

void Mummy::update(float dt) {
    // 1. Xử lý thời gian nghỉ (Pause)
    if (m_pauseTimer > 0.f) {
        m_pauseTimer -= dt;
        return;
    }

    // 2. Tự động lấy bước tiếp theo từ hàng đợi (nếu đang đứng yên)
    if (!m_isMoving && !m_pathQueue.empty()) {
        m_targetPos = m_pathQueue.front();
        m_pathQueue.pop();
        m_isMoving = true;
    }

    // 3. Logic trượt (Interpolation)
    if (m_isMoving) {
        sf::Vector2f direction = m_targetPos - m_position;
        float distance = std::sqrt(direction.x * direction.x + direction.y * direction.y);
        float step = m_moveSpeed * dt;

        if (distance <= step) {
            // Đã đến đích của bước này
            m_position = m_targetPos;
            m_isMoving = false;

            // Nếu vẫn còn bước nữa trong hàng đợi -> Set thời gian khựng
            if (!m_pathQueue.empty()) {
                m_pauseTimer = 0.15f; // Khựng 0.15s giữa 2 bước
            }
        }
        else {
            // Chưa đến -> Nhích tiếp
            sf::Vector2f moveVec = (direction / distance) * step;
            m_position += moveVec;
        }

        // Cập nhật Sprite
        if (m_sprite.has_value()) {
            m_sprite->setPosition(m_position);
        }
    }
}

void Mummy::render(sf::RenderWindow& window, float scaleRatio) {
    if (!m_sprite.has_value()) return;

    // Lấy kích thước Texture gốc
    sf::Vector2u texSize = m_texture.getSize();

    // Set Origin ở giữa chân để căn vị trí cho chuẩn
    m_sprite->setOrigin({ texSize.x / 2.0f, texSize.y - 10.0f });
    m_sprite->setScale({ scaleRatio, scaleRatio });
    m_sprite->setPosition(m_position);

    window.draw(*m_sprite);
}