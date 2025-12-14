#include "States/HowToPlayState.hpp"
#include "Core/GameData.hpp"
#include <iostream>

HowToPlayState::HowToPlayState(sf::RenderWindow* window, std::stack<std::unique_ptr<State>>* states)
    : State(window, states)
{
    initVariables();
    initGui();
}

HowToPlayState::~HowToPlayState() {}

void HowToPlayState::initVariables() {
    // 1. Load Font & Background
    if (!m_font.openFromFile("assets/fonts/Akashi.ttf")) std::cerr << "Err Font\n";
    if (m_bgTexture.loadFromFile("assets/textures/Backgrounds/menu_bg.png")) {
        m_bgSprite.emplace(m_bgTexture);
    }
    if (!m_buffClick.loadFromFile("assets/audios/click.wav")) {
		std::cerr << "Err Click Sound\n";
    }
}

void HowToPlayState::initGui() {
    float cx = m_window->getSize().x / 2.f;
    float cy = m_window->getSize().y / 2.f;

    // --- 1. TITLE ---
    m_title.emplace(m_font, "HOW TO PLAY", 80);
    sf::FloatRect tb = m_title->getLocalBounds();
    m_title->setOrigin({ tb.size.x / 2.f, 0 });
    m_title->setPosition({ cx, 60.f });

    // --- 2. NÚT BACK ---
    m_btnBack.emplace(m_font, "BACK", 50);
    m_btnBack->setPosition({ 80.f, 60.f });

    // --- 3. KHUNG CHỨA (Container) ---
    // Vẽ một hình chữ nhật màu đen mờ để chữ dễ đọc hơn
    m_container.setSize({ 1000.f, 500.f });
    m_container.setFillColor(sf::Color(0, 0, 0, 150)); // Đen, Alpha = 150 (Trong suốt)
    m_container.setOutlineColor(sf::Color::Cyan);
    m_container.setOutlineThickness(2.f);
    m_container.setOrigin({ 500.f, 250.f }); 
    m_container.setPosition({ cx, cy + 30.f });

    // --- 4. NỘI DUNG HƯỚNG DẪN ---
    // Định nghĩa nội dung: { "PHÍM", "CHỨC NĂNG" }
    std::vector<std::pair<std::string, std::string>> content = {
        { "WASD / ARROWS",  "Move Character" },
        { "Key 'U'",        "Undo (Step Back)" },
        { "Key 'I'",        "Redo (Step Forward)" },
        { "Key 'P'",        "Save Game" },
		{ "Key 'R'",        "Reset Level" }, 
        { "Goal",           "Reach the Exit" }
    };

    float startY = cy - 150.f; 
    float gapY = 70.f;         

    for (int i = 0; i < content.size(); i++) {
        sf::Text keyText(m_font, content[i].first, 40);
        sf::Text descText(m_font, content[i].second, 40);

        // Style cho Phím (Bên Trái - Màu Vàng)
        keyText.setFillColor(sf::Color::Yellow);
        sf::FloatRect kb = keyText.getLocalBounds();
        keyText.setOrigin({ kb.size.x, kb.size.y / 2.f }); // Căn lề phải
        keyText.setPosition({ cx - 30.f, startY + i * gapY });

        // Style cho Mô tả (Bên Phải - Màu Trắng)
        descText.setFillColor(sf::Color::White);
        sf::FloatRect db = descText.getLocalBounds();
        descText.setOrigin({ 0, db.size.y / 2.f }); // Căn lề trái
        descText.setPosition({ cx + 20.f, startY + i * gapY });

        // Đẩy vào vector để quản lý vẽ
        m_instructions.push_back({ keyText, descText });
    }
}

void HowToPlayState::updateInput(float dt) {
    sf::Vector2f mousePos = m_window->mapPixelToCoords(sf::Mouse::getPosition(*m_window));

    // Logic nút BACK
    if (m_btnBack.has_value()) {
        if (m_btnBack->getGlobalBounds().contains(mousePos)) {
            m_btnBack->setFillColor(sf::Color::Yellow); // Hover

            if (sf::Mouse::isButtonPressed(sf::Mouse::Button::Left)) {
                // Click
                if (!GameData::isSfxMuted) {
                    static sf::Sound snd(m_buffClick);
                    if (snd.getStatus() != sf::Sound::Status::Playing) snd.play();
                }

                sf::sleep(sf::milliseconds(150));
                m_states->pop();
            }
        }
        else {
            m_btnBack->setFillColor(sf::Color::White);
        }
    }
}

void HowToPlayState::update(float dt) {
    updateInput(dt);
}

void HowToPlayState::render(sf::RenderWindow& window) {
    if (m_bgSprite.has_value()) window.draw(*m_bgSprite);
    if (m_title.has_value()) window.draw(*m_title);

    // Vẽ khung mờ trước
    window.draw(m_container);

    // Vẽ nội dung
    for (const auto& line : m_instructions) {
        window.draw(line.first);  // Vẽ Phím
        window.draw(line.second); // Vẽ Mô tả
    }

    if (m_btnBack.has_value()) window.draw(*m_btnBack);
}