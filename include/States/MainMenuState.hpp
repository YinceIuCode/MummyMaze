#pragma once
#include "States/State.hpp"
#include "GUI/Button.hpp" // <--- BẮT BUỘC PHẢI CÓ
#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>

class MainMenuState : public State {
private:
    // --- VARIABLES ---
    sf::Texture m_bgTexture;
    std::optional<sf::Sprite> m_bgSprite;
    sf::Font m_font;
    std::optional<sf::Text> m_titleText;
    std::optional<sf::Text> m_msgNoSave;
	std::optional<sf::Text> m_HowToPlayText;
    float m_msgTimer = 0.f;

    // --- BUTTONS (Dùng Class Button mới) ---
    // Thay toàn bộ std::optional<sf::Sprite> bằng cái này
    std::unique_ptr<Button> m_btnPlay;
    std::unique_ptr<Button> m_btnCustomize;
    std::unique_ptr<Button> m_btnSettings;
    std::unique_ptr<Button> m_btnHowToPlay;
    std::unique_ptr<Button> m_btnExit;
    std::unique_ptr<Button> m_btnResume;

    // --- TEXTURES (Giữ nguyên để load ảnh) ---
    sf::Texture m_txPlay, m_txCustomize, m_txSettings, m_txHowToPlay, m_txExit, m_txResume;

    // --- SOUNDS (Chỉ cần Buffer, Button tự lo phần phát) ---
    sf::SoundBuffer m_buffHover;
    sf::SoundBuffer m_buffClick;

    // --- STATE DATA ---
    float m_totalTime = 0.f;
    bool m_isWaitingForMouseRelease = true; // Chống click nhầm từ màn trước

    // --- FUNCTIONS ---
    void initVariables();
    void initGui();
    void updateButtons(); // Hàm này giờ sẽ gọn hơn nhiều

public:
    MainMenuState(sf::RenderWindow* window, std::stack<std::unique_ptr<State>>* states);
    virtual ~MainMenuState();

    void update(float dt) override;
    void render(sf::RenderWindow& window) override;
};