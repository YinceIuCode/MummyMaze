#pragma once
#include "States/State.hpp"
#include "Core/GameData.hpp"

class SettingState : public State {
private:
    sf::Font m_font;
    sf::Text m_title;
    sf::Text m_volText;

    // --- SLIDER (Thanh trượt) ---
    sf::RectangleShape m_bar;  // Thanh ngang
    sf::RectangleShape m_knob; // Cục để kéo
    bool m_isDragging;         // Cờ kiểm tra đang giữ chuột

    // --- TOGGLE (Nút On/Off SFX) ---
    sf::RectangleShape m_sfxBtn;
    sf::Text m_sfxText;

    // Nút Back
    sf::Text m_backBtn;

    void initFonts();
    void initGui();

public:
    SettingState(sf::RenderWindow* window, std::stack<std::unique_ptr<State>>* states);
    virtual ~SettingState();

    void update(float dt) override;
    void render(sf::RenderWindow& window) override;
};