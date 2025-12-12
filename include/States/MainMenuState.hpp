#pragma once
#include "States/State.hpp"
#include "States/ModeSelectState.hpp" // Để chuyển sang chọn màn
//#include "States/CustomizeState.hpp"
#include "States/SettingState.hpp"
#include <optional> // Để dùng std::optional


class MainMenuState : public State {
private:
    // --- BACKGROUND ---
    sf::Texture m_bgTexture;
    std::optional<sf::Sprite> m_bgSprite; // Dùng optional để tránh lỗi constructor

    // --- LOGO ---
	sf::Font m_font;
	std::optional<sf::Text> m_titleText;

    // --- BUTTON TEXTURES ---
    sf::Texture m_txPlay;
    sf::Texture m_txCustomize;
    sf::Texture m_txSettings;
    sf::Texture m_txHowToPlay;
    sf::Texture m_txExit;
	sf::Texture m_txResume;

    // --- BUTTON SPRITES ---
    // Dùng optional cho phép ta để trống nó lúc đầu
    std::optional<sf::Sprite> m_btnPlay;
    std::optional<sf::Sprite> m_btnCustomize;
    std::optional<sf::Sprite> m_btnSettings;
    std::optional<sf::Sprite> m_btnHowToPlay;
    std::optional<sf::Sprite> m_btnExit;
	std::optional<sf::Sprite> m_btnResume;

    // --- ÂM THANH ---
    sf::SoundBuffer m_buffHover, m_buffClick;
    std::optional<sf::Sound> m_sfxHover, m_sfxClick;

    // Biến ghi nhớ nút đang được hover (-1 là không có nút nào)
    int m_lastHoveredButton = -1;

    float m_totalTime = 0.f;
    bool m_isWaitingForMouseRelease = true;

    // --- LOGIC ---
    void initVariables();
    void initGui();
    void updateButtons(); // Xử lý hiệu ứng hover/click

public:
    MainMenuState(sf::RenderWindow* window, std::stack<std::unique_ptr<State>>* states);
    virtual ~MainMenuState();

    void update(float dt) override;
    void render(sf::RenderWindow& window) override;
};