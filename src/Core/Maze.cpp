#include "../../include/Core/Maze.hpp"
#include "../../include/Entities/Player.hpp"
#include "../../include/Entities/Mummy.hpp"
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

Map::~Map() {

}

void Map::loadTheme(const std::string& themeName) {
    std::string pathPrefix = "assets/textures/" + themeName + "/";

    if (!m_texWallHoriz.loadFromFile(pathPrefix + "Wall/wall_horizontal.png"))
        std::cerr << "Error loading wall H: " << themeName << "\n";
    if (!m_texWallVert.loadFromFile(pathPrefix + "Wall/wall_vertical.png"))
        std::cerr << "Error loading wall V: " << themeName << "\n";

    m_texFloors.clear();
    for (int i = 0; i < 5; ++i) {
        sf::Texture tex;
        if (tex.loadFromFile(pathPrefix + "Floor/floor_" + std::to_string(i) + ".png")) {
            m_texFloors.push_back(tex);
        }
    }

    m_texBorders.clear();
    for (int i = 0; i < 4; i++) {
        sf::Texture tex;
        if (tex.loadFromFile(pathPrefix + "Wall/border_" + std::to_string(i) + ".png")) {
            m_texBorders.push_back(tex);
        }
    }
    std::cerr << "Theme loaded: " << themeName << "\n";

    if (!m_texExit.loadFromFile("assets/textures/exit.png")) {
        std::cerr << "Error loading exit texture\n";
    }
}

void Map::loadMap(const std::string& filePath, Player& player, Mummy& mummy) {
    std::ifstream file(filePath);
    if (!file.is_open()) return;

    std::vector<std::string> lines;
    std::string line;
    while (std::getline(file, line)) lines.push_back(line);
    file.close();

    if (lines.empty()) return;

    int textH = static_cast<int>(lines.size());
    int textW = static_cast<int>(lines[0].size());

    m_height = (textH - 1) / 2;
    m_width = (textW - 1) / 2;
    m_grid.resize(m_height, std::vector<Cell>(m_width));

    for (int y = 0; y < m_height; ++y) {
        for (int x = 0; x < m_width; ++x) {
            int charX = x * 2 + 1;
            int charY = y * 2 + 1;

            Cell& cell = m_grid[y][x];

            if (charY - 1 != 0 && lines[charY - 1][charX] == '#') cell.wallTop = true;
            if (charY + 1 != textH - 1 && lines[charY + 1][charX] == '#') cell.wallBottom = true;
            if (charX - 1 != 0 && lines[charY][charX - 1] == '#') cell.wallLeft = true;
            if (charX + 1 != textW - 1 && lines[charY][charX + 1] == '#') cell.wallRight = true;

            char centerChar = lines[charY][charX];
            float spawnX = x * m_tileSize + posmap.x - dynamicOffset;
            float spawnY = y * m_tileSize + posmap.y - dynamicOffset;

            if (centerChar == 'E') {
                player.setPosition(spawnX, spawnY);
            }
            else if (centerChar == 'M') {
                mummy.setSpawn(y, x, spawnX, spawnY);
            }

            if (lines[charY - 1][charX] == 'X') cell.exitVariant = 0;
            if (lines[charY + 1][charX] == 'X') cell.exitVariant = 3;
            if (lines[charY][charX + 1] == 'X') cell.exitVariant = 2;
            if (lines[charY][charX - 1] == 'X') cell.exitVariant = 1;

            if (!m_texFloors.empty()) {
                cell.floorVariant = rand() % m_texFloors.size();
            }
        }
    }
}

void Map::draw(sf::RenderWindow& window, Player& player, Mummy& mummy) {
    int playerGridY = static_cast<int>(((player.getPosition().y - posmap.y + dynamicOffset + m_tileSize / 2) / m_tileSize));
    int mummyGridY = mummy.getR();

    auto drawSprite = [&](const sf::Texture& tex, float x, float y, float ox, float oy) {
        sf::Sprite s(tex);
        sf::FloatRect b = s.getGlobalBounds();
        s.setOrigin({ b.size.x / 2.f + ox, b.size.y / 2.f + oy });
        s.setScale({ scaleRatio, scaleRatio });
        s.setPosition({ x, y });
        window.draw(s);
        };

    for (int y = 0; y < m_height; ++y) {
        for (int x = 0; x < m_width; ++x) {
            if (m_texFloors.empty()) continue;

            float px = x * m_tileSize + posmap.x - dynamicOffset;
            float py = y * m_tileSize + posmap.y - dynamicOffset;

            sf::Sprite floor(m_texFloors[m_grid[y][x].floorVariant]);
            sf::FloatRect b = floor.getGlobalBounds();
            floor.setOrigin({ b.size.x / 2.f, b.size.y / 2.f });
            floor.setScale({ scaleRatio, scaleRatio });
            floor.setPosition({ px, py });
            window.draw(floor);
        }
    }

    if (!m_texBorders.empty()) {
        sf::Sprite borderT(m_texBorders[0]);
        sf::FloatRect b = borderT.getGlobalBounds();
        borderT.setOrigin({ b.size.x / 2.f, b.size.y });
        borderT.setPosition({ 300.f + posmap.x, -60.f + posmap.y });
        window.draw(borderT);
    }

    for (int y = 0; y < m_height; ++y) {
        for (int x = 0; x < m_width; ++x) {
            const Cell& cell = m_grid[y][x];
            float px = x * m_tileSize + posmap.x - dynamicOffset;
            float py = y * m_tileSize + posmap.y - dynamicOffset;

            if (cell.wallTop)   drawSprite(m_texWallHoriz, px, py - m_tileSize / 2, -5, 15);
            if (cell.wallLeft)  drawSprite(m_texWallVert, px - m_tileSize / 2, py, -5, 10);
            if (cell.wallRight) drawSprite(m_texWallVert, px + m_tileSize / 2, py, -5, 10);
        }

        if (y == playerGridY) {
            player.render(window, scaleRatio);
        }
        if (y == mummyGridY) {
            mummy.render(window, scaleRatio);
            std::cerr << "Mummy drawn at: " << mummy.getR() << " " << mummy.getC() << "\n";
        }

        for (int x = 0; x < m_width; ++x) {
            float px = x * m_tileSize + posmap.x - dynamicOffset;
            float py = y * m_tileSize + posmap.y - dynamicOffset;

            if (m_grid[y][x].wallBottom) {
                drawSprite(m_texWallHoriz, px, py + m_tileSize / 2, -5, 15);
            }
        }
    }

    if (m_texBorders.size() >= 4) {
        sf::Sprite borderR(m_texBorders[1]);
        sf::FloatRect bR = borderR.getGlobalBounds();
        borderR.setOrigin({ 0, bR.size.y / 2.f });
        borderR.setPosition({ 660.f + posmap.x, 300.f + posmap.y });
        window.draw(borderR);

        sf::Sprite borderB(m_texBorders[2]);
        sf::FloatRect bB = borderB.getGlobalBounds();
        borderB.setOrigin({ bB.size.x / 2.f, 0 });
        borderB.setPosition({ 300.f + posmap.x, 660.f + posmap.y });
        window.draw(borderB);

        sf::Sprite borderL(m_texBorders[3]);
        sf::FloatRect bL = borderL.getGlobalBounds();
        borderL.setOrigin({ bL.size.x, bL.size.y / 2.f });
        borderL.setPosition({ -60.f + posmap.x, 300.f + posmap.y });
        window.draw(borderL);
    }

    for (int y = 0; y < m_height; ++y) {
        for (int x = 0; x < m_width; ++x) {
            const Cell& cell = m_grid[y][x];

            if (cell.exitVariant == -1) continue;

            float px = x * m_tileSize + posmap.x - dynamicOffset;
            float py = y * m_tileSize + posmap.y - dynamicOffset;

            sf::Sprite exitSprite(m_texExit);
            sf::FloatRect b = exitSprite.getLocalBounds();

            exitSprite.setOrigin({ b.size.x / 2.f, b.size.y / 2.f });
            exitSprite.setScale({ scaleRatio, scaleRatio });
            exitSprite.setPosition({ px, py });

            float offset = m_tileSize / 2.f;

            if (cell.exitVariant == 0) {
                exitSprite.setRotation(sf::degrees(-90.f));
                exitSprite.move({ 0, -offset - 60.f * scaleRatio });
            }
            else if (cell.exitVariant == 3) {
                exitSprite.setRotation(sf::degrees(90.f));
                exitSprite.move({ 0, offset + 60.f * scaleRatio });
            }
            else if (cell.exitVariant == 1) {
                exitSprite.setRotation(sf::degrees(180.f));
                exitSprite.move({ -offset - 60.f * scaleRatio, 0 });
            }
            else if (cell.exitVariant == 2) {
                exitSprite.setRotation(sf::degrees(0.f));
                exitSprite.move({ offset + 60.f * scaleRatio, 0 });
            }

            window.draw(exitSprite);
        }
    }
}

void Map::setTileSize(float size) {
    this->m_tileSize = size;
    this->scaleRatio = size / 120.0f;
    this->dynamicOffset = 60.f - 360.f / (720.f / this->m_tileSize);
}

void Map::setPosition(float x, float y) {
    posmap = { x, y };
}

sf::Vector2f Map::getPosition() const {
    return posmap;
}

float Map::getTileSize() const {
    return m_tileSize;
}

const Cell* Map::getCell(int x, int y) const {
    if (x < 0 || x >= m_width || y < 0 || y >= m_height) {
        return nullptr;
    }
    return &m_grid[y][x];
}

int Map::getWidth() const {
    return m_width;
}

int Map::getHeight() const {
    return m_height;
}