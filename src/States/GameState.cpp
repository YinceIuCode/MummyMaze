#include "States/GameState.hpp"
#include <cmath>
#include <iostream>
#include <fstream>
#include <filesystem>

void clearSaveData() {
    std::string folderPath = "assets/mazes/";

    if (std::filesystem::exists(folderPath)) {
        for (const auto& entry : std::filesystem::directory_iterator(folderPath)) {
            if (entry.is_regular_file() && entry.path().extension() == ".txt") {
                try {
                    std::filesystem::remove(entry.path());
                    std::cout << "Deleted: " << entry.path() << "\n";
                }
                catch (const std::filesystem::filesystem_error& e) {
                    std::cerr << "Error deleting: " << e.what() << "\n";
                }
            }
        }
    }
}

GameState::GameState(sf::RenderWindow* window, std::stack<std::unique_ptr<State>>* states, std::string mapPath, bool isResuming)
    : State(window, states)
{
    // Lưu lại path để lần sau save tiếp
    m_currentMapPath = mapPath;

    // Biến tạm để chứa dữ liệu load
    int pGX = 0, pGY = 0, mGX = 0, mGY = 0;
    std::string savedMapPath = "";

    // Nếu là Resume -> Đọc file trước
    if (isResuming) {
        savedMapPath = loadGameData(pGX, pGY, mGX, mGY);
        if (!savedMapPath.empty()) {
            m_currentMapPath = savedMapPath; // Cập nhật map đúng với file save
        }
    }

    initVariables(); // Khởi tạo Player, Mummy, Map...

    // Load Map (Dùng mapPath hoặc map vừa đọc được từ file save)
    m_map.loadMap(m_currentMapPath, m_player, m_mummy);

    // --- NẾU LÀ RESUME THÌ GHI ĐÈ VỊ TRÍ ---
    if (isResuming && !savedMapPath.empty()) {
        // Tính toán lại vị trí Pixel từ Grid
        float tileSize = m_map.getTileSize();
        sf::Vector2f mapPos = m_map.getPosition();
        float offset = m_map.getDynamicOffset();

        // Công thức: Pixel = Grid * TileSize + MapPos - Offset
        float pPixelX = pGX * tileSize + mapPos.x - offset;
        float pPixelY = pGY * tileSize + mapPos.y - offset;
        m_player.setPosition(pPixelX, pPixelY);

        float mPixelX = mGX * tileSize + mapPos.x - offset;
        float mPixelY = mGY * tileSize + mapPos.y - offset;

        // Mummy setSpawn hoặc setPosition tùy hàm bạn viết
        // Lưu ý: Mummy thường lưu (Row, Col) tức là (Y, X)
        m_mummy.setSpawn(mGY, mGX, mPixelX, mPixelY);

        std::cout << "Resumed Game Successfully!\n";
    }
}

GameState::~GameState() {}

void GameState::generateNewMaze(int mapsize)
{
    generate_maze maze(mapsize);
    do
    {
        maze.generate();
    } while (maze.can_solve_the_maze() > 2); // Đảm bảo map giải được và đủ khó
    maze.print_maze();
}

void GameState::saveGame() {
    std::ofstream outFile("assets/mazes/mazesave.txt");
    if (outFile.is_open()) {
        outFile << m_currentMapPath << "\n";

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

        // 2. Tính tọa độ lưới Player và lưu
        sf::Vector2i pGrid = getPlayerGrid(); // Hàm bạn vừa viết lúc nãy
        outFile << pGrid.x << " " << pGrid.y << "\n";

        // 3. Lưu tọa độ lưới Mummy
        outFile << m_mummy.getR() << " " << m_mummy.getC() << "\n";

        std::cout << "Game Saved!\n";
        outFile.close();
    }
}

std::string GameState::loadGameData(int& pGridX, int& pGridY, int& mGridX, int& mGridY) {
    std::ifstream inFile("assets/mazes/mazesave.txt");
    std::string mapPath = "";

    if (inFile.is_open()) {
        // Đọc dòng 1: Tên Map
        std::getline(inFile, mapPath);

        // Đọc dòng 2: Player Grid X Y
        inFile >> pGridX >> pGridY;

        // Đọc dòng 3: Mummy Grid Row Col (Row là Y, Col là X)
        int mR, mC;
        inFile >> mR >> mC;
        mGridY = mR;
        mGridX = mC;

        inFile.close();
    }
    return mapPath;
}

void GameState::initVariables()
{
    // 1. Đọc file để xem Map to bao nhiêu
    std::ifstream fileCount(m_currentMapPath);
    int lineCount = 0;
    std::string tempLine;
    if (fileCount.is_open()) {
        while (std::getline(fileCount, tempLine)) {
            // Chỉ đếm dòng có dữ liệu
            if (tempLine.length() > 1) lineCount++;
        }
        fileCount.close();
    }

    if (lineCount < 3) lineCount = 13; 

    int logicalSize = (lineCount - 1) / 2; 
    m_currentMapSize = logicalSize; 

    // 2. Tính TileSize dựa trên LOGICAL SIZE
    float maxMapHeight = 720.0f;
    float tileSize = maxMapHeight / logicalSize;
    if (tileSize > 120.0f) tileSize = 120.0f;
    m_map.setTileSize(tileSize);

    // 3. Load hình ảnh (Theme)
	std::string themePath;
    if (GameData::currentTheme == 0) {
        themePath = "Playmap"; // Ví dụ đường dẫn theme 1
    }
    else if (GameData::currentTheme == 1) {
        themePath = "Nobi"; // Ví dụ đường dẫn theme 2
    }
    else {
        // Fallback mặc định nếu theme bị lỗi
        themePath = "Playmap";
    }
    m_map.loadTheme(themePath);
    m_player.loadTheme(themePath);
    m_mummy.loadTheme(themePath);

    // 4. Đặt vị trí Map ra giữa màn hình

    float realMapWidth = logicalSize * tileSize; // Ví dụ 6 * 120 = 720
    float offsetX = (1290.f - realMapWidth) / 2.f;
    float offsetY = (720.f - realMapWidth) / 2.f + 60.f;
    m_map.setPosition(1290.f / 2.f - 300.f, 210.f);

    // 5. Load Map & Sinh nhân vật
    m_map.loadMap(m_currentMapPath, m_player, m_mummy);

    time_machine.push_state(m_player, m_mummy);

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
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::P)) { // Ví dụ phím P để Pause/Save
        saveGame();
        sf::sleep(sf::milliseconds(200));
        m_states->pop();
        return;
    }

    bool isPressingKey = false;
    // 3. MÁY TRẠNG THÁI (TURN MACHINE) - Logic theo lượt

    auto getPlayerGrid = [&]() -> sf::Vector2i {
        float tileSize = m_map.getTileSize();
        sf::Vector2f mapPos = m_map.getPosition();
        sf::Vector2f pPos = m_player.getPosition();
        float offset = m_map.getDynamicOffset();

        // Tính tọa độ tương đối
        float relativeX = pPos.x - mapPos.x + offset + tileSize / 2;
        float relativeY = pPos.y - mapPos.y + offset + tileSize / 2;

        int c = static_cast<int>(std::floor(relativeX / tileSize));
        int r = static_cast<int>(std::floor(relativeY / tileSize));

        return { c, r };
        };

    switch (m_turn)
    {
        // --- GIAI ĐOẠN 1: CHỜ NGƯỜI CHƠI BẤM PHÍM ---
    case TurnState::PlayerInput:
    {
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::U)) {
            auto old_state = time_machine.undo_state(m_player, m_mummy);
            m_player = old_state.first;
            m_mummy = old_state.second;

            sf::sleep(sf::milliseconds(200));
            return;
        }
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::I)) {
            auto future_state = time_machine.redo_state();
            m_player = future_state.first;
            m_mummy = future_state.second;
            sf::sleep(sf::milliseconds(200));
            return;
        }
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::R)) {
			m_map.loadMap("assets/mazes/maze1.txt", m_player, m_mummy);
            sf::sleep(sf::milliseconds(200));
            return;
		}

        isPressingKey = (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Left) ||
            sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Right) ||
            sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Up) ||
            sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Down) ||
            sf::Keyboard::isKeyPressed(sf::Keyboard::Key::A) ||
            sf::Keyboard::isKeyPressed(sf::Keyboard::Key::D) ||
            sf::Keyboard::isKeyPressed(sf::Keyboard::Key::W) ||
            sf::Keyboard::isKeyPressed(sf::Keyboard::Key::S));

        if (isPressingKey && !m_player.isMoving()) {
            time_machine.push_state(m_player, m_mummy);
            m_player.processInput(m_map);
            if (m_player.isMoving())
            {
                m_turn = TurnState::PlayerMoving;
            }
            else {
                time_machine.undo_state(m_player, m_mummy);
            }
        }
        break;
    }
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
                clearSaveData();
                sf::sleep(sf::milliseconds(500));
                m_states->pop(); // Quay về Menu
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
                clearSaveData();
                sf::sleep(sf::milliseconds(500));
                m_states->pop();
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
				clearSaveData();
                sf::sleep(sf::milliseconds(500));
                m_states->pop(); // Quay về Menu
                return;
            }

            m_turn = TurnState::PlayerInput;
        }
        break;
    }
}

void GameState::render(sf::RenderWindow& window)
{
    // Chỉ cần gọi hàm này.
    // Bên trong Map::draw đã có logic vẽ Player và Mummy theo chiều sâu (Z-Order)
    m_map.draw(window, m_player, m_mummy);
}