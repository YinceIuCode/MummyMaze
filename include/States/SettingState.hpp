#pragma once
#include "States/State.hpp"
#include "GUI/Button.hpp"
#include "Core/GameData.hpp"

class SettingState : public State {
private:
    // --- NỀN & FONT ---
    sf::Texture m_bgTexture;
    std::optional<sf::Sprite> m_bgSprite;
    sf::Font m_font;
    std::optional<sf::Text> m_title;
    std::optional<sf::Text> m_btnBack;

    // --- SLIDER (THANH KÉO NHẠC) ---
    sf::Texture m_txBar;   // Ảnh thanh dài
    sf::Texture m_txKnob;  // Ảnh cục nút tròn để kéo
    std::optional<sf::Sprite> m_sprBar;
    std::optional<sf::Sprite> m_sprKnob;
    std::optional<sf::Text> m_txtMusicLabel;
    sf::RectangleShape m_barFill;

    bool m_isDragging = false; // Cờ kiểm tra xem có đang giữ chuột không

    // --- TOGGLE (BẬT TẮT SFX) ---
    sf::Texture m_txTick;  // Ảnh dấu V
    sf::Texture m_txCross; // Ảnh dấu X
    std::optional<sf::Sprite> m_sprSfxToggle; // Sprite hiển thị Tick hoặc Cross
    std::optional<sf::Text> m_txtSfxLabel;

    // --- SOUND ---
    sf::SoundBuffer m_buffClick; // Để test âm thanh

    void initVariables();
    void initGui();
    void updateButtons();

public:
    SettingState(sf::RenderWindow* window, std::stack<std::unique_ptr<State>>* states);
    virtual ~SettingState();

    void update(float dt) override;
    void render(sf::RenderWindow& window) override;
};