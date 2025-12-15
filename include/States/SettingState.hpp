#pragma once
#include "States/State.hpp"
#include "GUI/Button.hpp"
#include "Core/GameData.hpp"

class SettingState : public State {
private:
    sf::Texture m_bgTexture;
    std::optional<sf::Sprite> m_bgSprite;
    sf::Font m_font;
    std::optional<sf::Text> m_title;
    std::optional<sf::Text> m_btnBack;

    sf::Texture m_txBar;
    sf::Texture m_txKnob;
    std::optional<sf::Sprite> m_sprBar;
    std::optional<sf::Sprite> m_sprKnob;
    std::optional<sf::Text> m_txtMusicLabel;
    sf::RectangleShape m_barFill;

    bool m_isDragging = false;

    sf::Texture m_txTick;
    sf::Texture m_txCross;
    std::optional<sf::Sprite> m_sprSfxToggle;
    std::optional<sf::Text> m_txtSfxLabel;

    sf::SoundBuffer m_buffClick;

    void initVariables();
    void initGui();
    void updateButtons();

public:
    SettingState(sf::RenderWindow* window, std::stack<std::unique_ptr<State>>* states);
    virtual ~SettingState();

    void update(float dt) override;
    void render(sf::RenderWindow& window) override;
};