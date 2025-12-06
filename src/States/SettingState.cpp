#include "States/SettingState.hpp"
#include <iostream>

SettingState::SettingState(sf::RenderWindow* window, std::stack<std::unique_ptr<State>>* states)
    : State(window, states),
    m_title(m_font),
    m_volText(m_font),
    m_sfxText(m_font),
    m_backBtn(m_font)
{
    m_isDragging = false;
    initFonts();
    initGui();
}

SettingState::~SettingState() {}

void SettingState::initFonts() {
    if (!m_font.openFromFile("assets/fonts/arial.ttf")) {
        return;
    }
}

void SettingState::initGui() {
    // 1. Tiêu đề
    m_title.setFont(m_font);
    m_title.setString("SETTINGS");
    m_title.setCharacterSize(50);
    m_title.setPosition({ 400.f, 50.f });

    // 2. Setup Thanh Bar (Màu xám)
    m_bar.setSize({ 400.f, 10.f });
    m_bar.setFillColor(sf::Color(150, 150, 150));
    m_bar.setPosition({ 300.f, 200.f });

    // 3. Setup Cục Knob (Màu đỏ)
    m_knob.setSize({ 20.f, 30.f });
    m_knob.setFillColor(sf::Color::Red);
    m_knob.setOrigin({ 10.f, 15.f }); // Tâm ở giữa cục

    // Đặt vị trí Knob dựa trên Volume hiện tại (từ GameData)
    float percent = GameData::masterVolume / 100.f;
    float barX = m_bar.getPosition().x;
    float barWidth = m_bar.getSize().x;
    m_knob.setPosition({ barX + (barWidth * percent), 200.f + 5.f });

    // Text hiển thị số %
    m_volText.setFont(m_font);
    m_volText.setString("Volume: " + std::to_string((int)GameData::masterVolume));
    m_volText.setPosition({ 300.f, 150.f });

    // 4. Setup Nút SFX
    m_sfxBtn.setSize({ 50.f, 50.f });
    m_sfxBtn.setPosition({ 300.f, 300.f });
    m_sfxBtn.setFillColor(GameData::isSoundOn ? sf::Color::Green : sf::Color::Red);

    m_sfxText.setFont(m_font);
    m_sfxText.setString("SFX On/Off");
    m_sfxText.setPosition({ 370.f, 310.f });

    // 5. Nút Back
    m_backBtn.setFont(m_font);
    m_backBtn.setString("BACK");
    m_backBtn.setPosition({ 50.f, 50.f });
}

void SettingState::update(float dt) {
    // Lấy vị trí chuột trong View (để scale window không bị lệch)
    sf::Vector2f mousePos = m_window->mapPixelToCoords(sf::Mouse::getPosition(*m_window));

    // --- LOGIC SLIDER (KÉO THẢ) ---
    if (sf::Mouse::isButtonPressed(sf::Mouse::Button::Left)) {
        // Nếu click trúng Knob HOẶC Click trúng thanh Bar HOẶC đang Drag
        if (m_knob.getGlobalBounds().contains(mousePos) ||
            m_bar.getGlobalBounds().contains(mousePos) ||
            m_isDragging)
        {
            m_isDragging = true;

            float barLeft = m_bar.getPosition().x;
            float barRight = barLeft + m_bar.getSize().x;
            float newX = mousePos.x;

            // Giới hạn không cho kéo ra ngoài
            if (newX < barLeft) newX = barLeft;
            if (newX > barRight) newX = barRight;

            m_knob.setPosition({ newX, m_knob.getPosition().y });

            // Tính toán lại Volume
            float percent = (newX - barLeft) / (barRight - barLeft);
            GameData::masterVolume = percent * 100.f;

            // Cập nhật text
            m_volText.setString("Volume: " + std::to_string((int)GameData::masterVolume));
        }

        // --- LOGIC BACK BUTTON ---
        if (m_backBtn.getGlobalBounds().contains(mousePos)) {
            m_states->pop(); // Quay về menu
            sf::sleep(sf::milliseconds(200));
            return; // Return ngay để tránh lỗi pointer
        }
    }
    else {
        m_isDragging = false; // Nhả chuột ra thì hết drag
    }

    // --- LOGIC SFX TOGGLE (Chỉ bắt sự kiện Click 1 lần) ---
    static bool isHandled = false; // Biến static để chặn click liên tục
    if (sf::Mouse::isButtonPressed(sf::Mouse::Button::Left)) {
        if (!isHandled) {
            if (m_sfxBtn.getGlobalBounds().contains(mousePos)) {
                GameData::isSoundOn = !GameData::isSoundOn;
                m_sfxBtn.setFillColor(GameData::isSoundOn ? sf::Color::Green : sf::Color::Red);
            }
            isHandled = true;
        }
    }
    else {
        isHandled = false;
    }
}

void SettingState::render(sf::RenderWindow& window) {
    window.draw(m_title);

    // Vẽ Slider
    window.draw(m_bar);
    window.draw(m_knob);
    window.draw(m_volText);

    // Vẽ Toggle
    window.draw(m_sfxBtn);
    window.draw(m_sfxText);

    window.draw(m_backBtn);
}