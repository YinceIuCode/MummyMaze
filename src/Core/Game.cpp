#include "Core/Game.hpp"
#include "States/MainMenuState.hpp"

void Game::initStates() {
    // Mở game lên là vào Menu đầu tiên
    m_states.push(std::make_unique<MainMenuState>(&m_window, &m_states));
}

void Game::initWindow() {
    m_videoMode = sf::VideoMode({ 1290, 960 });
    m_window.create(m_videoMode, "Mummy Game");
    m_window.setFramerateLimit(60);

    initStates();
}

Game::Game() {
    this->initWindow();
    this->initStates();
}

Game::~Game() {
    while (!m_states.empty()) m_states.pop();
}

const bool Game::isRunning() const {
    return m_window.isOpen();
}

void Game::updateDt() {
    m_dt = m_dtClock.restart().asSeconds();
}

void Game::update() {
    this->updateDt();

    while (const std::optional event = m_window.pollEvent()) {
        if (event->is<sf::Event::Closed>()) m_window.close();
    }

    if (!m_states.empty()) {
        m_states.top()->update(m_dt);
    }
    else {
        m_window.close(); // Hết màn hình thì tắt game
    }
}

void Game::render() {
    m_window.clear(sf::Color::Black);
    if (!m_states.empty()) {
        m_states.top()->render(m_window);
    }
    m_window.display();
}