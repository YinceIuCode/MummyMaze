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
    } while (maze.can_solve_the_maze() > 2);
    maze.print_maze();
}

void GameState::initVariables() {

    this->generateNewMaze(m_currentMapSize);

    float maxMapHeight = 720.0f;
    float tileSize = maxMapHeight / m_currentMapSize;

    m_map.setTileSize(tileSize);
	m_map.loadTheme(GameData::currentTheme);
	m_player.loadTheme(GameData::currentTheme);
    m_mummy.loadTheme(GameData::currentTheme);
    
    float offsetX = 1290 / 2.f - 300.f;
    float offsetY = 210.f;
        m_map.setPosition(offsetX, offsetY);
    
    m_map.loadMap("assets/mazes/maze1.txt", m_player, m_mummy);
    m_turn = TurnState::PlayerInput;
}

void GameState::update(float dt) {
	// Xử lý Input (Theme)
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Escape)) {
        m_states->pop();
        sf::sleep(sf::milliseconds(200));
        return; // Return ngay tránh lỗi~
    }

	// Xử lý Player
	m_player.update(dt);
    m_mummy.update(dt);
    m_player.update(dt);
    m_mummy.update(dt);

    // 2. Xử lý thoát game
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Escape)) {
        m_states->pop();
        return;
    }

    // 3. MÁY TRẠNG THÁI (TURN MACHINE)
    switch (m_turn) {

        // --- GIAI ĐOẠN 1: CHỜ NGƯỜI CHƠI BẤM PHÍM ---
    case TurnState::PlayerInput:
        // Chỉ xử lý input khi đến lượt
        m_player.processInput(m_map);

        // Nếu phát hiện người chơi bắt đầu di chuyển
        if (m_player.isMoving()) {
            m_turn = TurnState::PlayerMoving;
        }
        break;

        // --- GIAI ĐOẠN 2: CHỜ NGƯỜI CHƠI ĐI XONG ---
    case TurnState::PlayerMoving:
        // Kiểm tra xem Player đã dừng chưa
        if (!m_player.isMoving()) {
            // Player đã đến nơi -> Đến lượt Mummy tính toán
            m_turn = TurnState::MummyThinking;
        }
        break;

        // --- GIAI ĐOẠN 3: MUMMY TÍNH ĐƯỜNG ---
    case TurnState::MummyThinking:
        // Gọi hàm tìm đường (Hàm này sẽ set m_isMoving = true bên trong Mummy)
        m_mummy.move(m_map, m_player);

        // Kiểm tra xem Mummy có di chuyển không (hay bị kẹt/đứng yên)
        if (m_mummy.isMoving()) {
            m_turn = TurnState::MummyMoving;
        }
        else {
            // Nếu Mummy không cần đi (hoặc kẹt), trả lượt ngay cho người chơi
            m_turn = TurnState::PlayerInput;
        }
        break;

        // --- GIAI ĐOẠN 4: CHỜ MUMMY ĐI XONG ---
    case TurnState::MummyMoving:
        // Kiểm tra xem Mummy đã dừng chưa
        if (!m_mummy.isMoving()) {
            // Mummy đã đến nơi -> Trả lượt cho người chơi
            m_turn = TurnState::PlayerInput;
        }
        break;
    }
}

void GameState::render(sf::RenderWindow& window) {
    m_map.draw(window, m_player);
    float scaleRatio = m_map.getTileSize() / 120.0f;
    m_mummy.render(window, scaleRatio);
}