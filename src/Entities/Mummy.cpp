#include "Entities/Mummy.hpp"
#include <iostream>

// Định nghĩa thứ tự hướng đi khớp với dr/dc bên dưới
// 0: Left, 1: Right, 2: Up, 3: Down
const int dr[4] = { 0, 0, -1, 1 };
const int dc[4] = { -1, 1, 0, 0 };

Mummy::Mummy() : r(0), c(0) {
    m_isMoving = false;
    m_moveSpeed = 150.0f;
}

void Mummy::initTexture(const std::string& texturePath) {
    if (!m_texture.loadFromFile(texturePath)) {
        std::cerr << "ERROR: Khong tim thay anh Mummy: " << texturePath << "\n";
    }
    m_sprite.emplace(m_texture);

    // Mặc định cho ẩn đi nếu chưa setSpawn
    m_position = { -1000.f, -1000.f };
    m_sprite->setPosition(m_position);
}

void Mummy::loadTheme(const std::string& themeName) {
    // Tạo đường dẫn tự động: assets/textures/[ThemeName]/Mummy/mummy.png
    // Bạn nhớ đảm bảo trong thư mục theme có folder "Mummy" và file ảnh nhé
    std::string path = "assets/textures/" + themeName + "/Mummy/mummy1.1.1.3.png";

    // Gọi lại hàm load ảnh gốc
    this->initTexture(path);
}

void Mummy::setSpawn(int startR, int startC, const Map& map) {
    r = startR;
    c = startC;

    float tileSize = map.getTileSize();
    sf::Vector2f mapPos = map.getPosition();

    // Tính toán vị trí pixel
    float px = c * tileSize + mapPos.x;
    float py = r * tileSize + mapPos.y;

    m_position = { px, py };
    m_targetPos = { px, py };

    // Xóa sạch hàng đợi cũ và reset timer
    while (!m_pathQueue.empty()) m_pathQueue.pop();
    m_pauseTimer = 0.f;
    m_isMoving = false;

    if (m_sprite.has_value()) m_sprite->setPosition(m_position);
}

// Kiểm tra tường dựa trên Cell của Map
bool Mummy::hasWall(const Map& map, int currR, int currC, int dirIndex) const {
    const Cell* cell = map.getCell(currC, currR); // Lưu ý: getCell(x, y) tức là (col, row)
    if (!cell) return true; // Ngoài map coi như có tường

    // 0: Left, 1: Right, 2: Up, 3: Down
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
    int rDest[4];
    int cDest[4];
    int count = 0;
    int rMid[4], cMid[4];

    for (int k = 0; k < 4; k++) {
        // --- BƯỚC 1 ---
        // Kiểm tra tường từ ô hiện tại
        if (hasWall(map, r, c, k)) continue;

        int nr = r + dr[k];
        int nc = c + dc[k];

        // Kiểm tra biên map
        if (nr < 0 || nr >= map.getHeight() || nc < 0 || nc >= map.getWidth()) continue;

        // --- BƯỚC 2 (Mummy đi 2 ô) ---
        // Kiểm tra tường từ ô trung gian (nr, nc)
        if (hasWall(map, nr, nc, k)) continue;

        int nr2 = nr + dr[k];
        int nc2 = nc + dc[k];

        if (nr2 < 0 || nr2 >= map.getHeight() || nc2 < 0 || nc2 >= map.getWidth()) continue;

        // Nếu đi được cả 2 bước, lưu lại
        rDest[count] = nr2;
        cDest[count] = nc2;
        rMid[count] = nr;   cMid[count] = nc;
        count++;
    }

    if (count == 0) return; // Kẹt cứng

    int idx = std::rand() % count;

    // --- XỬ LÝ QUEUE ---
    float tileSize = map.getTileSize();
    sf::Vector2f mapPos = map.getPosition();

    // Push Bước 1
    float px1 = cMid[idx] * tileSize + mapPos.x;
    float py1 = rMid[idx] * tileSize + mapPos.y;
    m_pathQueue.push({ px1, py1 });

    // Push Bước 2
    float px2 = cDest[idx] * tileSize + mapPos.x;
    float py2 = rDest[idx] * tileSize + mapPos.y;
    m_pathQueue.push({ px2, py2 });

    // Cập nhật logic
    r = rDest[idx];
    c = cDest[idx];
}

void Mummy::moveOnceGreedy(const Map& map, int pR, int pC) {
    // --- TÍNH TOÁN BƯỚC 1 ---
    int r1 = r, c1 = c;
    int bestDist1 = manhattan(r, c, pR, pC);

    // Duyệt 4 hướng để tìm ô hàng xóm gần Player nhất
    for (int k = 0; k < 4; k++) {
        // Check tường chặn ngay bước 1
        if (hasWall(map, r, c, k)) continue;

        int nr = r + dr[k];
        int nc = c + dc[k];

        // Check biên map
        if (nr < 0 || nr >= map.getHeight() || nc < 0 || nc >= map.getWidth()) continue;

        int d = manhattan(nr, nc, pR, pC);

        // Nếu khoảng cách mới NHỎ HƠN khoảng cách cũ
        if (d < bestDist1) {
            bestDist1 = d;
            r1 = nr;
            c1 = nc;
        }
    }

    // Nếu không tìm được bước 1 nào tốt hơn -> Đứng yên (Bị kẹt hoặc đã trùng vị trí Player)
    if (r1 == r && c1 == c) return;


    // --- TÍNH TOÁN BƯỚC 2 (Từ vị trí r1, c1) ---
    int r2 = r1, c2 = c1;
    int bestDist2 = bestDist1; // Khoảng cách từ bước 1 tới Player

    // Nếu bước 1 đã bắt được Player rồi (dist=0) thì không cần tính bước 2 nữa
    if (bestDist1 > 0) {
        for (int k = 0; k < 4; k++) {
            // Check tường từ vị trí bước 1
            if (hasWall(map, r1, c1, k)) continue;

            int nr = r1 + dr[k];
            int nc = c1 + dc[k];

            if (nr < 0 || nr >= map.getHeight() || nc < 0 || nc >= map.getWidth()) continue;

            int d = manhattan(nr, nc, pR, pC);

            // Tìm bước nào giúp gần Player hơn nữa
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

    // 1. Luôn đẩy Bước 1 vào hàng đợi
    float px1 = c1 * tileSize + mapPos.x;
    float py1 = r1 * tileSize + mapPos.y;
    m_pathQueue.push({ px1, py1 });

    // 2. Chỉ đẩy Bước 2 nếu nó khác Bước 1 (Tức là Mummy đi được tiếp)
    if (r2 != r1 || c2 != c1) {
        float px2 = c2 * tileSize + mapPos.x;
        float py2 = r2 * tileSize + mapPos.y;
        m_pathQueue.push({ px2, py2 });
    }

    // Cập nhật tọa độ logic cuối cùng
    r = r2;
    c = c2;
}

void Mummy::move(const Map& map, const Player& player) {
    if (m_isMoving) return;
    // 1. Quy đổi tọa độ Player từ Pixel sang Grid
    float tileSize = map.getTileSize();
    sf::Vector2f mapPos = map.getPosition();
    sf::Vector2f pPos = player.getPosition();

    int oldR = r;
    int oldC = c;

    // Logic này lấy từ Player::processInput
    // Trừ offset map rồi chia cho size gạch
    int pC = static_cast<int>((pPos.x - mapPos.x + tileSize / 2) / tileSize);
    int pR = static_cast<int>((pPos.y - mapPos.y + tileSize / 2) / tileSize);

    // 2. Gọi thuật toán di chuyển
    switch (mode) {
    case mummyAlgorithm::Greedy:
        moveOnceGreedy(map, pR, pC);
        break;
    case mummyAlgorithm::random:
        moveOnceRandom(map);
        break;
    }

    // 3. Cập nhật lại vị trí hiển thị (Pixel) cho khớp với Grid mới
    // (Lưu ý: Bạn có thể thêm code interpolation ở Update để Mummy trượt mượt mà thay vì nhảy cóc)
    if (!m_pathQueue.empty()) {
        m_targetPos = m_pathQueue.front(); // Lấy điểm đến bước 1
        m_pathQueue.pop();                 // Xóa khỏi hàng đợi
        m_isMoving = true;                 // Bắt đầu di chuyển
    }
}

void Mummy::update(float dt) {
    // 1. Nếu đang trong thời gian "Khựng lại" (Pause)
    if (m_pauseTimer > 0.f) {
        m_pauseTimer -= dt;
        return; // Đứng yên, chưa làm gì cả
    }

    // 2. Nếu đang đứng yên NHƯNG trong hàng đợi vẫn còn bước đi (Bước 2)
    // -> Lấy tiếp ra để đi
    if (!m_isMoving && !m_pathQueue.empty()) {
        m_targetPos = m_pathQueue.front();
        m_pathQueue.pop();
        m_isMoving = true;
    }

    // 3. Logic di chuyển (Interpolation)
    if (m_isMoving) {
        sf::Vector2f direction = m_targetPos - m_position;
        float distance = std::sqrt(direction.x * direction.x + direction.y * direction.y);

        // Tốc độ: 300.f (Bạn có thể tăng lên 400 hoặc 500 nếu muốn nó vọt lẹ hơn)
        float step = 200.f * dt;

        if (distance <= step) {
            // --- ĐÃ ĐẾN ĐÍCH CỦA BƯỚC NÀY ---
            m_position = m_targetPos;
            m_isMoving = false; // Tạm dừng di chuyển

            // Nếu vừa đi xong bước 1 (hàng đợi vẫn còn bước 2) -> Set thời gian khựng
            if (!m_pathQueue.empty()) {
                m_pauseTimer = 0.15f; // Khựng 0.15s
            }
        }
        else {
            // --- CHƯA ĐẾN ĐÍCH -> NHÍCH TIẾP ---
            sf::Vector2f moveVec = (direction / distance) * step;
            m_position += moveVec;
        }

        // Cập nhật vị trí Sprite
        if (m_sprite.has_value()) {
            m_sprite->setPosition(m_position);
        }
    }
}

void Mummy::render(sf::RenderWindow& window, float scaleRatio) {
    // Chỉnh lại tâm để vẽ chuẩn (Giống Player)
    sf::FloatRect bounds = m_sprite->getGlobalBounds();
    // Giả sử ảnh Mummy cũng cần căn giữa chân
    // Lấy kích thước Texture gốc vì GlobalBounds đã bị scale
    sf::Vector2u texSize = m_texture.getSize();

    m_sprite->setOrigin({ texSize.x / 2.0f, texSize.y - 10.0f });
    m_sprite->setScale({ scaleRatio, scaleRatio });
    m_sprite->setPosition(m_position);

    window.draw(*m_sprite);
}