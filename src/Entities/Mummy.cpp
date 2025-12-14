#include "Entities/Mummy.hpp"
#include "Core/Maze.hpp"
#include "Entities/Player.hpp"
#include <iostream>

const std::string MUMMY_DIR_NAMES[] = { "Down", "Left", "Right", "Up" };
const int dr[4] = { 0, 0, -1, 1 };
const int dc[4] = { -1, 1, 0, 0 };

Mummy::Mummy() : r(0), c(0) {
    m_isMoving = false;
    m_moveSpeed = 200.0f; 
    m_pauseTimer = 0.f;
    mode = MummyAlgorithm::Greedy;

    m_currentDir = 0;
    m_currentFrame = 0;
    m_animTimer = 0.f;
    m_animStep = 0;

    m_position = { -1000.f, -1000.f };
}

void Mummy::loadTheme(const std::string& themeName) {
    std::string path = "assets/textures/" + themeName + "/Mummy/";

    m_textures.clear();
    m_textures.resize(4); 

    for (int dir = 0; dir < 4; dir++) {
        m_textures[dir].resize(3); 

        for (int frame = 0; frame < 3; frame++) {
            std::string fileName = MUMMY_DIR_NAMES[dir] + "_" + std::to_string(frame) + ".png";
            std::string fullPath = path + fileName;

            if (!m_textures[dir][frame].loadFromFile(fullPath)) {
                std::cerr << "ERROR: Khong tim thay anh Mummy: " << fullPath << "\n";
                if (frame == 0 && dir == 0) {
                    if (!m_textures[dir][frame].loadFromFile("assets/textures/" + themeName + "/Mummy/mummy1.1.1.3.png")) {
						std::cerr << "ERROR: Khong tim thay anh Mummy fallback!\n";
                    }
                }
            }
        }
    }

    m_currentFrame = 0;
    m_currentDir = 0; 
    m_animTimer = 0.f;
}

void Mummy::setSpawn(int startR, int startC, float pixelX, float pixelY) {
    r = startR;
    c = startC;

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

bool Mummy::hasWall(const Map& map, int currR, int currC, int dirIndex) const {
    const Cell* cell = map.getCell(currC, currR);
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

void Mummy::moveOnceRandom(const Map& map) {
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
    // 1. Xử lý Pause
    if (m_pauseTimer > 0.f) {
        m_pauseTimer -= dt;
        // Reset về dáng đứng khi pause
        m_currentFrame = 0;
        return;
    }

    // 2. Lấy bước đi từ hàng đợi
    if (!m_isMoving && !m_pathQueue.empty()) {
        m_targetPos = m_pathQueue.front();
        m_pathQueue.pop();
        m_isMoving = true;
    }

    // 3. Logic di chuyển & Animation
    if (m_isMoving) {
        sf::Vector2f direction = m_targetPos - m_position;
        float distance = std::sqrt(direction.x * direction.x + direction.y * direction.y);
        float step = m_moveSpeed * dt;

        // --- A. TÍNH HƯỚNG (MỚI) ---
        // Xác định hướng dựa trên vector di chuyển để chọn sprite đúng
        if (std::abs(direction.x) > std::abs(direction.y)) {
            // Đi ngang
            if (direction.x > 0) m_currentDir = 2; // Right
            else m_currentDir = 1; // Left
        }
        else {
            // Đi dọc
            if (direction.y > 0) m_currentDir = 0; // Down
            else m_currentDir = 3; // Up
        }

        // --- B. DI CHUYỂN (GIỮ NGUYÊN) ---
        if (distance <= step) {
            m_position = m_targetPos;
            m_isMoving = false;
            if (!m_pathQueue.empty()) {
                m_pauseTimer = 0.15f;
            }
        }
        else {
            sf::Vector2f moveVec = (direction / distance) * step;
            m_position += moveVec;
        }

        // --- C. ANIMATION (GIỐNG PLAYER) ---
        m_animTimer += dt;
        if (m_animTimer >= 0.15f) { // Tốc độ đổi frame (0.15s)
            m_animTimer = 0.f;
            m_animStep++;

            int stepCycle = m_animStep % 4;
            // Chu kỳ đi bộ: Đứng -> Chân Trái -> Đứng -> Chân Phải
            if (stepCycle == 0) m_currentFrame = 0;
            else if (stepCycle == 1) m_currentFrame = 1;
            else if (stepCycle == 2) m_currentFrame = 0;
            else if (stepCycle == 3) m_currentFrame = 2;
        }
    }
    else {
        // Nếu đứng yên thì reset về frame 0
        m_currentFrame = 0;
        m_animStep = 0;
    }

    // Cập nhật vị trí Sprite cũ (nếu bạn vẫn dùng m_sprite làm biến tạm)
    // Nhưng hàm render ở dưới sẽ dùng m_textures nên dòng này không quá quan trọng
    if (m_sprite.has_value()) {
        m_sprite->setPosition(m_position);
    }
}

void Mummy::render(sf::RenderWindow& window, float scaleRatio) {
    // Kiểm tra an toàn: Đảm bảo texture đã load đủ
    if (m_textures.size() <= m_currentDir || m_textures[m_currentDir].size() <= m_currentFrame) return;

    // Tạo sprite từ frame hiện tại
    sf::Sprite sprite(m_textures[m_currentDir][m_currentFrame]);

    sprite.setPosition(m_position);

    // Căn Origin: Giữa chân
    sf::FloatRect bounds = sprite.getGlobalBounds();
    sprite.setOrigin({ bounds.size.x / 2.0f, bounds.size.y - 10.0f });

    sprite.setScale({ scaleRatio, scaleRatio });

    window.draw(sprite);
}