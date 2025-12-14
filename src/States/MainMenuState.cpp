#include "States/MainMenuState.hpp"
#include "States/ModeSelectState.hpp" // Nhớ include các state chuyển đến
#include "States/SettingState.hpp" 
#include "States/CustomizeState.hpp"
#include "States/HowtoplayState.hpp"
#include <fstream>

#include <iostream>

MainMenuState::MainMenuState(sf::RenderWindow* window, std::stack<std::unique_ptr<State>>* states)
    : State(window, states)
{
    initVariables();
    initGui();
}

MainMenuState::~MainMenuState() {
}

void MainMenuState::initVariables() {
    // 1. Load Background & Font (Giữ nguyên)
    if (m_bgTexture.loadFromFile("assets/textures/Backgrounds/menu_bg.png")) {
        m_bgTexture.setSmooth(true);
        m_bgSprite.emplace(m_bgTexture);
    }
    if (!m_font.openFromFile("assets/fonts/Akashi.ttf")) {
        std::cerr << "ERROR: Akashi.ttf\n";
    }
    m_titleText.emplace(m_font, "SYSTEM MAZE", 100);
    

    // 2. Load Textures (Giữ nguyên)
    if (!m_txPlay.loadFromFile("assets/textures/Menu/btn_play.png")) std::cerr << "Err Play\n";
    if (!m_txCustomize.loadFromFile("assets/textures/Menu/btn_customize.png")) std::cerr << "Err Customize\n";
    if (!m_txSettings.loadFromFile("assets/textures/Menu/btn_settings.png")) std::cerr << "Err Settings\n";
    if (!m_txHowToPlay.loadFromFile("assets/textures/Menu/btn_howtoplay.png")) std::cerr << "Err HTP\n";
    if (!m_txExit.loadFromFile("assets/textures/Menu/btn_exit.png")) std::cerr << "Err Exit\n";
    if (!m_txResume.loadFromFile("assets/textures/Menu/btn_resume.png")) std::cerr << "Err Resume\n";

    // 3. Load Audio (Giữ nguyên)
    if (!m_buffHover.loadFromFile("assets/audios/hover.wav")) std::cerr << "Err Hover\n";
    if (!m_buffClick.loadFromFile("assets/audios/click.wav")) std::cerr << "Err Click\n";
}

void MainMenuState::initGui() {
    float centerX = m_window->getSize().x / 2.f;
    float startY = 350.f;
    float gapY = 110.f;

    m_msgNoSave.emplace(m_font, "NO SAVE DATA!", 30);
    m_msgNoSave->setFillColor(sf::Color::Red);
    m_msgNoSave->setStyle(sf::Text::Bold);
    sf::FloatRect textBounds = m_msgNoSave->getLocalBounds();

    m_msgNoSave->setOrigin({ 0, textBounds.size.y / 2.f });
    m_msgNoSave->setPosition({ centerX + 200, startY + gapY - 10 });
    m_msgTimer = 0.f;

	m_HowToPlayText.emplace(m_font, "HOW TO PLAY", 20);
	m_HowToPlayText->setFillColor(sf::Color::White);
	m_HowToPlayText->setOrigin({ m_HowToPlayText->getLocalBounds().size.x, m_HowToPlayText->getLocalBounds().size.y / 2});
	m_HowToPlayText->setPosition({ m_window->getSize().x - 60.f * 2 - 90.f, 50.f});
	m_HowToPlayText->setStyle(sf::Text::Underlined | sf::Text::Bold);

    // Setup Title
    if (m_titleText.has_value()) {
        sf::FloatRect bounds = m_titleText->getLocalBounds();
        m_titleText->setOrigin({ bounds.size.x / 2.f, bounds.size.y / 2.f });
        m_titleText->setPosition({ centerX, 200.f });
    }

    // --- KHỞI TẠO NÚT BẰNG CLASS MỚI ---

    // Nút Play (To gấp đôi, có hiệu ứng thở)
    m_btnPlay = std::make_unique<Button>(m_txPlay, centerX, startY + gapY * 2, 3.0f);
    m_btnPlay->setSound(m_buffHover, m_buffClick);
    m_btnPlay->setBreathing(true); // <--- Bật hiệu ứng thở

    // Các nút phụ (Kích thước 1.0)
    m_btnResume = std::make_unique<Button>(m_txResume, centerX, startY + gapY, 2.5f);
    m_btnResume->setSound(m_buffHover, m_buffClick);

    m_btnCustomize = std::make_unique<Button>(m_txCustomize, centerX, startY + gapY * 3, 2.5f);
    m_btnCustomize->setSound(m_buffHover, m_buffClick);

    m_btnExit = std::make_unique<Button>(m_txExit, centerX, startY + gapY * 4, 2.5f);
    m_btnExit->setSound(m_buffHover, m_buffClick);

    m_btnSettings = std::make_unique<Button>(m_txSettings, m_window->getSize().x - 60.f, 60.f, 2.0f);
    m_btnSettings->setSound(m_buffHover, m_buffClick);

    m_btnHowToPlay = std::make_unique<Button>(m_txHowToPlay, m_window->getSize().x - 60.f * 2 - 30.f, 60.f, 2.0f);
    m_btnHowToPlay->setSound(m_buffHover, m_buffClick);
}

void MainMenuState::updateButtons() {
    sf::Vector2f mousePos = m_window->mapPixelToCoords(sf::Mouse::getPosition(*m_window));
    bool isMousePressed = sf::Mouse::isButtonPressed(sf::Mouse::Button::Left);

    // 1. Chặn click thừa từ màn hình trước
    if (isMousePressed && m_isWaitingForMouseRelease) return;
    if (!isMousePressed) m_isWaitingForMouseRelease = false;

    // 2. CẬP NHẬT TRẠNG THÁI VISUAL (Hover, Breathing...)
    m_btnPlay->update(mousePos, isMousePressed, m_totalTime);
    m_btnResume->update(mousePos, isMousePressed, m_totalTime);
    m_btnCustomize->update(mousePos, isMousePressed, m_totalTime);
    m_btnSettings->update(mousePos, isMousePressed, m_totalTime);
    m_btnHowToPlay->update(mousePos, isMousePressed, m_totalTime);
    m_btnExit->update(mousePos, isMousePressed, m_totalTime);

    // 3. XỬ LÝ CLICK (Chỉ viết 1 lần duy nhất ở đây)
    static bool isHandled = false;

    if (isMousePressed && !isHandled) {

        // Helper lambda
        auto triggerButton = [&](std::unique_ptr<Button>& btn, auto action) {
            btn->executeClickEffect(
                [&]() { // Draw Func
                    m_window->clear();
                    this->render(*m_window);
                    m_window->display();
                },
                action // On Finished Func
            );
            };

        // --- CÁC NÚT CHUYỂN MÀN ---

        // 1. PLAY
        if (m_btnPlay->isClicked()) {
            isHandled = true;
            triggerButton(m_btnPlay, [&]() {
                m_states->push(std::make_unique<ModeSelectState>(m_window, m_states));
                });
        }
        // 2. SETTINGS
        else if (m_btnSettings->isClicked()) {
            isHandled = true;
            triggerButton(m_btnSettings, [&]() {
                m_states->push(std::make_unique<SettingState>(m_window, m_states));
                });
        }
        // 3. EXIT
        else if (m_btnExit->isClicked()) {
            isHandled = true;
            triggerButton(m_btnExit, [&]() {
                m_window->close();
                });
        }
        // 4. CUSTOMIZE
        else if (m_btnCustomize->isClicked()) {
            isHandled = true;
            triggerButton(m_btnCustomize, [&]() {
                m_states->push(std::make_unique<CustomizeState>(m_window, m_states));
                });
        }
        // 5. HOW TO PLAY
        else if (m_btnHowToPlay->isClicked()) {
            isHandled = true;
            triggerButton(m_btnHowToPlay, [&]() {
                m_states->push(std::make_unique<HowToPlayState>(m_window, m_states));
                });
        }
        // 6. RESUME (Logic đã fix lỗi file rỗng)
        else if (m_btnResume->isClicked()) {
            std::ifstream checkFile("assets/mazes/mazesave.txt");
            bool hasData = false;

            // Kiểm tra file tồn tại VÀ có dữ liệu
            if (checkFile.is_open()) {
                if (checkFile.peek() != std::ifstream::traits_type::eof()) {
                    hasData = true;
                }
                checkFile.close();
            }

            if (hasData) {
                // Vào game
                m_states->push(std::make_unique<GameState>(m_window, m_states, "", true));
            }
            else {
                // Báo lỗi
                std::cout << "No valid save file found!\n";
                m_msgTimer = 2.0f;
                if (m_msgNoSave.has_value()) {
                    sf::Color c = m_msgNoSave->getFillColor();
                    c.a = 255;
                    m_msgNoSave->setFillColor(c);
                }
            }
        }
    }

    // Reset cờ khi nhả chuột
    if (!isMousePressed) isHandled = false;
}

void MainMenuState::update(float dt) {
    m_totalTime += dt; // Tăng thời gian để tính hiệu ứng thở
    updateButtons();

    if (m_msgTimer > 0.f) {
        m_msgTimer -= dt;
        int alpha = static_cast<int>((m_msgTimer / 2.0f) * 255.f);
        if (alpha < 0) alpha = 0; if (alpha > 255) alpha = 255;

        // Kiểm tra an toàn trước khi chỉnh
        if (m_msgNoSave.has_value()) {
            sf::Color c = m_msgNoSave->getFillColor();
            c.a = alpha;
            m_msgNoSave->setFillColor(c);
        }
    }
}

void MainMenuState::render(sf::RenderWindow& window) {
    if (m_bgSprite.has_value()) window.draw(*m_bgSprite);
    if (m_titleText.has_value()) window.draw(*m_titleText);
	if (m_HowToPlayText.has_value()) window.draw(*m_HowToPlayText);

    // Vẽ nút bằng hàm render của class Button
    m_btnPlay->render(window);
    m_btnResume->render(window);
    m_btnCustomize->render(window);
    m_btnSettings->render(window);
    m_btnHowToPlay->render(window);
    m_btnExit->render(window);
    if (m_msgTimer > 0.f && m_msgNoSave.has_value()) {
        window.draw(*m_msgNoSave);
    }
}