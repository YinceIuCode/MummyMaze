#include "States/MainMenuState.hpp"
#include <iostream>

MainMenuState::MainMenuState(sf::RenderWindow* window, std::stack<std::unique_ptr<State>>* states)
    : State(window, states)
{
    // Không cần khởi tạo Sprite trong này nữa vì đã dùng std::optional
    initVariables();
    initGui();
}

MainMenuState::~MainMenuState() {
}

void MainMenuState::initVariables() {
    // 1. Load Background
    if (m_bgTexture.loadFromFile("assets/textures/Backgrounds/menu_bg.png")) {
        m_bgTexture.setSmooth(true);
        m_bgSprite.emplace(m_bgTexture);
    }

    // 2. Load Textures cho các nút
    // Bạn nhớ kiểm tra đúng tên file ảnh nhé
    if (!m_txPlay.loadFromFile("assets/textures/Menu/btn_play.png")) {
        std::cerr << "ERROR: Could not load btn_play.png\n";
    }
    if (!m_txCustomize.loadFromFile("assets/textures/Menu/btn_customize.png")) {
        std::cerr << "ERROR: Could not load btn_customize.png\n";
    }
    if (!m_txSettings.loadFromFile("assets/textures/Menu/btn_settings.png")) {
        std::cerr << "ERROR: Could not load btn_settings.png\n";
    }
    if (!m_txHowToPlay.loadFromFile("assets/textures/Menu/btn_howtoplay.png")) {
        std::cerr << "ERROR: Could not load btn_howtoplay.png\n";
    }
    if (!m_txExit.loadFromFile("assets/textures/Menu/btn_exit.png")) {
        std::cerr << "ERROR: Could not load btn_exit.png\n";
    }

    // 3. Gán Texture vào Sprite
    m_btnPlay.emplace(m_txPlay);
    m_btnCustomize.emplace(m_txCustomize);
    m_btnSettings.emplace(m_txSettings);
    m_btnHowToPlay.emplace(m_txHowToPlay);
    m_btnExit.emplace(m_txExit);
}

void MainMenuState::initGui() {
    float centerX = m_window->getSize().x / 2.f;
    float startY = 300.f; // Vị trí Y bắt đầu

    // Hàm lambda để setup nút cho nhanh (đỡ viết lặp lại)
    auto setupBtn = [&](std::optional<sf::Sprite>& btn, float xPos, float yPos) {
        if (btn.has_value()) {
            sf::FloatRect bounds = btn->getLocalBounds();
            btn->setOrigin({ bounds.size.x / 2.f, bounds.size.y / 2.f }); // Tâm ở giữa
            btn->setPosition({ xPos, yPos });
        }
    };

    setupBtn(m_btnPlay, centerX, m_window->getSize().y / 2 + m_btnPlay->getGlobalBounds().size.y / 2);
    setupBtn(m_btnCustomize, centerX, m_window->getSize().y / 2 + m_btnPlay->getGlobalBounds().size.y + m_btnCustomize->getGlobalBounds().size.y / 2 + 20);
    setupBtn(m_btnSettings, m_window->getSize().x - m_btnSettings->getGlobalBounds().size.x, 30);
    setupBtn(m_btnHowToPlay, m_window->getSize().x - m_btnHowToPlay->getGlobalBounds().size.x - 15, m_btnHowToPlay->getGlobalBounds().size.y + 10);
    setupBtn(m_btnExit, m_btnExit->getGlobalBounds().size.x, m_btnExit->getGlobalBounds().size.y + 10);
}

void MainMenuState::updateButtons() {
    sf::Vector2f mousePos = m_window->mapPixelToCoords(sf::Mouse::getPosition(*m_window));
    static bool isHandled = false; // Chặn click đúp

    // --- HOVER EFFECT (Phóng to khi chuột chỉ vào) ---
    auto hoverEffect = [&](std::optional<sf::Sprite>& btn, float baseScale = 1.0f) {
        if (!btn.has_value()) return;

        if (btn->getGlobalBounds().contains(mousePos)) {
            btn->setScale({ baseScale * 1.1f, baseScale * 1.1f }); // Phóng to 10%
            btn->setColor(sf::Color(255, 255, 255, 255)); // Sáng bình thường
        }
        else {
            btn->setScale({ baseScale * 1.0f, baseScale * 1.0f }); // Kích thước gốc
            btn->setColor(sf::Color(200, 200, 200, 255));
        }
    };

    hoverEffect(m_btnPlay, 2.0f);
    hoverEffect(m_btnCustomize);
    hoverEffect(m_btnSettings, 2.0f);
    hoverEffect(m_btnHowToPlay);
    hoverEffect(m_btnExit);

    // --- CLICK EVENT ---
    if (sf::Mouse::isButtonPressed(sf::Mouse::Button::Left)) {
        if (!isHandled) {

            // 1. PLAY -> Vào màn hình chọn chế độ
            if (m_btnPlay->getGlobalBounds().contains(mousePos)) {
                sf::sleep(sf::milliseconds(500));
                m_states->push(std::make_unique<ModeSelectState>(m_window, m_states));
            }

            // 2. CUSTOMIZE -> Vào màn hình đổi skin
            /*else if (m_btnCustomize->getGlobalBounds().contains(mousePos)) {
                m_states->push(std::make_unique<CustomizeState>(m_window, m_states));
            }*/

            // 3. SETTINGS -> Vào cài đặt
            else if (m_btnSettings->getGlobalBounds().contains(mousePos)) {
                m_states->push(std::make_unique<SettingState>(m_window, m_states));
            }

            // 4. EXIT -> Thoát game
            else if (m_btnExit->getGlobalBounds().contains(mousePos)) {
                m_window->close();
            }

            isHandled = true;
        }
    }
    else {
        isHandled = false;
    }
}

void MainMenuState::update(float dt) {
    updateButtons();
}

void MainMenuState::render(sf::RenderWindow& window) {
    // Vẽ background
    if (m_bgSprite.has_value()) window.draw(*m_bgSprite);

    // Vẽ các nút (Nhớ dùng dấu * để lấy giá trị từ optional)
    if (m_btnPlay.has_value()) window.draw(*m_btnPlay);
    if (m_btnCustomize.has_value()) window.draw(*m_btnCustomize);
    if (m_btnSettings.has_value()) window.draw(*m_btnSettings);
    if (m_btnHowToPlay.has_value()) window.draw(*m_btnHowToPlay);
    if (m_btnExit.has_value()) window.draw(*m_btnExit);
}