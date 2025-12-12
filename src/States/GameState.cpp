#include "States/GameState.hpp"
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
    // 1. Cập nhật Animation & Di chuyển mượt (Luôn chạy)
    m_player.update(dt);
    m_mummy.update(dt);

    // 2. Xử lý thoát game (Chỉ cần check 1 lần)
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Escape)) {
        m_states->pop();
        // Không cần sleep ở đây nếu pop xử lý tốt, nhưng giữ lại nếu bạn muốn delay nhẹ
        sf::sleep(sf::milliseconds(200));
        return;
    }

    // 3. MÁY TRẠNG THÁI (TURN MACHINE) - Logic theo lượt
    switch (m_turn) {

        // --- GIAI ĐOẠN 1: CHỜ NGƯỜI CHƠI BẤM PHÍM ---
    case TurnState::PlayerInput:
        m_player.processInput(m_map);

        if (m_player.isMoving()) {
            m_turn = TurnState::PlayerMoving;
        }
        break;

        // --- GIAI ĐOẠN 2: CHỜ NGƯỜI CHƠI ĐI XONG ---
    case TurnState::PlayerMoving:
        if (!m_player.isMoving()) {
            // Player đến nơi -> Chuyển sang lượt Mummy tính toán
            m_turn = TurnState::MummyThinking;
        }
        break;

        // --- GIAI ĐOẠN 3: MUMMY TÍNH ĐƯỜNG ---
    case TurnState::MummyThinking:
        // Mummy tìm đường (sẽ tự set m_isMoving = true nếu tìm được)
        m_mummy.move(m_map, m_player);

        if (m_mummy.isMoving()) {
            m_turn = TurnState::MummyMoving;
        }
        else {
            // Mummy bị kẹt hoặc không cần đi -> Trả lượt ngay
            m_turn = TurnState::PlayerInput;
        }
        break;

        // --- GIAI ĐOẠN 4: CHỜ MUMMY ĐI XONG ---
    case TurnState::MummyMoving:
        if (!m_mummy.isMoving()) {
            // Mummy đến nơi -> Trả lượt cho người chơi
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