#include "States/GameState.hpp"
#include <cmath>
#include <iostream>
#include <fstream>
#include <filesystem>

void clearSaveData() {
    std::string filePath = "assets/mazes/mazesave.txt";

    try {
        // Kiểm tra xem file có tồn tại không
        if (std::filesystem::exists(filePath)) {
            // Hàm remove trả về true nếu xóa thành công
            if (std::filesystem::remove(filePath)) {
                std::cout << "Deleted save file successfully: " << filePath << "\n";
            }
            else {
                std::cerr << "Failed to delete file.\n";
            }
        }
        else {
            // Không có file để xóa (trường hợp chưa save bao giờ)
            std::cout << "No save file found to delete.\n";
        }
    }
    catch (const std::filesystem::filesystem_error& e) {
        std::cerr << "Error handling file: " << e.what() << "\n";
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
    } while (maze.can_solve_the_maze() > 2);
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
    // Load Texture
    if (!m_txUndo.loadFromFile("assets/textures/Menu/btn_undo.png")) std::cerr << "Err Undo\n";
    if (!m_txRedo.loadFromFile("assets/textures/Menu/btn_redo.png")) std::cerr << "Err Redo\n";
    if (!m_txReset.loadFromFile("assets/textures/Menu/btn_reset.png")) std::cerr << "Err Reset\n";
    if (!m_txBack.loadFromFile("assets/textures/Menu/btn_back.png")) std::cerr << "Err Back\n";
    if (!m_txSave.loadFromFile("assets/textures/Menu/btn_save.png")) std::cerr << "Err Save\n";
	if (!m_txYes.loadFromFile("assets/textures/Menu/icon_tick.png")) std::cerr << "Err Yes\n";
	if (!m_txNo.loadFromFile("assets/textures/Menu/icon_cross.png")) std::cerr << "Err No\n";
    if (!m_txNext.loadFromFile("assets/textures/Menu/btn_next.png")) std::cerr << "Err Next\n";

    // --- CÀI ĐẶT VỊ TRÍ NÚT (DÀN HÀNG NGANG Ở DƯỚI) ---
    float startX = 100.f;  // Bắt đầu từ bên trái
    float startY = 200.f;  // Độ cao: Thấp xuống dưới chân màn hình
    float gap = 120.f;     // Khoảng cách giữa các nút
    float scale = 2.0f;    // Scale nhỏ lại (1.0) cho vừa vặn, không dùng 3.0

    // Khởi tạo Button bằng make_unique (Khớp với constructor file cũ của bạn)
    m_btnUndo = std::make_unique<Button>(m_txUndo, startX, startY, scale);
    m_btnReset = std::make_unique<Button>(m_txReset, startX, startY + gap, scale);
    m_btnSave = std::make_unique<Button>(m_txSave, startX, startY + gap * 2, scale);
    m_btnRedo = std::make_unique<Button>(m_txRedo, startX, startY + gap * 3, scale);
    m_btnBack = std::make_unique<Button>(m_txBack, 1220.f, 50.f, scale);

    std::ifstream fileCount(m_currentMapPath);
    int lineCount = 0;
    std::string tempLine;
    if (fileCount.is_open()) {
        while (std::getline(fileCount, tempLine)) if (tempLine.length() > 1) lineCount++;
        fileCount.close();
    }
    if (lineCount < 3) lineCount = 13;
    int logicalSize = (lineCount - 1) / 2;
    m_currentMapSize = logicalSize;

    float maxMapHeight = 720.0f;
    float tileSize = maxMapHeight / logicalSize;
    if (tileSize > 120.0f) tileSize = 120.0f;
    m_map.setTileSize(tileSize);

    std::string themePath = (GameData::currentTheme == 1) ? "Nobi" : "Playmap";
    m_map.loadTheme(themePath);
    m_player.loadTheme(themePath);
    m_mummy.loadTheme(themePath);

    float realMapWidth = logicalSize * tileSize;
    m_map.setPosition(1290.f / 2.f - 300.f, 210.f);

    m_map.loadMap(m_currentMapPath, m_player, m_mummy);
    time_machine.push_state(m_player, m_mummy);
    m_turn = TurnState::PlayerInput;

    // --- THÊM PHẦN KHỞI TẠO POPUP ---
    m_showExitConfirm = false;

    // 1. Lớp đen mờ (Dim background)
    m_darkLayer.setSize(sf::Vector2f(m_window->getSize().x, m_window->getSize().y)); // Kích thước bằng cửa sổ game
    m_darkLayer.setFillColor(sf::Color(0, 0, 0, 150)); // Màu đen, Alpha 150 (trong suốt)

    // 2. Bảng thông báo (Nền)
    m_popupPanel.setSize(sf::Vector2f(800.f, 300.f));
    m_popupPanel.setFillColor(sf::Color(50, 40, 30)); // Màu nâu đất
    m_popupPanel.setOutlineThickness(4.f);
    m_popupPanel.setOutlineColor(sf::Color(200, 180, 50)); // Viền vàng
    m_popupPanel.setOrigin({ 400.f, 150.f });
    m_popupPanel.setPosition({ m_window->getSize().x / 2.f, m_window->getSize().y / 2.f}); // Căn giữa màn hình
    static sf::Font font;
    if (!font.openFromFile("assets/fonts/Akashi.ttf")) {
		std::cerr << "Error loading font for popup text!\n";
    }

    m_popupText.emplace(font, "Save Game Before Exit?", 50);
    m_popupText->setFillColor(sf::Color::White);

    sf::FloatRect textRect = m_popupText->getLocalBounds();
    m_popupText->setOrigin({ textRect.size.x / 2.0f, textRect.size.y / 2.0f });
    m_popupText->setPosition({ m_window->getSize().x / 2.f, m_window->getSize().y / 2.f - 50.f }); // Cao hơn tâm một chút

    // 4. Hai nút Yes / No

    float centerX = m_window->getSize().x / 2.f;
    float centerY = m_window->getSize().y / 2.f;

    // Nút YES (Bên trái) - Tạm dùng Texture nút Play (m_txUndo cũ của bạn hình như là Play)
    m_btnYes = std::make_unique<Button>(m_txYes, centerX - 100.f, centerY + 60.f, 1.5f);

    // Nút NO (Bên phải) - Tạm dùng Texture nút Exit (m_txSave)
    m_btnNo = std::make_unique<Button>(m_txNo, centerX + 100.f, centerY + 60.f, 1.5f);

    m_endGameStatus = 0; // Mặc định là đang chơi (0)

    // 1. Cài đặt chữ Win/Lose
    m_endGameText.emplace(font, "", 100);
    m_endGameText->setStyle(sf::Text::Bold);
    m_endGameText->setOutlineThickness(3.f);
    m_endGameText->setOutlineColor(sf::Color::Black);

    // 2. Khởi tạo nút
    float cx = m_window->getSize().x / 2.f;
    float cy = m_window->getSize().y / 2.f;

    // Nút RETRY (Chơi lại màn này) - Dùng icon Reset
    m_btnRetry = std::make_unique<Button>(m_txReset, cx - 80.f, cy + 80.f, 1.5f);

    // Nút MENU (Về sảnh chính) - Dùng icon Back/Exit
    m_btnMenuEnd = std::make_unique<Button>(m_txBack, cx, cy + 80.f, 1.5f);

	m_btnNext = std::make_unique<Button>(m_txNext, cx + 80.f, cy + 80.f, 1.5f);
}

void GameState::update(float dt) {
    m_totalTime += dt;
    sf::Vector2f mousePos = m_window->mapPixelToCoords(sf::Mouse::getPosition(*m_window));
    bool isMousePressed = sf::Mouse::isButtonPressed(sf::Mouse::Button::Left);

    // Chặn click thừa
    if (isMousePressed && m_isWaitingForMouseRelease) return;
    if (!isMousePressed) m_isWaitingForMouseRelease = false;

    if (m_endGameStatus != 0) {
        // Nếu thắng hoặc thua -> Chỉ update 2 nút Retry và Menu
        m_btnRetry->update(mousePos, isMousePressed, m_totalTime);
        m_btnMenuEnd->update(mousePos, isMousePressed, m_totalTime);
		m_btnNext->update(mousePos, isMousePressed, m_totalTime);

        // -- Xử lý bấm RETRY --
        if (m_btnRetry->isClicked()) {
            // Reset lại map hiện tại
            m_map.loadMap(m_currentMapPath, m_player, m_mummy);
            clearSaveData(); // Xóa file save cũ cho công bằng
            m_endGameStatus = 0; // Trở về trạng thái đang chơi
            m_turn = TurnState::PlayerInput; // Reset lượt
            sf::sleep(sf::milliseconds(200));
        }

        // -- Xử lý bấm MENU --
        if (m_btnMenuEnd->isClicked()) {
            clearSaveData();
            m_states->pop();
            return;
        }

        if (m_btnNext->isClicked()) {
            generate_maze gen(m_currentMapSize);
            gen.generate();
            gen.print_maze();

            auto newGameState = std::make_unique<GameState>(m_window, m_states, "assets/mazes/maze1.txt", false);
            auto statesStack = m_states;
            statesStack->pop();

            statesStack->push(std::move(newGameState));
        }
        return; // <--- DỪNG UPDATE GAME
    }

    // --- LOGIC MỚI: KIỂM TRA POPUP ---
    if (m_showExitConfirm) {

        m_btnYes->update(mousePos, isMousePressed, m_totalTime);
        m_btnNo->update(mousePos, isMousePressed, m_totalTime);

        // Xử lý click YES -> Lưu -> Thoát
        if (m_btnYes->isClicked()) {
            saveGame();
            sf::sleep(sf::milliseconds(200));
            m_states->pop(); // Thoát ra menu
            return;
        }

        // Xử lý click NO -> Không lưu -> Thoát luôn
        if (m_btnNo->isClicked()) {
            sf::sleep(sf::milliseconds(200));
            m_states->pop(); // Thoát ra menu
            return;
        }

        // Xử lý nếu bấm ESC lần nữa -> Hủy thoát (Đóng popup)
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Escape)) {
            m_showExitConfirm = false; // Tắt bảng đi, chơi tiếp
            m_isWaitingForMouseRelease = true; // Chặn click nhầm
            sf::sleep(sf::milliseconds(200));
        }

        return; // <--- RETURN NGAY, KHÔNG CHẠY LOGIC GAME BÊN DƯỚI
    }

    m_player.update(dt);
    m_mummy.update(dt);

    // --- GỌI HÀM UPDATE NÚT ---
    if (m_btnUndo)  m_btnUndo->update(mousePos, isMousePressed, m_totalTime);
    if (m_btnRedo)  m_btnRedo->update(mousePos, isMousePressed, m_totalTime);
    if (m_btnReset) m_btnReset->update(mousePos, isMousePressed, m_totalTime);
    if (m_btnSave)  m_btnSave->update(mousePos, isMousePressed, m_totalTime);
    if (m_btnBack)  m_btnBack->update(mousePos, isMousePressed, m_totalTime);

    bool isPressingKey = false;

    // Helper lambda lấy tọa độ grid
    auto getPlayerGrid = [&]() -> sf::Vector2i {
        float tileSize = m_map.getTileSize();
        sf::Vector2f mapPos = m_map.getPosition();
        sf::Vector2f pPos = m_player.getPosition();
        float offset = m_map.getDynamicOffset();
        float relativeX = pPos.x - mapPos.x + offset + tileSize / 2;
        float relativeY = pPos.y - mapPos.y + offset + tileSize / 2;
        int c = static_cast<int>(std::floor(relativeX / tileSize));
        int r = static_cast<int>(std::floor(relativeY / tileSize));
        return { c, r };
        };

    switch (m_turn)
    {
    case TurnState::PlayerInput:
    {
        // --- XỬ LÝ CLICK NÚT ---
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::U) || (m_btnUndo && m_btnUndo->isClicked())) {
            auto old_state = time_machine.undo_state(m_player, m_mummy);
            m_player = old_state.first;
            m_mummy = old_state.second;
            sf::sleep(sf::milliseconds(200));
            return;
        }
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::I) || (m_btnRedo && m_btnRedo->isClicked())) {
            auto future_state = time_machine.redo_state();
            m_player = future_state.first;
            m_mummy = future_state.second;
            sf::sleep(sf::milliseconds(200));
            return;
        }
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::R) || (m_btnReset && m_btnReset->isClicked())) {
            m_map.loadMap(m_currentMapPath, m_player, m_mummy);
            sf::sleep(sf::milliseconds(200));
            return;
        }
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::P) || (m_btnSave && m_btnSave->isClicked())) {
            saveGame();
            sf::sleep(sf::milliseconds(200));
            return;
        }
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Escape) || (m_btnBack && m_btnBack->isClicked())) {
            m_showExitConfirm = true;
            m_isWaitingForMouseRelease = true; 
            return;
        }

        // Logic di chuyển
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
            if (m_player.isMoving()) m_turn = TurnState::PlayerMoving;
            else time_machine.undo_state(m_player, m_mummy);
        }
        break;
    }
    case TurnState::PlayerMoving:
        if (!m_player.isMoving()) {
            sf::Vector2i pGrid = getPlayerGrid();
            int mapW = m_map.getWidth();
            int mapH = m_map.getHeight();

            // --- LOGIC THẮNG ---
            // Nếu Player ra khỏi biên -> WIN
            if (pGrid.x < 0 || pGrid.x >= mapW || pGrid.y < 0 || pGrid.y >= mapH) {
                std::cout << ">>> VICTORY! <<<\n"; // Debug chơi thôi

                // CẬP NHẬT TRẠNG THÁI WIN
                m_endGameStatus = 1;
                m_endGameText->setString("VICTORY!");
                m_endGameText->setFillColor(sf::Color::Yellow);

                // Căn giữa chữ
                sf::FloatRect b = m_endGameText->getLocalBounds();
                m_endGameText->setOrigin({ b.size.x / 2.f, b.size.y / 2.f });
                m_endGameText->setPosition({ m_window->getSize().x / 2.f, m_window->getSize().y / 2.f - 50.f });

                clearSaveData(); // Thắng rồi thì xóa save
                return;
            }
            m_turn = TurnState::MummyThinking;
        }
        break;

    case TurnState::MummyThinking:
        m_mummy.move(m_map, m_player); // 1. Tính toán nước đi

        // 2. NẾU MUMMY BẮT ĐẦU DI CHUYỂN -> CHUYỂN TRẠNG THÁI NGAY
        if (m_mummy.isMoving()) {
            m_turn = TurnState::MummyMoving; // <--- THÊM DÒNG NÀY ĐỂ NÓ KHÔNG TÍNH TOÁN NỮA
        }
        else {
            // 3. Nếu Mummy không cần đi (đứng im hoặc bị kẹt)
            sf::Vector2i pGrid = getPlayerGrid();

            // Check xem có bắt được Player ngay tại chỗ không
            if (m_mummy.getR() == pGrid.y && m_mummy.getC() == pGrid.x) {
                // --- LOGIC THUA (Copy từ code cũ) ---
                m_endGameStatus = 2;
                m_endGameText->setString("DEFEAT!");
                m_endGameText->setFillColor(sf::Color::Red);

                sf::FloatRect b = m_endGameText->getLocalBounds();
                m_endGameText->setOrigin({ b.size.x / 2.f, b.size.y / 2.f });
                m_endGameText->setPosition({ m_window->getSize().x / 2.f, m_window->getSize().y / 2.f - 50.f });

                clearSaveData();
                return;
            }

            // Nếu không bắt được -> Hết lượt, chuyển về lượt Player
            m_turn = TurnState::PlayerInput;
        }
        break;

    case TurnState::MummyMoving:
        if (!m_mummy.isMoving()) {
            sf::Vector2i pGrid = getPlayerGrid();
            // --- LOGIC THUA (2) ---
            if (m_mummy.getR() == pGrid.y && m_mummy.getC() == pGrid.x) {
                m_endGameStatus = 2;
                m_endGameText->setString("YOU DIED!");
                m_endGameText->setFillColor(sf::Color::Red);

                sf::FloatRect b = m_endGameText->getLocalBounds();
                m_endGameText->setOrigin({b.size.x / 2.f, b.size.y / 2.f });
                m_endGameText->setPosition({ m_window->getSize().x / 2.f, m_window->getSize().y / 2.f - 50.f });

                clearSaveData();
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
    if (m_btnUndo)  m_btnUndo->render(window);
    if (m_btnRedo)  m_btnRedo->render(window);
    if (m_btnReset) m_btnReset->render(window);
    if (m_btnSave)  m_btnSave->render(window);
    if (m_btnBack)  m_btnBack->render(window);
    if (m_showExitConfirm) {
        window.draw(m_darkLayer);  // Làm tối màn hình game
        window.draw(m_popupPanel); // Vẽ cái bảng nền
        window.draw(*m_popupText);  // Vẽ chữ hỏi

        m_btnYes->render(window);  // Vẽ nút Yes
        m_btnNo->render(window);   // Vẽ nút No
    }
    if (m_endGameStatus != 0) {
        // Tái sử dụng lớp đen mờ và bảng popup cho đẹp
        window.draw(m_darkLayer);
        window.draw(m_popupPanel);

        window.draw(*m_endGameText); // Chữ VICTORY hoặc DEFEAT

        m_btnRetry->render(window);
        m_btnMenuEnd->render(window);
		m_btnNext->render(window);
    }
}
