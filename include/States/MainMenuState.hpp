#pragma once
#include "../../include/States/State.hpp"
#include "../../include/GUI/Button.hpp"
#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>

class MainMenuState : public State {
private:
    sf::Texture m_bgTexture;
    std::optional<sf::Sprite> m_bgSprite;
    sf::Font m_font;
    std::optional<sf::Text> m_titleText;
    std::optional<sf::Text> m_msgNoSave;
    std::optional<sf::Text> m_HowToPlayText;
    float m_msgTimer = 0.f;

    std::unique_ptr<Button> m_btnPlay;
    std::unique_ptr<Button> m_btnCustomize;
    std::unique_ptr<Button> m_btnSettings;
    std::unique_ptr<Button> m_btnHowToPlay;
    std::unique_ptr<Button> m_btnExit;
    std::unique_ptr<Button> m_btnResume;

    sf::Texture m_txPlay, m_txCustomize, m_txSettings, m_txHowToPlay, m_txExit, m_txResume;

    sf::SoundBuffer m_buffHover;
    sf::SoundBuffer m_buffClick;

    float m_totalTime = 0.f;
    bool m_isWaitingForMouseRelease = true;

    void initVariables();
    void initGui();
    void updateButtons();

public:
    MainMenuState(sf::RenderWindow* window, std::stack<std::unique_ptr<State>>* states);
    virtual ~MainMenuState();

    void update(float dt) override;
    void render(sf::RenderWindow& window) override;
};