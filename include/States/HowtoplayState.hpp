#pragma once
#include "States/State.hpp"
#include <vector>

class HowToPlayState : public State {
private:
    sf::Texture m_bgTexture;
    std::optional<sf::Sprite> m_bgSprite;
    sf::Font m_font;

    std::optional<sf::Text> m_title;
    std::optional<sf::Text> m_btnBack;

    sf::RectangleShape m_container;

    std::vector<std::pair<sf::Text, sf::Text>> m_instructions;

    sf::SoundBuffer m_buffClick;

    void initVariables();
    void initGui();
    void updateInput(float dt);

public:
    HowToPlayState(sf::RenderWindow* window, std::stack<std::unique_ptr<State>>* states);
    virtual ~HowToPlayState();

    void update(float dt) override;
    void render(sf::RenderWindow& window) override;
};