#include <Core/Maze.hpp>
#include <Entities/Player.hpp>
#include <fstream>
#include <iostream>
#include <cstdlib>

Map::Map() {
	this->m_tileSize = 120.0f;
    this->scaleRatio = this->m_tileSize / 120.0f;
    this->dynamicOffset = 60.f - 360.f / (720.f / this->m_tileSize);
    this->posmap = { 1290 / 2.f - 300.f, 210.f };

    this->loadTheme("Playmap");
}

void Map::loadTheme(const std::string& themeName) {
    // 1. Tạo đường dẫn gốc: "assets/textures/Nobita/"
    std::string pathPrefix = "assets/textures/" + themeName + "/";

    // 2. Load Tường (Cộng chuỗi để ra đường dẫn đầy đủ)
    if (!m_texWallHoriz.loadFromFile(pathPrefix + "Wall/wall_horizontal.png")) {
        std::cerr << "Error loading wall H for theme: " << themeName << "\n";
    }
    if (!m_texWallVert.loadFromFile(pathPrefix + "Wall/wall_vertical.png")) {
        std::cerr << "Error loading wall V for theme: " << themeName << "\n";
    }

    // 3. Load Sàn (Xóa dữ liệu cũ đi)
    m_texFloors.clear(); // Xóa sạch texture của theme cũ

    for (int i = 0; i < 5; ++i) {
        sf::Texture tex;
        std::string floorPath = pathPrefix + "Floor/floor_" + std::to_string(i) + ".png";

        if (tex.loadFromFile(floorPath)) {
            m_texFloors.push_back(tex);
        }
    }

    // 4. Load Khung tường
    m_texBorders.clear();
    
    for (int i = 0; i < 4; i++) {
        sf::Texture tex;
        std::string borderPath = pathPrefix + "Wall/border_" + std::to_string(i) + ".png";

        if (tex.loadFromFile(borderPath)) {
            m_texBorders.push_back(tex);
        }
    }

    std::cerr << "Theme loaded: " << themeName << "\n";
}

Map::~Map() {

}

float Map::getTileSize() const {
    return m_tileSize;
}


const Cell* Map::getCell(int x, int y) const {
    // Kiểm tra an toàn: Nếu tọa độ nằm ngoài bản đồ thì trả về nullptr
    if (x < 0 || x >= m_width || y < 0 || y >= m_height) {
        return nullptr;
    }
    // Trả về địa chỉ của ô đó trong bộ nhớ
    return &m_grid[y][x];
}

int Map::getWidth() const {
    return m_width;
}

int Map::getHeight() const {
    return m_height;
}

void Map::loadMap(const std::string& filePath) {
    std::ifstream file(filePath);
    if (!file.is_open()) return;

    std::vector<std::string> lines;
    std::string line;

    // Đọc file
    while (std::getline(file, line)) {
        lines.push_back(line);
    }
    file.close();

    if (lines.empty()) return;

    int textH = static_cast<int>(lines.size());
    int textW = static_cast<int>(lines[0].size());

    m_height = (textH - 1) / 2;
    m_width = (textW - 1) / 2;

    m_grid.resize(m_height, std::vector<Cell>(m_width));

    for (int y = 0; y < m_height; ++y) {
        for (int x = 0; x < m_width; ++x) {
            // Tính tọa độ trong file text
            int charX = x * 2 + 1;
            int charY = y * 2 + 1;

            Cell& cell = m_grid[y][x];

            char topChar = lines[charY - 1][charX];
            if (topChar == '#' && charY - 1 != 0) cell.wallTop = true;

            char botChar = lines[charY + 1][charX];
            if (botChar == '#' && charY + 1 != textH - 1) cell.wallBottom = true;

            char leftChar = lines[charY][charX - 1];
            if (leftChar == '#' && charX - 1 != 0) cell.wallLeft = true;

            char rightChar = lines[charY][charX + 1];
            if (rightChar == '#' && charX + 1 != textW - 1) cell.wallRight = true;

            char centerChar = lines[charY][charX];
            
            if (!m_texFloors.empty()) {
                cell.floorVariant = rand() % m_texFloors.size();
            }
        }
    }
}

void Map::draw(sf::RenderWindow& window, Player& player) {
    int playerGridY = static_cast<int>((player.getPosition().y / m_tileSize));

    for (int y = 0; y < m_height; ++y) {
        for (int x = 0; x < m_width; ++x) {
            const Cell& cell = m_grid[y][x];

            float px = x * m_tileSize + posmap.x - dynamicOffset;
            float py = y * m_tileSize + posmap.y - dynamicOffset;

            if (!m_texFloors.empty()) {
                sf::Sprite m_sprite(m_texFloors[cell.floorVariant]);
                sf::FloatRect m_rect = m_sprite.getGlobalBounds();
                m_sprite.setOrigin({ m_rect.size.x / 2.0f, m_rect.size.y / 2.0f });
                m_sprite.setScale({ scaleRatio, scaleRatio });
                m_sprite.setRotation(sf::degrees(0)); // Reset xoay
                m_sprite.setPosition({ px, py });
                window.draw(m_sprite);
            }
        }
    }

    sf::Sprite m_spritet(m_texBorders[0]);
    sf::FloatRect m_rectt = m_spritet.getGlobalBounds();
    m_spritet.setOrigin({ m_rectt.size.x / 2.0f, m_rectt.size.y });
    m_spritet.setPosition({ 300.f + posmap.x, -60.f + posmap.y });
    window.draw(m_spritet);

    for (int y = 0; y < m_height; ++y) {
        for (int x = 0; x < m_width; ++x) {
            const Cell& cell = m_grid[y][x];

            float px = x * m_tileSize + posmap.x - dynamicOffset;
            float py = y * m_tileSize + posmap.y - dynamicOffset;
            
            if (cell.wallTop) {
                sf::Sprite m_sprite(m_texWallHoriz);
                sf::FloatRect m_rect = m_sprite.getGlobalBounds();
                m_sprite.setOrigin({ m_rect.size.x / 2.0f - 5, m_rect.size.y / 2.0f + 15 });
                m_sprite.setScale({ scaleRatio, scaleRatio });
                m_sprite.setPosition({ px, py - m_tileSize / 2 });
                window.draw(m_sprite);
            }

            // Vẽ tường trái (Left)
            if (cell.wallLeft) {
                sf::Sprite m_sprite(m_texWallVert);
                sf::FloatRect m_rect = m_sprite.getGlobalBounds();
                m_sprite.setOrigin({ m_rect.size.x / 2.0f - 5, m_rect.size.y / 2.0f + 10 });
                m_sprite.setScale({ scaleRatio, scaleRatio });
                m_sprite.setPosition({ px - m_tileSize / 2, py });
                window.draw(m_sprite);
            }
            
            // Vẽ tường phải (Right) - Chỉ cần vẽ ở cột cuối cùng
            if (cell.wallRight) {
                sf::Sprite m_sprite(m_texWallVert);
                sf::FloatRect m_rect = m_sprite.getGlobalBounds();
                m_sprite.setOrigin({ m_rect.size.x / 2.0f - 5, m_rect.size.y / 2.0f + 10 });
                m_sprite.setScale({ scaleRatio, scaleRatio });
                m_sprite.setPosition({ px + m_tileSize / 2, py });
                window.draw(m_sprite);
            }
        }

        if (y == playerGridY) {
            player.render(window);
        }

        for (int x = 0; x < m_width; ++x) {
            const Cell& cell = m_grid[y][x];

            float px = x * m_tileSize + posmap.x - dynamicOffset;
            float py = y * m_tileSize + posmap.y - dynamicOffset;

            // Vẽ tường dưới (Bottom) - Chỉ cần vẽ ở hàng cuối cùng của map
            if (cell.wallBottom) {
                sf::Sprite m_sprite(m_texWallHoriz);
                sf::FloatRect m_rect = m_sprite.getGlobalBounds();
                m_sprite.setOrigin({ m_rect.size.x / 2.0f - 5, m_rect.size.y / 2.0f + 15 });
                m_sprite.setScale({ scaleRatio, scaleRatio });
                m_sprite.setPosition({ px, py + m_tileSize / 2 });
                window.draw(m_sprite);
            }
        }
    }

    sf::Sprite m_spriter(m_texBorders[1]);
    sf::FloatRect m_rectr = m_spriter.getGlobalBounds();
    m_spriter.setOrigin({ 0, m_rectr.size.y / 2.0f });
    m_spriter.setPosition({ 660.f + posmap.x, 300.f + posmap.y });
    window.draw(m_spriter);

    sf::Sprite m_spriteb(m_texBorders[2]);
    sf::FloatRect m_rectb = m_spriteb.getGlobalBounds();
    m_spriteb.setOrigin({ m_rectb.size.x / 2.0f, 0 });
    m_spriteb.setPosition({ 300.f + posmap.x, 660.f + posmap.y });
    window.draw(m_spriteb);

    sf::Sprite m_spritel(m_texBorders[3]);
    sf::FloatRect m_rectl = m_spritel.getGlobalBounds();
    m_spritel.setOrigin({ m_rectl.size.x, m_rectl.size.y / 2.0f });
    m_spritel.setPosition({ -60.f + posmap.x, 300.f + posmap.y });
    window.draw(m_spritel);
}