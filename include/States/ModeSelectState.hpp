#pragma once
#include "States/State.hpp"
#include "States/GameState.hpp"

class ModeSelectState : public State {
private:
    sf::Font m_font;
    sf::Text m_title;
    sf::Text m_btn6x6;
    sf::Text m_btn8x8;
    sf::Text m_btn10x10;
    sf::Text m_btnBack;

    void initFonts();
    void initGui();
    void updateButtons(); // Xử lý hover/click

public:
    ModeSelectState(sf::RenderWindow* window, std::stack<std::unique_ptr<State>>* states);
    virtual ~ModeSelectState();

    void update(float dt) override;
    void render(sf::RenderWindow& window) override;
};