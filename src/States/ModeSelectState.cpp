#include "States/ModeSelectState.hpp"
#include <iostream>

ModeSelectState::ModeSelectState(sf::RenderWindow* window, std::stack<std::unique_ptr<State>>* states)
    : State(window, states),
    m_title(m_font), m_btn6x6(m_font), m_btn8x8(m_font), m_btn10x10(m_font), m_btnBack(m_font)
{
    initFonts();
    initGui();
}

ModeSelectState::~ModeSelectState() {}

void ModeSelectState::initFonts() {
    if (!m_font.openFromFile("assets/fonts/arial.ttf")) {
        std::cerr << "ERROR: Could not load font\n";
    }
}

void ModeSelectState::initGui() {
    // Setup cơ bản (bạn có thể căn chỉnh lại tọa độ cho đẹp)
    m_title.setString("SELECT MODE");
    m_title.setCharacterSize(60);
    m_title.setPosition({ 450.f, 100.f });

    m_btn6x6.setString("6 x 6");
    m_btn6x6.setPosition({ 550.f, 300.f });

    m_btn8x8.setString("8 x 8");
    m_btn8x8.setPosition({ 550.f, 400.f });

    m_btn10x10.setString("10 x 10");
    m_btn10x10.setPosition({ 550.f, 500.f });

    m_btnBack.setString("BACK");
    m_btnBack.setPosition({ 50.f, 50.f });
}

void ModeSelectState::updateButtons() {
    sf::Vector2f mousePos = m_window->mapPixelToCoords(sf::Mouse::getPosition(*m_window));
    static bool isHandled = false;

    // Hover effect (đổi màu khi chuột chỉ vào)
    auto handleHover = [&](sf::Text& txt) {
        if (txt.getGlobalBounds().contains(mousePos)) txt.setFillColor(sf::Color::Red);
        else txt.setFillColor(sf::Color::White);
        };
    handleHover(m_btn6x6); handleHover(m_btn8x8); handleHover(m_btn10x10); handleHover(m_btnBack);

    // Click effect
    if (sf::Mouse::isButtonPressed(sf::Mouse::Button::Left)) {
        if (!isHandled) {
            if (m_btn6x6.getGlobalBounds().contains(mousePos)) {
                // TRUYỀN SIZE 6 VÀO GAMESTATE
                m_states->push(std::make_unique<GameState>(m_window, m_states, 6));
            }
            else if (m_btn8x8.getGlobalBounds().contains(mousePos)) {
                m_states->push(std::make_unique<GameState>(m_window, m_states, 8));
            }
            else if (m_btn10x10.getGlobalBounds().contains(mousePos)) {
                m_states->push(std::make_unique<GameState>(m_window, m_states, 10));
            }
            else if (m_btnBack.getGlobalBounds().contains(mousePos)) {
                m_states->pop();
            }
            isHandled = true;
        }
    }
    else {
        isHandled = false;
    }
}

void ModeSelectState::update(float dt) {
    updateButtons();
}

void ModeSelectState::render(sf::RenderWindow& window) {
    window.draw(m_title);
    window.draw(m_btn6x6);
    window.draw(m_btn8x8);
    window.draw(m_btn10x10);
    window.draw(m_btnBack);
}