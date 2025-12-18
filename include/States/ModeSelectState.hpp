#pragma once
#include "../../include/States/State.hpp"
#include "../../include/States/GameState.hpp"
#include <SFML/Audio.hpp>

class ModeSelectState : public State {
private:
    sf::Font m_font;
    sf::Text m_title;
    sf::Text m_btn6x6;
    sf::Text m_btn8x8;
    sf::Text m_btn10x10;
    sf::Text m_btnBack;
	sf::Texture m_bgTexture;
    std::optional<sf::Text> m_loadingText;
	std::optional<sf::Sprite> m_bgSprite;
    int m_lastThemeId;
    sf::RectangleShape m_darkLayer;

    sf::SoundBuffer m_buffHover;
    sf::SoundBuffer m_buffClick;
    std::optional<sf::Sound> m_sfx;

    sf::Text* m_currentHoveredBtn = nullptr;
    bool m_isClick = false;

    void initFonts();
    void initGui();
    void updateButtons();
	void reloadBackground();

public:
    ModeSelectState(sf::RenderWindow* window, std::stack<std::unique_ptr<State>>* states);
    virtual ~ModeSelectState();

    void update(float dt) override;
    void render(sf::RenderWindow& window) override;
};