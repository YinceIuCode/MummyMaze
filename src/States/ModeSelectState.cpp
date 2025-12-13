#include "States/ModeSelectState.hpp"
#include <iostream>

ModeSelectState::ModeSelectState(sf::RenderWindow* window, std::stack<std::unique_ptr<State>>* states)
    : State(window, states),
    m_title(m_font), m_btn6x6(m_font), m_btn8x8(m_font), m_btn10x10(m_font), m_btnBack(m_font)
{
    initFonts();
    initGui();
}

ModeSelectState::~ModeSelectState() {}

void ModeSelectState::initFonts() {
    if (!m_font.openFromFile("assets/fonts/Akashi.ttf")) {
        std::cerr << "ERROR: Could not load font\n";
    }
}

void ModeSelectState::initGui() {
    // Setup cơ bản (bạn có thể căn chỉnh lại tọa độ cho đẹp)
    m_title.setString("SELECT MODE");
    m_title.setCharacterSize(100);
    sf::FloatRect m_rect = m_title.getGlobalBounds();
    m_title.setOrigin(m_rect.getCenter());
    m_title.setPosition({ m_window->getSize().x / 2.0f, 250.f});

    m_btn6x6.setString("6 x 6");
    m_btn6x6.setCharacterSize(70);
    m_rect = m_btn6x6.getGlobalBounds();
    m_btn6x6.setOrigin(m_rect.getCenter());
    m_btn6x6.setPosition({ m_window->getSize().x / 2.0f, 500.f });

    m_btn8x8.setString("8 x 8");
    m_btn8x8.setCharacterSize(70);
    m_rect = m_btn8x8.getGlobalBounds();
    m_btn8x8.setOrigin(m_rect.getCenter());
    m_btn8x8.setPosition({ m_window->getSize().x / 2.0f, 600.f });

    m_btn10x10.setString("10 x 10");
    m_btn10x10.setCharacterSize(70);
    m_rect = m_btn10x10.getGlobalBounds();
    m_btn10x10.setOrigin(m_rect.getCenter());
    m_btn10x10.setPosition({ m_window->getSize().x / 2.0f, 700.f });

    m_btnBack.setString("BACK");
    m_rect = m_btnBack.getGlobalBounds();
    m_btnBack.setOrigin(m_rect.getCenter());
    m_btnBack.setPosition({ 100.f, 50.f });

    if (!m_bgTexture.loadFromFile("assets/textures/Backgrounds/menu_bg.png")) {
		std::cerr << "Error loading background texture\n";
    }
	m_background.emplace(m_bgTexture);
}

void ModeSelectState::updateButtons() {
    sf::Vector2f mousePos = m_window->mapPixelToCoords(sf::Mouse::getPosition(*m_window));
    static bool isHandled = false;

    // Hover effect (đổi màu khi chuột chỉ vào)
    auto handleHover = [&](sf::Text& txt) {
        if (txt.getGlobalBounds().contains(mousePos)) txt.setFillColor(sf::Color::Red);
        else txt.setFillColor(sf::Color::White);
        };
    handleHover(m_btn6x6); handleHover(m_btn8x8); handleHover(m_btn10x10); handleHover(m_btnBack);

    auto createAndPlay = [&](int size) {
        // 1. Tạo mê cung & Lưu file (Giữ nguyên)
        generate_maze gen(size);
        gen.generate();
        gen.print_maze();

        // 2. CHUẨN BỊ GAME STATE (Nhưng khoan hãy push)
        auto newGameState = std::make_unique<GameState>(m_window, m_states, "assets/mazes/maze1.txt", false);

        // 3. KỸ THUẬT SWAP (TRÁO ĐỔI)
        // Mục tiêu: Xóa ModeSelect đi, thay bằng GameState
        // Stack: [Menu, Select] -> [Menu, Game]

        // QUAN TRỌNG: Lưu con trỏ m_states ra biến cục bộ
        // Vì sau lệnh pop(), đối tượng 'this' (ModeSelectState) sẽ bị hủy, biến m_states sẽ không còn truy cập được.
        auto statesStack = m_states;

        // BƯỚC A: Xóa màn hình chọn mode hiện tại
        statesStack->pop();

        // BƯỚC B: Đẩy màn hình Game vào
        statesStack->push(std::move(newGameState));
        };

    // Click effect
    if (sf::Mouse::isButtonPressed(sf::Mouse::Button::Left)) {
        if (!isHandled) {
            if (m_btn6x6.getGlobalBounds().contains(mousePos)) {
                createAndPlay(6); // Tạo map 6x6 -> Lưu -> Chơi
            }
            else if (m_btn8x8.getGlobalBounds().contains(mousePos)) {
                createAndPlay(8); // Tạo map 8x8 -> Lưu -> Chơi
            }
            else if (m_btn10x10.getGlobalBounds().contains(mousePos)) {
                createAndPlay(10); // Tạo map 10x10 -> Lưu -> Chơi
            }
            else if (m_btnBack.getGlobalBounds().contains(mousePos)) {
                m_states->pop();
            }
            isHandled = true;
        }
    }
    else {
        isHandled = false;
    }
}

void ModeSelectState::update(float dt) {
    updateButtons();
}

void ModeSelectState::render(sf::RenderWindow& window) {
    window.draw(*m_background);
    window.draw(m_title);
    window.draw(m_btn6x6);
    window.draw(m_btn8x8);
    window.draw(m_btn10x10);
    window.draw(m_btnBack);
}