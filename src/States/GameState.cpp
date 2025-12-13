#include "States/GameState.hpp"
#include <cmath>
#include <iostream>

GameState::GameState(sf::RenderWindow* window, std::stack<std::unique_ptr<State>>* states, int mapSize)
    : State(window, states)
{
    this->m_currentMapSize = mapSize;
    this->initVariables();
}

GameState::~GameState() {}

void GameState::generateNewMaze(int mapsize) {
    generate_maze maze(mapsize);
    do {
        maze.generate();
    } while (maze.can_solve_the_maze() > 2); // Đảm bảo map giải được và đủ khó
    maze.print_maze();
}

void GameState::initVariables() {
    // 1. Tạo mê cung mới
    this->generateNewMaze(m_currentMapSize);

    // 2. Tính toán kích thước ô gạch (TileSize)
    // Map luôn cao tối đa 720px để vừa màn hình
    float maxMapHeight = 720.0f;
    float tileSize = maxMapHeight / m_currentMapSize;
    m_isWin = false;
	m_isDefeat = false;

    // Giới hạn không cho ô quá to (nếu map 6x6)
    if (tileSize > 120.0f) tileSize = 120.0f;

    m_map.setTileSize(tileSize);

    // 3. Load hình ảnh (Theme)
    m_map.loadTheme(GameData::currentTheme);
    m_player.loadTheme(GameData::currentTheme);
    m_mummy.loadTheme(GameData::currentTheme);

    // 4. Đặt vị trí Map ra giữa màn hình TRƯỚC
    // (Logic tính toán của bạn: 1290/2 - 300)
    float offsetX = 1290.f / 2.f - 300.f;
    float offsetY = 210.f;
    m_map.setPosition(offsetX, offsetY);

    // 5. Load Map & Sinh nhân vật
    // Hàm này sẽ dùng vị trí Map vừa set ở trên để đặt Player/Mummy chuẩn luôn
    m_map.loadMap("assets/mazes/maze1.txt", m_player, m_mummy);

    // 6. Bắt đầu lượt chơi
    m_turn = TurnState::PlayerInput;
}

void GameState::update(float dt) {
    m_player.update(dt);
    m_mummy.update(dt);

    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Escape)) {
        m_states->pop();
        sf::sleep(sf::milliseconds(200));
        return;
    }

    // Helper: Lấy tọa độ Grid hiện tại của Player
    auto getPlayerGrid = [&]() -> sf::Vector2i {
        float tileSize = m_map.getTileSize();
        sf::Vector2f mapPos = m_map.getPosition();
        sf::Vector2f pPos = m_player.getPosition();
        float offset = m_map.getDynamicOffset();

        // Tính tọa độ tương đối
        float relativeX = pPos.x - mapPos.x + offset + tileSize / 2;
        float relativeY = pPos.y - mapPos.y + offset + tileSize / 2;

        // DÙNG std::floor ĐỂ LÀM TRÒN XUỐNG (Xử lý số âm chuẩn)
        // floor(-0.4) = -1.0 -> cast sang int thành -1
        int c = static_cast<int>(std::floor(relativeX / tileSize));
        int r = static_cast<int>(std::floor(relativeY / tileSize));

        return { c, r };
        };

    switch (m_turn) {
    case TurnState::PlayerInput:
        m_player.processInput(m_map);
        if (m_player.isMoving()) {
            m_turn = TurnState::PlayerMoving;
        }
        break;

    case TurnState::PlayerMoving:
        if (!m_player.isMoving()) {
            sf::Vector2i pGrid = getPlayerGrid();

            // --- KIỂM TRA THẮNG (Player ra ngoài map) ---
            int mapW = m_map.getWidth();
            int mapH = m_map.getHeight();
            std::cerr << mapW << " " << mapH << "\n";
            std::cerr << pGrid.x << " " << pGrid.y << "\n";

            // Nếu tọa độ nhỏ hơn 0 hoặc lớn hơn kích thước map -> Đã thoát thành công
            if (pGrid.x < 0 || pGrid.x >= mapW || pGrid.y < 0 || pGrid.y >= mapH) {
                std::cout << ">>> VICTORY! ESCAPED! <<<\n";
                sf::sleep(sf::milliseconds(500));
                m_states->pop(); // Quay về Menu
                m_isWin = true;
                return;
            }

            // Nếu chưa thắng thì chuyển lượt cho Mummy
            m_turn = TurnState::MummyThinking;
        }
        break;

    case TurnState::MummyThinking:
        m_mummy.move(m_map, m_player);
        // Logic kiểm tra nếu Mummy bắt được ngay khi tính toán (ít xảy ra)
        if (m_mummy.isMoving()) {
            m_turn = TurnState::MummyMoving;
        }
        else {
            // Mummy đứng yên, check va chạm luôn
            sf::Vector2i pGrid = getPlayerGrid();
            if (m_mummy.getR() == pGrid.y && m_mummy.getC() == pGrid.x) {
                std::cout << ">>> DEFEAT! <<<\n";
                sf::sleep(sf::milliseconds(500));
                m_states->pop();
				m_isDefeat = true;
                return;
            }
            m_turn = TurnState::PlayerInput;
        }
        break;

    case TurnState::MummyMoving:
        if (!m_mummy.isMoving()) {
            // --- KIỂM TRA THUA (Mummy bắt được) ---
            sf::Vector2i pGrid = getPlayerGrid();

            // So sánh tọa độ Mummy (R, C) với Player (Y, X)
            if (m_mummy.getR() == pGrid.y && m_mummy.getC() == pGrid.x) {
                std::cout << ">>> DEFEAT! MUMMY CAUGHT YOU <<<\n";
                sf::sleep(sf::milliseconds(500));
                m_states->pop(); // Quay về Menu
				m_isDefeat = true;
                return;
            }

            m_turn = TurnState::PlayerInput;
        }
        break;
    }
}

void GameState::render(sf::RenderWindow& window) {
    // Chỉ cần gọi hàm này. 
    // Bên trong Map::draw đã có logic vẽ Player và Mummy theo chiều sâu (Z-Order)
    m_map.draw(window, m_player, m_mummy);
}